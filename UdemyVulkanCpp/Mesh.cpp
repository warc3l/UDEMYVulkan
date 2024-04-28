//
// Created by Marcel Vilalta i Soler on 28/4/24.
//

#include "Mesh.h"

Mesh::Mesh() {

}

Mesh::~Mesh() {

}

Mesh::Mesh(VkPhysicalDevice physicalDevice, VkDevice newDevice, std::vector<Vertex>* vertices) {
    vertexCount = vertices->size();
    this->physicalDevice = physicalDevice;
    this->device = newDevice;
    createVertexBuffer(vertices);
}


void Mesh::destroyVertexBuffer() {
    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr); // Yoou are not using anymore, you are permitted to use it again
}

void Mesh::createVertexBuffer(std::vector<Vertex>* vertices) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(Vertex) * vertices->size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Error to create a buffer " + std::to_string(result));
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memoryRequirements);

    // ALLOCATE MEMORY TO BUFFER
    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(memoryRequirements.memoryTypeBits,
                                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Alocate to VkDeviceMemory
    result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &vertexBufferMemory);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate Vertex buffer memory");
    }

    // Allocate memory to vertex buffer
    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

    // MAP MEMORY TO THE VERTEX BUFFER
    void * data;                                // 1. Create point to a point a normal memory
    vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data); // 2. Map the vertex buffer memory to that point
    memcpy(data, vertices->data(), (size_t) bufferInfo.size);               // 3. Copy memory from vertices vector to the pointer
    vkUnmapMemory(device, vertexBufferMemory);




}

uint32_t Mesh::findMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties)
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
