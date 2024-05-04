//
// Created by Marcel Vilalta i Soler on 20/4/24.
//

#ifndef UDEMYVULKANCPP_VULKANRENDERITZAR_H
#define UDEMYVULKANCPP_VULKANRENDERITZAR_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_beta.h>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <set>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <array>
#include <iostream>
#include "Mesh.h"
#include "stb_image.h"
#include "MeshModel.h"

class VulkanRenderitzar {

public:
    VulkanRenderitzar();

    int init(GLFWwindow* newWindow);

    void draw();

    void cleanup();
    void updateModel(int modelId, glm::mat4 newModel);
    int createMeshModel(std::string modelFile);

    ~VulkanRenderitzar();

private:
    int currentFrame = 0;

    GLFWwindow* window;

    // Scene objects
    std::vector<MeshModel> meshList;

    // Scene Settings
    struct UboViewProjection {
        glm::mat4 projection;
        glm::mat4 view;
    } uboViewProjection;


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

    std::vector<VkImage> colorBufferImage;
    std::vector<VkDeviceMemory> colorBufferImageMemory;
    std::vector<VkImageView> colorBufferImageView;

    std::vector<VkImage> depthBufferImage;
    std::vector<VkDeviceMemory> depthBufferImageMemory;
    std::vector<VkImageView> depthBufferImageView;

    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSetLayout samplerSetLayout;
    VkDescriptorSetLayout inputSetLayout;

    VkDescriptorPool descriptorPool;
    VkDescriptorPool samplerDescriptorPool;
    VkDescriptorPool inputDescriptorPool;

    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkDescriptorSet> samplerDescriptorSets;
    std::vector<VkDescriptorSet> inputDescriptorSets;

    std::vector<VkBuffer> vpUniformBuffer;
    std::vector<VkDeviceMemory> vpUniformBufferMemory;

    std::vector<VkBuffer> mDUniformBuffer;
    std::vector<VkDeviceMemory> mDUniformBufferMemory;

    VkDeviceSize minUniformBufferOffset;
    size_t modelUniformAlignment;
    UboModel* modelTransferSpace;

    // Pipeline, can do
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;

    VkPipeline secondPipeline;
    VkPipelineLayout secondPipelineLayout;

    VkRenderPass renderPass;
    VkPushConstantRange pushConstantRange;

    VkSampler textureSampler;
    std::vector<VkImage> textureImages;
    std::vector<VkDeviceMemory> textureImageMemory;
    std::vector<VkImageView> textureImageViews;

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
    void createDepthBufferImage();
    void createCommandPool();
    void createCommandBuffers();
    void createSynchronization();
    void createTextureSampler();
    int createTextureDescriptor(VkImageView textureImage);
    void createDescriptorSetLayout();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createColorBufferImage();
    void createInputDescriptorSets();
    void createPushConstantRange();


    int createTextureImage(const std::string& filename);
    int createTexture(std::string filename);
    stbi_uc * loadTexture(const std::string& filename, int* width, int *height, VkDeviceSize * imageSize);
    void updateUniformBuffers(uint32_t imageIndex);

    void recordCommands(uint32_t currentImage);


    // Not from the Udemy course, but from
    // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers
    bool checkValidationLayerSupport();



    // Support
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
    bool checkDeviceSuitable(VkPhysicalDevice device);

    void allocateDynamicBufferTransferSpace();


    // Extra based on that:
    // https://vulkan.org/user/pages/09.events/vulkanised-2023/vulkanised_2023_vulkan_development_in_apple_environments.pdf
    void checkPortabilitySubsetExtension(VkPhysicalDevice device);
    VkPhysicalDevicePortabilitySubsetFeaturesKHR portabilityFeatures;


    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice devicee);
    SwapChainDetails getSwapChainDetails(VkPhysicalDevice device);

    VkSurfaceFormatKHR chooseSurfaceFormat (const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR  chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilitiesKhr);
    VkFormat chooseSupportedFormat(const std::vector<VkFormat> &formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags);

    VkImage createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags, VkDeviceMemory *imageMemory);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    VkShaderModule createShaderModule(const std::vector<char>& code);
};


#endif //UDEMYVULKANCPP_VULKANRENDERITZAR_H
