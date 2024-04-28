//
// Created by Marcel Vilalta i Soler on 21/4/24.
//
#pragma once
#ifndef UDEMYVULKANCPP_UTIL_H
#define UDEMYVULKANCPP_UTIL_H

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



#endif //UDEMYVULKANCPP_UTIL_H
