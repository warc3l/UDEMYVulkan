//
// Created by Marcel Vilalta i Soler on 21/4/24.
//

#ifndef UDEMYVULKANCPP_UTIL_H
#define UDEMYVULKANCPP_UTIL_H


const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
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



#endif //UDEMYVULKANCPP_UTIL_H
