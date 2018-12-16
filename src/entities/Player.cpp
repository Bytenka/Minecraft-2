#include "pch.h"
#include "Player.h"

#include <GLFW/glfw3.h>

namespace tk
{
Player::Player(const glm::dvec3 &initialPosition,
               const glm::dvec3 &initialDirection)
    : m_camera(initialPosition, initialDirection)
{
}

Player::~Player()
{
}

// public:

void Player::update(const Window &window) noexcept
{
    poolKeys(window);
    poolMouse(window);
}

// private:

void Player::poolKeys(const Window &window) noexcept
{
    GLFWwindow *w = window.getGLFWwindow();

    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS)
        m_camera.move(m_camera.getForward());

    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.move(-m_camera.getForward());

    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.move(-m_camera.getRight());

    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.move(m_camera.getRight());

    if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_camera.move({0, 1, 0});

    if (glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        m_camera.move({0, -1, 0});

    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.getGLFWwindow(), true);
}

void Player::poolMouse(const Window &window) noexcept
{
    glm::dvec2 travel = window.getCursorTravel() * m_mouseSensitivity;
    m_camera.rotate({travel.y, travel.x});

	GLFWwindow* w = window.getGLFWwindow();
}

} // namespace tk
