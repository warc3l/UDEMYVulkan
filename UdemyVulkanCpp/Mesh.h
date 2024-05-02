//
// Created by Marcel Vilalta i Soler on 28/4/24.
//
#pragma once
#ifndef UDEMYVULKANCPP_MESH_H
#define UDEMYVULKANCPP_MESH_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "Util.h"



struct UboModel {
    glm::mat4 model;
};


class Mesh {

public:
    Mesh();
    Mesh(VkPhysicalDevice physicalDevice, VkDevice newDevice, VkQueue transformQueue,
         VkCommandPool transferCommandPool, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices);

    void setModel(glm::mat4 model);
    UboModel& getModel();

    VkBuffer getVertexBuffer() {
        return vertexBuffer;
    }

    int getVertexCount() {
        return vertexCount;
    }

    int getIndicesCount() {
        return indexCount;
    }

    VkBuffer getIndexBuffer() {
        return indexBuffer;
    }

    void destroyBuffers();

    ~Mesh();

private:
    UboModel uboModel;

    int indexCount;
    int vertexCount;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkPhysicalDevice physicalDevice;
    VkDevice device;

    void createVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices);
    void createIndicesBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t>* indices);

};


#endif //UDEMYVULKANCPP_MESH_H
