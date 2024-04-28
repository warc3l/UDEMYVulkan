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

class Mesh {

public:
    Mesh();
    Mesh(VkPhysicalDevice physicalDevice, VkDevice newDevice, std::vector<Vertex>* vertices);

    VkBuffer getVertexBuffer() {
        return vertexBuffer;
    }

    int getVertexCount() {
        return vertexCount;
    }

    void destroyVertexBuffer();

    ~Mesh();

private:
    int vertexCount;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkPhysicalDevice physicalDevice;
    VkDevice device;

    void createVertexBuffer(std::vector<Vertex>* vertices);
};


#endif //UDEMYVULKANCPP_MESH_H
