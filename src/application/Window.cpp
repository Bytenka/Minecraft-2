#include "pch.h"

#include "Window.h"
#include "Application.h"

#include "../utils/Exceptions.h"
#include "../graphics/Image.h"

namespace tk
{
Window::Window(const std::string &title, unsigned width, unsigned height)
    : m_title(title), m_width(width), m_height(height), m_cursorPos({width / 2, height / 2})
{
    // Current context infos for if somethings go wrong
    GLFWwindow *newWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (newWindow == NULL)
        throw RuntimeException("Unable to create a GLFW window");

    glfwMakeContextCurrent(newWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw RuntimeException("Failed to initialize GLAD for window");

    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(newWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(newWindow, cursor_pos_callback);
    // @TODO glfwSetWindowFocusCallback()

    // Setting up OpenGL. Functions are safe to call in that configuration
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_glfwWindow = newWindow;
    setClearColor(0, 127, 127);

    useMouseAsInput(true);

	glfwMakeContextCurrent(NULL);

    LOG_INFO("Created new window \"{}\" ({}, {})", m_title, m_width, m_height);
}

Window::~Window()
{
    glfwDestroyWindow(m_glfwWindow);
    LOG_INFO("Destroyed window \"{}\"", m_title);
}

// public:

void Window::bindContext() const noexcept
{
	glfwMakeContextCurrent(m_glfwWindow);
}

void Window::unbindContext() const noexcept
{
	glfwMakeContextCurrent(NULL);
}
void Window::draw() const noexcept
{
    glfwSwapBuffers(m_glfwWindow);
}

void Window::update() noexcept
{
    m_cursorTravel = glm::dvec2(0);
    glfwPollEvents();
}

void Window::setClearColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) const noexcept
{
    glClearColor(
        red / 255.0f,
        green / 255.0f,
        blue / 255.0f,
        alpha / 255.0f);
}

void Window::clear() const noexcept
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::setIcon(const std::string &imgPath)
{
    try
    {
        Image img(imgPath);

        GLFWimage glfwImage;
        glfwImage.width = img.getWidth();
        glfwImage.height = img.getHeight();
        glfwImage.pixels = img.getData();

        glfwSetWindowIcon(m_glfwWindow, 1, &glfwImage);
    }
    catch (RuntimeException &e)
    {
        LOG_ERROR("Unable to set icon for window \"{}\": {}", m_title, e.what());
    }
}

void Window::updateSize(int width, int height) noexcept
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}

void Window::updateCursorPosition(double xpos, double ypos) noexcept
{
    glm::dvec2 newPos = {xpos, ypos};
    m_cursorTravel = newPos - m_cursorPos;

    if (m_mouseIsInput)
    {
        newPos = {m_width / 2.0, m_height / 2.0};
        glfwSetCursorPos(m_glfwWindow, newPos.x, newPos.y);
    }

    m_cursorPos = newPos;
}

void Window::useMouseAsInput(bool value) noexcept
{
    if (value)
    {
        //glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        updateCursorPosition(m_width / 2.0, m_height / 2.0);
    }
    else
    {
        glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    m_mouseIsInput = value;
}

//--------------------------------------------------//

// Callbacks
void framebuffer_size_callback(GLFWwindow *window, int width, int height) noexcept
{
    if (width != 0 && height != 0)
        Application::getInstance().updateWindowSize(window, width, height);
}

void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) noexcept
{
    Application::getInstance().updateWindowCursorPosition(window, xpos, ypos);
}
} // namespace tk
