//
// Created by Marcel Vilalta i Soler on 21/4/24.
//

#ifndef UDEMYVULKANCPP_UTIL_H
#define UDEMYVULKANCPP_UTIL_H

#include <vulkan/vulkan_beta.h>


// Indices (locations) of Queues families if they exist at all

struct QueueFamilyIndices {
    uint32_t graphicsFamily = -1; // Location of the Graphic family
    uint32_t presentationFamily = -1;

    bool isValid() {
        return graphicsFamily >= 0 && presentationFamily >= 0;
    }
};

struct SwapChainDetails {

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> formats;        // eg RGBA and sizes
    std::vector<VkPresentModeKHR> presentationsModes; // How image should be presented in the screen


};


#endif //UDEMYVULKANCPP_UTIL_H
