//
// Created by Marcel Vilalta i Soler on 28/4/24.
//

#include "Mesh.h"

Mesh::Mesh() {

}

Mesh::~Mesh() {

}


UboModel& Mesh::getModel() {
    return uboModel;
}

void Mesh::setModel(glm::mat4 model) {
    uboModel.model = model;
}

Mesh::Mesh(VkPhysicalDevice physicalDevice, VkDevice newDevice, VkQueue transformQueue, VkCommandPool transferCommandPool,
           std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, int texId) {
    indexCount = indices->size();
    vertexCount = vertices->size();
    this->physicalDevice = physicalDevice;
    this->device = newDevice;
    createVertexBuffer(transformQueue, transferCommandPool, vertices);
    createIndicesBuffer(transformQueue, transferCommandPool, indices);

    this->texId = texId;
    uboModel.model = glm::mat4(1.0f);
}

void Mesh::destroyBuffers() {
    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr); // Yoou are not using anymore, you are permitted to use it again

    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);

}

void Mesh::createIndicesBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t>* indices)
{
    VkDeviceSize bufferSize = sizeof(uint32_t) * indices->size();

    // Temporary buffer to stagge vertex data before transfering to GPU
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 &stagingBuffer, &stagingBufferMemory);

    // MAP MEMORY TO THE STG BUFFER
    void * data;                                // 1. Create point to a point a normal memory
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data); // 2. Map the vertex buffer memory to that point
    memcpy(data, indices->data(), (size_t) bufferSize);               // 3. Copy memory from vertices vector to the pointer
    vkUnmapMemory(device, stagingBufferMemory);

    // Create buffer for INDEX data on GPU access only area
    createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBuffer, &indexBufferMemory);

    copyBuffer(device, transferQueue, transferCommandPool, stagingBuffer, indexBuffer, bufferSize);

    // Destroy + Release staging buffer resources
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}


void Mesh::createVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices) {

    VkDeviceSize bufferSize = sizeof(Vertex) * vertices->size();

    // Temporary buffer to stagge vertex data before transfering to GPU
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 &stagingBuffer, &stagingBufferMemory);

    // MAP MEMORY TO THE STG BUFFER
    void * data;                                // 1. Create point to a point a normal memory
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data); // 2. Map the vertex buffer memory to that point
    memcpy(data, vertices->data(), (size_t) bufferSize);               // 3. Copy memory from vertices vector to the pointer
    vkUnmapMemory(device, stagingBufferMemory);

    // Create buffer with TRANSFER_DST_BIT to mark as a recipient of transfer data also vertex buffer
    // DEVICE LOCAL BIT meaning memory is on the GPU and only accessible by it, (not CPU)
    createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 &vertexBuffer, &vertexBufferMemory);



    copyBuffer(device, transferQueue, transferCommandPool, stagingBuffer, vertexBuffer, bufferSize);


    // We need to destroy the staging buffer as it is temporary
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

}

