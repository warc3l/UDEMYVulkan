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

    VkDeviceSize bufferSize = sizeof(Vertex) * vertices->size();

    createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 &vertexBuffer, &vertexBufferMemory);

    // MAP MEMORY TO THE VERTEX BUFFER
    void * data;                                // 1. Create point to a point a normal memory
    vkMapMemory(device, vertexBufferMemory, 0, bufferSize, 0, &data); // 2. Map the vertex buffer memory to that point
    memcpy(data, vertices->data(), (size_t) bufferSize);               // 3. Copy memory from vertices vector to the pointer
    vkUnmapMemory(device, vertexBufferMemory);

}

