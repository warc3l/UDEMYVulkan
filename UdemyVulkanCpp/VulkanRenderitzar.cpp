//
// Created by Marcel Vilalta i Soler on 20/4/24.
//

#include "VulkanRenderitzar.h"

VulkanRenderitzar::VulkanRenderitzar() {

}

int VulkanRenderitzar::init(GLFWwindow* newWindow) {
    window = newWindow;

    try {
        crearInstancia();
        createSurface();
        getPhysicalDevice(); // Both checks depending on the surface.
        createLogicalDevice(); // Make sure that the device supports the surface that we select.
        createSwapChain();
    } catch(const std::runtime_error & e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void VulkanRenderitzar::cleanup() {
    vkDestroySwapchainKHR(mainDevice.logicalDevice, swapChain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);
}

VulkanRenderitzar::~VulkanRenderitzar()
{

};

void VulkanRenderitzar::crearInstancia() {

    // Information of the application itself.
    // Most data here does not affect the program and it is for developer convenience,
    // just a debug-like tooling
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App"; // Directly the vulkan name of the application
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // Custom version of the application
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); // Just for developer, is not functional
    appInfo.apiVersion = VK_API_VERSION_1_0; // I think that I have 1.3.280.1

    // Everything we create in Vulkan, will be through 'create info' functions
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // structure Type.
    // We can chec "which type" as we are doing "reflection" with other languages using this sType

    // The idea is to get "extension" here. Extended information, extended data.
    // createInfo.sNext = nullptr; // We are not going to use this

    // createInfo.flags, there are a lot of flags, as a bitmask to enable things
    createInfo.pApplicationInfo = &appInfo; // pXXX pointer of XXX
    // let's start with the extension because is simply to explain


    // Create list to hold instance extensions
    std::vector<const char*> instanceExtensions = std::vector<const char*>();

    uint32_t glfwExtensionCount = 0; // we will increment it, GLFW may require multiple
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Add GLFW extension to list of extensions
    for (int i = 0; i < glfwExtensionCount; i++) {
        instanceExtensions.push_back(glfwExtensions[i]);
    }
    // instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    // In case of MacOS...
    instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    if (!checkInstanceExtensionSupport(&instanceExtensions)) {
        // At least one of the extensions, is not supported.
        throw std::runtime_error("VkInstance does not support required extensions");
    }

//    if (!checkValidationLayerSupport()) {
//        throw std::runtime_error("Validation Layers not available");
//    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()); // Depending on the implementation, better to explicitly set this
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    // Validation Layer - per default, it will handle to the standard output.
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()); // We will set this on videos later, as it is more complicated for now.
    createInfo.ppEnabledLayerNames = validationLayers.data();

    // FINALLY - Create an instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if ( result != VK_SUCCESS ) {
        throw std::runtime_error("Failed to create a Vulkan Instance with an error " + std::to_string(result));
    }


    // Before to create the Device, wee need to DESTROY and free the memory.
    // We need two devices, Physical and Logical. Logical is the interface TO the physical.



}

void VulkanRenderitzar::createLogicalDevice() {
    QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    std::set<int> queueFamilyIndices = {indices.graphicsFamily, indices.presentationFamily };

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    float priority = 1.0f;

//queueCreateInfos

    for (auto queueFamilyIndex : queueFamilyIndices) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &priority;

        queueCreateInfos.push_back(queueCreateInfo);
    }


    // Informamtion to create the logical device.s
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());  // Number of Queues Create Infos
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();


    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()); // We do not have for now extensions for the DEVICE (but yes for the instance)
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data(); // The Device Extensions are predefined before, then, we validate that we actually have them. If not, throw errors

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    // Creates Devices and Queues
    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Could not create a Logical Device due to " + std::to_string(result));
    }

    // Get a reference of the Queue Family graphics to able to clean-up later
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);

    // We just a handle of the Presentation. This does not create the queue, just gets the references.
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamily, 0, &presentationQueue);
}


void VulkanRenderitzar::createSurface() {
    // We can take benefit of GLFW built-in function to know which operating systme
    // we are working on

    // Crete the Surface
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Cannot create a surface for the window due to " + std::to_string(result));
    }
}

