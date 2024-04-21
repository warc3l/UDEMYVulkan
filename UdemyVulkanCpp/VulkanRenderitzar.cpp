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
    } catch(const std::runtime_error & e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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

    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()); // Depending on the implementation, better to explicitly set this
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    // TODO: Set up validationm layer that instance will use
    createInfo.enabledLayerCount = 0; // We will set this on videos later, as it is more complicated for now.
    createInfo.ppEnabledLayerNames = nullptr;

    // FINALLY - Create an instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if ( result != VK_SUCCESS ) {
        throw std::runtime_error("Failed to create a Vulkan Instance with an error " + std::to_string(result));
    }


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



