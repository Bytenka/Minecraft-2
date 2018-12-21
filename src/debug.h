#pragma once

#include <GLFW/glfw3.h>
#include "graphics/Camera.h"

namespace tk
{
bool poolKeys(GLFWwindow *w)
{
    if (glfwGetKey(w, GLFW_KEY_R) == GLFW_PRESS)
        return true;

    return false;
}
} // namespace tk