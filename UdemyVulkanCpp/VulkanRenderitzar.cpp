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
        getPhysicalDevice();
    } catch(const std::runtime_error & e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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

void VulkanRenderitzar::createLogicalDevice() {

    QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;
    float priority = 1.0f;
    queueCreateInfo.pQueuePriorities = &priority;


    // Informamtion to create the logical device.s
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;  // Number of Queues Create Infos
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = 0; // We do not have for now extensions for the DEVICE (but yes for the instance)
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledLayerCount = 0;

    // Physical Devices features the logical device will be using;
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures; // Physical Device features logical device will use

    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Could not create a Logical Device due to " + std::to_string(result));
    }

    // Get a reference of the Queue Family graphics to able to clean-up later
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);


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
    appInfo.apiVersion = VK_API_VERSION_1_3; // I think that I have 1.3.280.1

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
    const char** glfwExtensions;    // Extensions
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Add GLFW extension to list of extensions
    for (int i = 0; i < glfwExtensionCount; i++) {
        instanceExtensions.push_back(glfwExtensions[i]);
    }

    // In case of MacOS...
    instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    if (!checkInstanceExtensionSupport(&instanceExtensions)) {
        // At least one of the extensions, is not supported.
        throw std::runtime_error("VkInstance does not support required extensions");
    }

    if (!checkValidationLayerSupport()) {
        throw std::runtime_error("Validation Layers not available");
    }

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

void VulkanRenderitzar::cleanup() {
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);
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

    return indices.isValid(); // we will return on this later, for when doing queue families.
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

    int i = 0;// it would be better to use for loop instead of a foreach one
    for (const auto& queueFamily: queueFamilyList) {
        // We are going to check if the VK GRAPHICS QUEUE is there
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            // what is the index of the queue family?
            indices.graphicsFamily = i;
        }

        if (indices.isValid()) {
            break;
        }

        i++;
    }

    return indices;
}




