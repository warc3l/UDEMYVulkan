//
// Created by Marcel Vilalta i Soler on 4/5/24.
//

#ifndef UDEMYVULKANCPP_MESHMODEL_H
#define UDEMYVULKANCPP_MESHMODEL_H

#include <assimp/scene.h>

#include "Mesh.h"

class MeshModel {

public:
    MeshModel();
    MeshModel(std::vector<Mesh> newMeshModel);

    size_t getMeshCount() {
        return meshList.size();
    }

    Mesh* getMesh(size_t index) {
        if (index < 0 || index >=meshList.size()) {
            throw std::runtime_error("Error, wrong index on mesh");
        }
        return &meshList[index];
    }

    glm::mat4& getModel() {
        return model;
    }

    void setModel(glm::mat4 model) {
        this->model = model;
    }

    void destroyMeshModel() {
        for (auto &mesh : meshList)
        {
            mesh.destroyBuffers();
        }
    }

    static std::vector<Mesh> LoadNode(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue, VkCommandPool transferCommandPool,
            aiNode * node, const aiScene * scene, std::vector<int> matToTex);

    static std::vector<std::string> LoadMaterials(const aiScene * scene);

    static Mesh LoadMesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue, VkCommandPool transferCommandPool,
                         aiMesh * mesh, const aiScene * scene, std::vector<int> matToTex);
private:
    std::vector<Mesh> meshList;
    glm::mat4 model;

};


#endif //UDEMYVULKANCPP_MESHMODEL_H
