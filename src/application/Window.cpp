#include "Window.h"

#include "../utils/Exceptions.h"

namespace tk
{
Window::Window(const std::string &title, unsigned width, unsigned height)
    : m_title(title), m_width(width), m_height(height)
{
    // Current context infos for if somethings go wrong
    GLFWwindow *newWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (newWindow == NULL)
        throw RuntimeException("Unable to create a GLFW window");

    glfwMakeContextCurrent(newWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw RuntimeException("Failed to initialize GLAD for window");

    glfwSwapInterval(0);
    //glfwSetFramebufferSizeCallback()
    //glfwSetCursorPosCallback()

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

    LOG_INFO("Created new window \"{}\" ({}, {})", m_title, m_width, m_height);
}

Window::~Window()
{
    glfwDestroyWindow(m_glfwWindow);
    LOG_INFO("Destroyed window \"{}\"", m_title);
}

void Window::draw() const noexcept
{
    glfwMakeContextCurrent(m_glfwWindow);
    glfwSwapBuffers(m_glfwWindow);
}

void Window::update() const noexcept
{
    glfwMakeContextCurrent(m_glfwWindow);
    glfwPollEvents();
}

void Window::setClearColor(u_int8_t red, u_int8_t green, u_int8_t blue, u_int8_t alpha) const noexcept
{
    glfwMakeContextCurrent(m_glfwWindow);
    glClearColor(
        red / 255.0f,
        green / 255.0f,
        blue / 255.0f,
        alpha / 255.0f);
}

void Window::clear() const noexcept
{
    glfwMakeContextCurrent(m_glfwWindow);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
} // namespace tk
