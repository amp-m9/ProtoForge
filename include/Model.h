#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Mesh.h"
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>

class aiScene;
class Camera;
class Shader;

class Model
{
  public:
    Model(const char* path, bool bFlipTexturesVertically = false) { LoadModel(path, bFlipTexturesVertically); }

    void Draw(Camera& Camera, Shader& shader);

  private:
    std::vector<Mesh> mMeshes;
    std::vector<TextureInfo> mTextures;

    std::string mDirectory;
    glm::mat4 mModelTransform = glm::mat4(1.0f);
    void LoadModel(const std::string path, bool bFlipTexturesVertically);
    void ProcessNode(aiNode* node, const aiScene* scene, std::vector<MeshCreationInfo>& InMeshCreationInfoArray,
                     std::vector<TextureInfo>& InTextureInfoArray, const bool bFlipTexturesVertically);
    MeshCreationInfo GatherMeshInfo(std::vector<TextureInfo>& InTextureInfoArray, aiMesh* InMesh, const aiScene* scene,
                                    const bool bFlipTexturesVertically);
    std::vector<unsigned int> GatherMaterialTextures(std::vector<TextureInfo>& InTextureInfoArray, aiMaterial* InMaterial, aiTextureType textureType,
                                                     std::string textureTypeName, const aiScene* InScene, const bool bFlipTexturesVertically);
    void LoadMeshTextures(std::vector<TextureInfo>& TextureInfoArray);
    void AddMeshesToModel(const std::vector<MeshCreationInfo>& MeshInfoArray, std::vector<TextureInfo>& TextureInfoArray);
};