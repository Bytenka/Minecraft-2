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
    glfwTerminate();
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
        if (uid == 0)
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
            for (int i = m_windows.size() - 1; i >= 0; i--)
            {
                if (m_windows[i].first == uid)
                {
                    m_windows.erase(m_windows.begin() + i);
                    return;
                }
            }
            throw std::invalid_argument("Window is not in windows list");
        }
    }
    catch (std::invalid_argument &e)
    {
        LOG_ERROR("Window with UID \"{}\" does not exist", uid);
    }
    catch (RuntimeException &e)
    {
        LOG_ERROR("Cannot destroy window: {}", e.what());
    }
}

} // namespace tk