void VulkanRenderitzar::createSwapChain() {
    SwapChainDetails swapChainDetails = getSwapChainDetails(mainDevice.physicalDevice);

    // 1. We need to choose the BEST surface format
    VkSurfaceFormatKHR surfaceFormatKhr = chooseSurfaceFormat(swapChainDetails.formats);

    // 2. Choose the Presentation mode
    VkPresentModeKHR  presentModeKhr = chooseBestPresentationMode(swapChainDetails.presentationsModes);

    // 3. Chose the SWAP IMAGE RESOLUTION
    VkExtent2D extent = chooseSwapExtent(swapChainDetails.surfaceCapabilities);

    // How many images are in the swap chain?
    uint32_t imageCount = swapChainDetails.surfaceCapabilities.minImageCount + 1;
    if (swapChainDetails.surfaceCapabilities.maxImageCount < imageCount > 0 && swapChainDetails.surfaceCapabilities.maxImageCount < imageCount) {
        imageCount = swapChainDetails.surfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfoKhr = {};
    swapchainCreateInfoKhr.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfoKhr.imageFormat = surfaceFormatKhr.format;
    swapchainCreateInfoKhr.surface = surface;
    swapchainCreateInfoKhr.imageColorSpace = surfaceFormatKhr.colorSpace;
    swapchainCreateInfoKhr.presentMode = presentModeKhr;
    swapchainCreateInfoKhr.imageExtent = extent;
    swapchainCreateInfoKhr.minImageCount = imageCount;
    swapchainCreateInfoKhr.imageArrayLayers = 1;
    swapchainCreateInfoKhr.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfoKhr.preTransform = swapChainDetails.surfaceCapabilities.currentTransform;
    swapchainCreateInfoKhr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfoKhr.clipped = VK_TRUE;


    QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    if (indices.graphicsFamily != indices.presentationFamily) {
        uint32_t queueFamilyIndices[] = {
                (uint32_t) indices.graphicsFamily,
                (uint32_t) indices.presentationFamily
        };

        swapchainCreateInfoKhr.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfoKhr.queueFamilyIndexCount = 2;
        swapchainCreateInfoKhr.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        // Only one queue. Graphcis and Presentation family are the same here
        // This is our case here
        swapchainCreateInfoKhr.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfoKhr.queueFamilyIndexCount = 0;
        swapchainCreateInfoKhr.pQueueFamilyIndices = nullptr;
    }

    // Useful when resizing the window
    swapchainCreateInfoKhr.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(mainDevice.logicalDevice, &swapchainCreateInfoKhr, nullptr, &swapChain);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Cannot create SwapChain due to " + std::to_string(result));
    }
}


// Best format is subjetive but based on the Udemy recommendation is VK_FORMAT_R8GB8A8 (omg why that many 8 and B)
VkSurfaceFormatKHR VulkanRenderitzar::chooseSurfaceFormat (const std::vector<VkSurfaceFormatKHR>& formats)
{
    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        return {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto& format: formats) {
        if ((format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM) && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    return formats[0];
}

VkPresentModeKHR VulkanRenderitzar::chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes) {
    for (const auto&presentationMode: presentationModes) {
        if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentationMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRenderitzar::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilitiesKhr)
{
    if ( surfaceCapabilitiesKhr.currentExtent.width != std::numeric_limits<uint32_t>::max() ) {
        return surfaceCapabilitiesKhr.currentExtent;
    }

    // Otherwise, we need to find it ourselves. It means that the value can vary
    int width, height;
    glfwGetFramebufferSize(window, &width, &height); // The actual width/height of the window

    // We need to create a new extend with the window size
    VkExtent2D newExtent = {};
    newExtent.width = static_cast<uint32_t>(width);
    newExtent.height = static_cast<uint32_t>(height);

    // We need to make sure that within the boundaries by clamping the value.
    newExtent.width = std::max(surfaceCapabilitiesKhr.minImageExtent.width, std::min(surfaceCapabilitiesKhr.maxImageExtent.width, newExtent.width));
    newExtent.height = std::max(surfaceCapabilitiesKhr.minImageExtent.height, std::min(surfaceCapabilitiesKhr.maxImageExtent.height, newExtent.height));

    return newExtent;
}




void VulkanRenderitzar::getPhysicalDevice() {
    // Enumerate Physical Devices which we can have access
    uint32_t deviceCount = 0;
    // How many instance we would like to enumerate of?
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    // If no devices available...
    if (deviceCount == 0) {
        throw std::runtime_error("No GPU physical devices encountered");
    }

    // Get me the list
    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

    for (const auto& device: deviceList) {
        if (checkDeviceSuitable(device)) {
            mainDevice.physicalDevice = device;
            break;
        }
    }


    // We need to link the physical device found with the logical device.
}

bool VulkanRenderitzar::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions) {
    // We need to check twice, once for the size, and other for the allocate.

    // First, to get the amount.
    uint32_t howManyVulkansExtensionsSupport = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &howManyVulkansExtensionsSupport, nullptr);

    // Create a list of VkExtensions using the previous count.
    std::vector<VkExtensionProperties> extensions(howManyVulkansExtensionsSupport);
    vkEnumerateInstanceExtensionProperties(nullptr, &howManyVulkansExtensionsSupport, extensions.data());

    bool hasExtension;
    for (const auto& checkExtension: *checkExtensions) {
        hasExtension = false;
        for (const auto& extension: extensions) {
            // We just need to compare the names
            if (strcmp(extension.extensionName, checkExtension)==0) {
                hasExtension = true;
                break;
            }
        }

        if (!hasExtension) {
            return false;
        }
    }

    // All the extensions exist
    return true;
}

