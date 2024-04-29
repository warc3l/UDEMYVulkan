//
// Created by Marcel Vilalta i Soler on 20/4/24.
//

#ifndef UDEMYVULKANCPP_VULKANRENDERITZAR_H
#define UDEMYVULKANCPP_VULKANRENDERITZAR_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_beta.h>
#include <glm/gtc/matrix_transform.hpp>

#include <set>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <array>
#include <iostream>
#include "Util.h"
#include "Mesh.h"

class VulkanRenderitzar {

public:
    VulkanRenderitzar();

    int init(GLFWwindow* newWindow);

    void draw();

    void cleanup();
    void updateModel(glm::mat4 newModel);

    ~VulkanRenderitzar();

private:
    int currentFrame = 0;

    GLFWwindow* window;

    // Scene objects
    std::vector<Mesh> meshList;

    // Scene Settings
    struct MVP {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
    } mvp;


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
    std::vector<VkFramebuffer> swapChainFrameBuffers;
    std::vector<VkCommandBuffer> commandBuffers;

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkBuffer> uniformBuffer;
    std::vector<VkDeviceMemory> uniformBufferMemory;


    // Pipeline, can do
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;

    VkCommandPool graphicsCommandPool; // A pool, similar in Metal

    // We need two signals,
    std::vector<VkSemaphore> imageAvailable;
    std::vector<VkSemaphore> renderFinished;
    std::vector<VkFence> drawFences;

    void crearInstancia();
    void getPhysicalDevice();
    void createLogicalDevice();
    void createSurface();
    void createSwapChain();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSynchronization();
    void createDescriptorSetLayout();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void updateUniformBuffer(uint32_t imageIndex);

    void recordCommands();


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
    VkShaderModule createShaderModule(const std::vector<char>& code);
};


#endif //UDEMYVULKANCPP_VULKANRENDERITZAR_H
