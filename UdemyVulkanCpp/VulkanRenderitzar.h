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
#include "Util.h"

class VulkanRenderitzar {

public:
    VulkanRenderitzar();

    int init(GLFWwindow* newWindow);

private:
    GLFWwindow* window;

    VkInstance instance; // Vulkan Starts with Vk. Vulkan Type. It is just a typedef

    struct {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;

    } mainDevice;
    VkQueue graphicsQueue;

    void getPhysicalDevice();
    void createLogicalDevice();


    // Vulkan Functions
    void crearInstancia();
    void cleanup();


    // Support
    bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
    bool checkDeviceSuitable(VkPhysicalDevice device);

    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice devicee);

};


#endif //UDEMYVULKANCPP_VULKANRENDERITZAR_H
