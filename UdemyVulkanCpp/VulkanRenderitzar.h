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
#include <algorithm>
#include <iostream>
#include "Util.h"

class VulkanRenderitzar {

public:
    VulkanRenderitzar();

    int init(GLFWwindow* newWindow);
    void cleanup();

    ~VulkanRenderitzar();

private:
    GLFWwindow* window;

    VkInstance instance; // Vulkan Starts with Vk. Vulkan Type. It is just a typedef
    struct {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    } mainDevice;
    VkQueue graphicsQueue;
    VkQueue presentationQueue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;

    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<SwapChainImage> swapChainImages;

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };



    void crearInstancia();
    void getPhysicalDevice();
    void createLogicalDevice();
    void createSurface();
    void createSwapChain();

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

    VkSurfaceFormatKHR chooseSurfaceFormat (const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR  chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilitiesKhr);


    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

};


#endif //UDEMYVULKANCPP_VULKANRENDERITZAR_H
