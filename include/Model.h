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
    Model() {}

    void Draw(Camera& Camera, Shader& shader);
    void LoadMeshTextures(std::vector<TextureInfo>& TextureInfoArray);
    void AddMeshesToModel(const std::vector<MeshCreationInfo>& MeshInfoArray, std::vector<TextureInfo>& TextureInfoArray);
    void SetPosition(const glm::vec3 Position);
    void SetScale(const glm::vec3 Position);
    
    struct Utils
    {
        Utils() = delete;
        static Model Cube();
        static unsigned char* CreateSolidTexture(unsigned char r,unsigned char g,unsigned char b,unsigned char a)
        {
            constexpr int size = 100 * 100 * 4;

            auto* data = new unsigned char[size];

            for (int i = 0; i < 100 * 100; ++i)
            {
                data[i * 4 + 0] = r;
                data[i * 4 + 1] = g;
                data[i * 4 + 2] = b;
                data[i * 4 + 3] = a;
            }

            return data;
        }
    };

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
};