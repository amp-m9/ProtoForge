#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <cstdint>

#include "Graphics/Object3D.h"
#include "Graphics/Mesh.h"
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>

class aiScene;
class PFCamera;
class PFShader;



class PFModel : public PFObject3D
{
public:
    PFModel(const char* path, bool bFlipTexturesVertically = false) 
    { 
        mDisplayName = path;
        LoadModel(path, bFlipTexturesVertically); 
    }

    PFModel() {}

    void Draw(PFCamera& Camera, PFShader& shader);
    void LoadMeshTextures(std::vector<PFTextureCreationInfo>& TextureInfoArray);
    void AddMeshesToModel(const std::vector<PFMeshCreationInfo>& MeshInfoArray, std::vector<PFTextureCreationInfo>& TextureInfoArray);

    const char* GetDisplayName(){ return mDisplayName.c_str(); }
    void SetDisplayName(std::string InDisplayName){  mDisplayName = InDisplayName; }

    struct Utils
    {
        Utils() = delete;
        static PFModel Cube();
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
    std::vector<PFMesh> mMeshes;
    std::vector<PFTextureCreationInfo> mTextures;
    std::string mDirectory;

    void LoadModel(const std::string path, bool bFlipTexturesVertically);
    void ProcessNode(aiNode* node, const aiScene* scene, std::vector<PFMeshCreationInfo>& InMeshCreationInfoArray,
                     std::vector<PFTextureCreationInfo>& InTextureInfoArray, const bool bFlipTexturesVertically);
    PFMeshCreationInfo GatherMeshInfo(std::vector<PFTextureCreationInfo>& InTextureInfoArray, aiMesh* InMesh, const aiScene* scene,
                                    const bool bFlipTexturesVertically);
    std::vector<unsigned int> GatherMaterialTextures(std::vector<PFTextureCreationInfo>& InTextureInfoArray, aiMaterial* InMaterial, aiTextureType textureType,
                                                     std::string textureTypeName, const aiScene* InScene, const bool bFlipTexturesVertically);
};