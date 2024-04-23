//
// Created by Marcel Vilalta i Soler on 20/4/24.
//

#ifndef UDEMYVULKANCPP_VULKANRENDERITZAR_H
#define UDEMYVULKANCPP_VULKANRENDERITZAR_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_beta.h>

#include <set>
#include <stdexcept>
#include <vector>
#include <iostream>
#include "Util.h"

class VulkanRenderitzar {

public:
    VulkanRenderitzar();

    int init(GLFWwindow* newWindow);
    void cleanup();

private:
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};



    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    GLFWwindow* window;
    VkInstance instance = {}; // Vulkan Starts with Vk. Vulkan Type. It is just a typedef

    struct {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;

    } mainDevice{0};
    VkQueue graphicsQueue = {};
    VkQueue presentationQueue = {};
    VkSurfaceKHR surface = {};


    void crearInstancia();
    void getPhysicalDevice();
    void createLogicalDevice();
    void createSurface();

    // Not from the Udemy course, but from
    // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers
    bool checkValidationLayerSupport();



    // Support
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
    bool checkDeviceSuitable(VkPhysicalDevice device);


    // Extra based on that:
    // https://vulkan.org/user/pages/09.events/vulkanised-2023/vulkanised_2023_vulkan_development_in_apple_environments.pdf
    void checkPortabilitySubsetExtension(VkPhysicalDevice device);
    VkPhysicalDevicePortabilitySubsetFeaturesKHR portabilityFeatures;


    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice devicee);
    SwapChainDetails getSwapChainDetails(VkPhysicalDevice device);


};


#endif //UDEMYVULKANCPP_VULKANRENDERITZAR_H
