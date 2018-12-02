#include "Application.h"

#include "../utils/Log.h"
#include "../utils/Exceptions.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace tk
{
Application::Application()
{
    try
    {
        if (glfwInit() == GLFW_FALSE)
            throw RuntimeException("Unable to initialize GLFW");

        //glfwSetErrorCallback();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        Log::init();
        LOG_INFO("Application started");
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
    bool appShouldTerminate = m_windows.empty();
    while (!appShouldTerminate)
    {
        for (int i = m_windows.size() - 1; i >= 0; i--)
        {
            auto &currentPair = m_windows[i];
            auto &currentWindow = currentPair.second;
            currentWindow->update();
            currentWindow->clear();
            currentWindow->draw();

            if (currentWindow->shouldClose())
            {
                if (currentPair.first == m_mainWindowUID)
                    appShouldTerminate = true;

                m_windows.erase(m_windows.begin() + i);
            }
        }
    }
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
    catch (std::invalid_argument &e)
    {
        LOG_ERROR("Window with UID \"{}\" does not exist", uid);
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
        LOG_ERROR("Unable to find window with UID \"{}\"", uid);
        return nullptr;
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

} // namespace tk
