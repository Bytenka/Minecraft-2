#pragma once

#include <GLFW/glfw3.h>
#include <graphics/Camera.h>

namespace tk
{
void poolKeys(GLFWwindow *w, Camera &cam)
{
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS)
        cam.move(cam.getForward());

    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS)
        cam.move(-cam.getForward());

    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS)
        cam.move(-cam.getRight());

    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS)
        cam.move(cam.getRight());

    if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS)
        cam.move({0, 1, 0});

    if (glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cam.move({0, -1, 0});

    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(w, true);
}
} // namespace tk