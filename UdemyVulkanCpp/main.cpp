#include "GLFW/glfw3.h"
#include "GLM/glm.hpp"
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include "VulkanRenderitzar.h"

GLFWwindow* window;
VulkanRenderitzar vulkanRenderitzar;

void initWindow(std::string sName = "Window", const int width = 800, const int height = 600)
{
    glfwInit();

    // Avoid OpenGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, sName.c_str(), nullptr, nullptr);
}

int main() {

    initWindow("Test Window", 800, 600);

    if ( vulkanRenderitzar.init(window) == EXIT_FAILURE ) {
        return EXIT_FAILURE;
    }

    while (!glfwWindowShouldClose(window)) {
        vulkanRenderitzar.draw();
        glfwPollEvents();
    }

    // Destroy and Stop
    vulkanRenderitzar.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
