#include "GLFW/glfw3.h"
#include "GLM/glm.hpp"
#include <vulkan/vulkan.h>

#include <iostream>

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Test Punteru Window", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << "Extension count: " << extensionCount << std::endl;

    glm::mat4 testMatrix(1.0f);
    glm::mat4 testVector(1.0f);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
