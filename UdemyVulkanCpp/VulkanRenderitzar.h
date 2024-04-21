//
// Created by Marcel Vilalta i Soler on 20/4/24.
//

#ifndef UDEMYVULKANCPP_VULKANRENDERITZAR_H
#define UDEMYVULKANCPP_VULKANRENDERITZAR_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <iostream>

class VulkanRenderitzar {

public:
    VulkanRenderitzar();

    int init(GLFWwindow* newWindow);

private:
    GLFWwindow* window;

    VkInstance instance; // Vulkan Starts with Vk. Vulkan Type. It is just a typedef

    // Vulkan Functions
    void crearInstancia();


    // Support
    bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);

};


#endif //UDEMYVULKANCPP_VULKANRENDERITZAR_H
