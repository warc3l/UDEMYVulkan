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

    float angle = 0.0f;
    float deltaTime = 0.0f;
    float lastTime = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        angle += 10.0f * deltaTime;
        if (angle > 360) {
            angle = angle - 360;
        }

        vulkanRenderitzar.updateModel(glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f)));
        vulkanRenderitzar.draw();
        glfwPollEvents();
    }

    // Destroy and Stop
    vulkanRenderitzar.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
