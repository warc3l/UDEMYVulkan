//
// Created by Marcel Vilalta i Soler on 21/4/24.
//
#pragma once
#ifndef UDEMYVULKANCPP_UTIL_H
#define UDEMYVULKANCPP_UTIL_H

#define GLFW_INCLUDE_VULKAN
#include <fstream>
#include <vulkan/vulkan_beta.h>
#include <GLM/glm.hpp>
#include <vector>

const int MAX_FRAME_DRAW = 2;

const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 col;
};


// Indices (locations) of Queues families if they exist at all

struct QueueFamilyIndices {
    int graphicsFamily = -1; // Location of the Graphic family
    int presentationFamily = -1;

    bool isValid() {
        return graphicsFamily >= 0 && presentationFamily >= 0;
    }
};

struct SwapChainDetails {

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> formats;        // eg RGBA and sizes
    std::vector<VkPresentModeKHR> presentationsModes; // How image should be presented in the screen
};

struct SwapChainImage {
    VkImage image;
    VkImageView imageView;
};

static std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    // Check file
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open he file " + filename);
    }

    // Current read position for the resize the filebuffer
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> fileBuffer(fileSize);
    file.seekg(0);

    file.read(fileBuffer.data(), fileSize);

    file.close();

    return fileBuffer;
}

static uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((allowedTypes & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i; // Memory type valid, return index
        }
    }

    return 0;
}

static void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags bufferProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = bufferUsageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, buffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Error to create a buffer " + std::to_string(result));
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, *buffer, &memoryRequirements);

    // ALLOCATE MEMORY TO BUFFER
    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(physicalDevice,
                                                             memoryRequirements.memoryTypeBits,
                                                             bufferProperties);

    // Alocate to VkDeviceMemory
    result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, bufferMemory);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate Vertex buffer memory");
    }

    // Allocate memory to vertex buffer
    vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
}

static void copyBuffer(VkDevice device, VkQueue transferQueue, VkCommandPool transferCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
{
    // Command buffer to hold transfer commands
    VkCommandBuffer transferCommandBuffer;

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = transferCommandPool;
    allocateInfo.commandBufferCount = 1;

    // Allocate command buffer from pool
    vkAllocateCommandBuffers(device, &allocateInfo, &transferCommandBuffer);

    // Information to begin the command buffer record
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;


    // Begin recording transfer commands
    vkBeginCommandBuffer(transferCommandBuffer, &beginInfo);

    // What are the regions that we need to copy from and to
    VkBufferCopy bufferCopyRegion = {};
    bufferCopyRegion.srcOffset = 0;
    bufferCopyRegion.dstOffset = 0; // copy to the start of the second buffer
    bufferCopyRegion.size = bufferSize;

    // Command to copy src buffer to dst buffer
    vkCmdCopyBuffer(transferCommandBuffer, srcBuffer, dstBuffer, 1, &bufferCopyRegion);

    vkEndCommandBuffer(transferCommandBuffer);


    // Queue submission information
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &transferCommandBuffer;


    // Submit transfer command to the transfer queue and wait until it finishes
    vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(transferQueue);


    // Free temporary command buffer back to the pool
    vkFreeCommandBuffers(device, transferCommandPool, 1, &transferCommandBuffer);
}




#endif //UDEMYVULKANCPP_UTIL_H
