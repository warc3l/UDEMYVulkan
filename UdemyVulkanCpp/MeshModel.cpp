//
// Created by Marcel Vilalta i Soler on 4/5/24.
//

#include "MeshModel.h"

MeshModel::MeshModel() {

}

MeshModel::MeshModel(std::vector<Mesh> newMeshList) {
    meshList = newMeshList;
    model = glm::mat4(1.0f);
}

std::vector<std::string> MeshModel::LoadMaterials(const aiScene *scene) {
    std::vector<std::string> textureList(scene->mNumMaterials);
    for (size_t i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial * material = scene->mMaterials[i];
        textureList[i] = "";
        if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
                int idx = std::string(path.data).rfind("\\");
                std::string fileName = std::string(path.data).substr(idx + 1);
                textureList[i] = fileName;
            }
        }
    }

    return textureList;
}

Mesh MeshModel::LoadMesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue,
                         VkCommandPool transferCommandPool, aiMesh *mesh, const aiScene *scene,
                         std::vector<int> matToTex) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.resize(mesh->mNumVertices);

    for (size_t i = 0; i < mesh->mNumVertices; i++) {
        vertices[i].pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

        if (mesh->mTextureCoords[0]) {
            vertices[i].tex = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }
        else {
            vertices[i].tex = { 0.0f, 0.0f };
        }
        vertices[i].col = { 1.0f, 1.0f, 1.0f };
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    Mesh newMesh = Mesh(newPhysicalDevice, newDevice, transferQueue, transferCommandPool, &vertices, &indices, matToTex[mesh->mMaterialIndex]);

    return newMesh;
}

std::vector<Mesh> MeshModel::LoadNode(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue,
                                      VkCommandPool transferCommandPool, aiNode *node, const aiScene *scene,
                                      std::vector<int> matToTex) {
    std::vector<Mesh> meshList;
    for (size_t i = 0; i < node->mNumMeshes; i++) {
        meshList.push_back(
                LoadMesh(newPhysicalDevice, newDevice, transferQueue, transferCommandPool, scene->mMeshes[node->mMeshes[i]], scene, matToTex)
        );
    }

    for (size_t i = 0; i < node->mNumChildren; i++) {
        std::vector<Mesh> newList = LoadNode(newPhysicalDevice, newDevice, transferQueue, transferCommandPool, node->mChildren[i], scene, matToTex);
        meshList.insert(meshList.end(), newList.begin(), newList.end());
    }

    return meshList;
}