bool VulkanRenderitzar::checkDeviceExtensionSupport(VkPhysicalDevice device) {

    // How many
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    if (extensionCount == 0) {
        return false; // no extensions.
    }

    // Let's get
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());


    for (auto const& deviceExtension: deviceExtensions) {
        bool hasExtension = false;
        for (const auto& extension: extensions) {
            if (strcmp(deviceExtension, extension.extensionName) == 0) {
                hasExtension = true;
                break;
            }
        }

        if (!hasExtension) {
            return false;
        }
    }

    return true;
}


bool VulkanRenderitzar::checkValidationLayerSupport() {

    //How many
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    // Let's get
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName: validationLayers) {
        bool layerFound = false;
        for (const auto& layerProperties: availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

bool VulkanRenderitzar::checkDeviceSuitable(VkPhysicalDevice device) {

    // Get the information of the device itself
//    VkPhysicalDeviceProperties deviceProperties;
//    vkGetPhysicalDeviceProperties(device, &deviceProperties);
//
//    // Get the physical device features.
//    VkPhysicalDeviceFeatures deviceFeatures;
//    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);


    // Let's check which queues are supported, and let's add a new Utility class

    QueueFamilyIndices indices = getQueueFamilies(device);

    // Support the extension?
    bool extensionSupported = checkDeviceExtensionSupport(device);

    bool swapChainValid = false;
    if (extensionSupported) {
        SwapChainDetails swapChainDetails = getSwapChainDetails(device);
        swapChainValid = !swapChainDetails.presentationsModes.empty() && !swapChainDetails.formats.empty();
    }

    checkPortabilitySubsetExtension(device);
    return indices.isValid() && extensionSupported && swapChainValid; // we will return on this later, for when doing queue families.
}






QueueFamilyIndices VulkanRenderitzar::getQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    // How many
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    // Give me the list
    std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

    // Go through each equeu family and check it has at least 1 of the required type of the queu.

    uint32_t i = 0;// it would be better to use for loop instead of a foreach one
    for (const auto& queueFamily: queueFamilyList) {
        // We are going to check if the VK GRAPHICS QUEUE is there
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            // what is the index of the queue family?
            indices.graphicsFamily = i;
        }

        // We need to check the presentation family as well
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);
        // Can we check both if graphics and presenations queue
        if (queueFamily.queueCount > 0 && presentationSupport) {
            indices.presentationFamily = i;
        }

        if (indices.isValid()) {
            break;
        }

        i++;
    }

    return indices;
}

SwapChainDetails VulkanRenderitzar::getSwapChainDetails(VkPhysicalDevice device) {
    SwapChainDetails result;

    // Get the surface capabilities for the given surface on the given physical device
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &result.surfaceCapabilities);

    // Let's get the Formats
    uint32_t  formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount > 0) {
        result.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, result.formats.data());
    }

    // Lastly, we need to get the presentation mode, similar way as we did
    uint32_t presentationCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, nullptr);

    if (presentationCount > 0) {
        result.presentationsModes.resize(presentationCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, result.presentationsModes.data());
    }


    return result;
}


//extra
void VulkanRenderitzar::checkPortabilitySubsetExtension(VkPhysicalDevice device) {
    portabilityFeatures = {};
    portabilityFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {};
    physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physicalDeviceFeatures2.pNext = &portabilityFeatures;
    vkGetPhysicalDeviceFeatures2(device, &physicalDeviceFeatures2);
}










