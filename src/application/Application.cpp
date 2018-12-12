#include "pch.h"
#include "Application.h"

#include "../utils/Log.h"
#include "../utils/Exceptions.h"

#include <iostream>
#include <algorithm>

#include "../graphics/Camera.h"
#include "../graphics/Shader.h"
#include "../world/ChunkMesh.h"
#include "../world/Chunk.h"
#include "../world/ChunkColumn.h"
#include "../world/World.h"
#include "../debug.h"
#include "../graphics/TextureAtlas.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

namespace tk
{
Application::Application()
{
    try
    {
        if (glfwInit() == GLFW_FALSE)
            throw RuntimeException("Unable to initialize GLFW");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        Log::init();
        LOG_INFO("Application started");

        glfwSetErrorCallback(glfw_error_callback);
    }
    catch (RuntimeException &e)
    {
        std::cout << "Can't start application: " << e.what() << std::endl;
        throw;
    }
}

Application::~Application()
{
    LOG_INFO("Application terminating...");
    m_windows.clear();
    glfwTerminate();
}

//public:

void Application::runLoop()
{
    m_windows[0].second->bindContext();

    Shader s("res/shaders/default3D.vert", "res/shaders/default3D.frag");
    s.setUniform1i("textu", 0);

    glm::mat4 model = glm::mat4(1.0f);
    s.setUniformMatrix4fv("modelMat", model);

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1280.0f / 720.0f, 0.1f, 500.0f);
    s.setUniformMatrix4fv("projectionMat", projection);

    TextureAtlas::getInstance().load("res/textures/blocks/atlas.png");

    Camera cam({0, 0, -1}, {0, 0, 0});

    World w;
    glm::dvec3 pos(0);

    for (int x = 0; x < 5; x++)
        for (int z = 0; z < 5; z++)
            w.loadColumn({x, z});

    bool appShouldTerminate = m_windows.empty();
    std::thread updateThread(World::updateLoop, std::ref(w), std::ref(pos), std::ref(appShouldTerminate));

    while (!appShouldTerminate)
    {
        for (int i = m_windows.size() - 1; i >= 0; i--)
        {
            auto &currentPair = m_windows[i];
            auto &currentWindow = currentPair.second;

            //LOG_INFO("{}", (void*)glfwGetCurrentContext());

            currentWindow->bindContext();

            //LOG_INFO("{}", (void*)glfwGetCurrentContext());

            s.enable();

            glm::dvec2 rot = currentWindow->getCursorTravel() * 0.1;
            cam.rotate(rot.y, rot.x);
            s.setUniformMatrix4fv("viewMat", cam.getView());

            currentWindow->update();
            currentWindow->clear();

            // @DEBUG
            poolKeys(currentWindow->getGLFWwindow(), cam);

            auto test = w.getRenderData();
            for (const auto &d : test)
            {
                glBindVertexArray(d.vao);
                glDrawElements(GL_TRIANGLES, d.nbVertices, GL_UNSIGNED_INT, (void *)0);
                glBindVertexArray(0);
            }

            currentWindow->draw();
            currentWindow->unbindContext();

            if (currentWindow->shouldClose())
            {
                if (currentPair.first == m_mainWindowUID)
                    appShouldTerminate = true;

                m_windows.erase(m_windows.begin() + i);
            }
        }
    }

    updateThread.join();
}

WindowUID Application::createWindow(const std::string &title, unsigned width, unsigned height)
{
    try
    {
        auto newWindowPtr(std::make_unique<Window>(title, width, height));

        WindowUID uid = m_windowUIDCounter++;
        auto newPair(std::make_pair(uid, std::move(newWindowPtr)));
        m_windows.push_back(std::move(newPair));

        return uid;
    }
    catch (RuntimeException &e)
    {
        LOG_ERROR("Cannot create new Window \"{}\": {}", title, e.what());
        return 0;
    }
}

void Application::destroyWindow(WindowUID uid)
{
    try
    {
        if (uid == WINDOW_NULL)
            throw RuntimeException("Window is null");

        if (uid == m_mainWindowUID)
        {
            LOG_TRACE("Destroying main window -> destroying all windows");
            m_windows.clear();
            m_mainWindowUID = WINDOW_NULL;
            m_windowUIDCounter = 1;
        }
        else
        {
            m_windows.erase(getWindowFromUID(uid));
        }
    }
    catch (RuntimeException &e)
    {
        LOG_ERROR("Cannot destroy window with UID \"{}\": {}", uid, e.what());
    }
}

Window *Application::getInternalWindow(WindowUID uid) noexcept
{
    try
    {
        return getWindowFromUID(uid)->second.get();
    }
    catch (RuntimeException &e)
    {
        LOG_ERROR("Unable to find window with UID \"{}\": ", uid, e.what());
        return nullptr;
    }
}

void Application::updateWindowSize(GLFWwindow *window, int width, int height)
{
    try
    {
        auto it = getWindowFromGLFWwindow(window);
        it->second->updateSize(width, height);
    }
    catch (RuntimeException &e)
    {
        LOG_ERROR("Could not update window size : {}", e.what());
    }
}

void Application::updateWindowCursorPosition(GLFWwindow *window, double xpos, double ypos)
{
    try
    {
        auto it = getWindowFromGLFWwindow(window);
        it->second->updateCursorPosition(xpos, ypos);
    }
    catch (RuntimeException &e)
    {
        LOG_ERROR("Could not update cursor position : {}", e.what());
    }
}

//private:

std::vector<std::pair<WindowUID, std::unique_ptr<Window>>>::iterator Application::getWindowFromUID(WindowUID uid)
{
    auto it = std::find_if(
        m_windows.begin(),
        m_windows.end(),
        [&](const std::pair<WindowUID, std::unique_ptr<Window>> &current) {
            return current.first == uid;
        });

    if (it == m_windows.end())
        throw RuntimeException("Window does not exist in window list");

    return it;
}

std::vector<std::pair<WindowUID, std::unique_ptr<Window>>>::iterator Application::getWindowFromGLFWwindow(GLFWwindow *window)
{
    auto it = std::find_if(
        m_windows.begin(),
        m_windows.end(),
        [&](const std::pair<WindowUID, std::unique_ptr<Window>> &current) {
            return (current.second->getGLFWwindow()) == window;
        });

    if (it == m_windows.end())
        throw RuntimeException("Unable to find window in list from GLFW window");

    return it;
}

//------------------------------------//
// Callbacks

void glfw_error_callback(int error_code, const char *description)
{
    LOG_CRITICAL("GLFW error occured: {}", description);
    throw RuntimeException();
}

} // namespace tk
