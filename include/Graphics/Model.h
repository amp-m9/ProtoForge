#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <array>
#include <cstdint>

#include "Graphics/Mesh.h"
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>

class aiScene;
class PFCamera;
class PFShader;

enum class RotationType : std::uint8_t 
{
    Quaternion,
    XYZ,
    XZY,
    YXZ,
    YZX,
    ZXY,
    ZYX,
    AxisAngle,
};

inline std::array<std::uint8_t, 3> GetRotationOrderFromEnum(const RotationType Type)
{
    switch (Type)
    {
    case RotationType::XYZ:
        return{2, 1, 0};
        
    case RotationType::XZY:
        return{1, 2, 0};
    
    case RotationType::YXZ:
        return{2, 0, 1};
    
    case RotationType::YZX:
        return{0, 2, 1};
    
    case RotationType::ZXY:
        return{1, 0, 2};

    case RotationType::ZYX:
        return{0, 1, 2};

    case RotationType::Quaternion:
    case RotationType::AxisAngle:
    default:
        return {0,0,0};
    }
}

class PFModel
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
    void SetPosition(const glm::vec3 Position);
    void SetScale(const glm::vec3 Scale);
    void SetRotation(const float X, const float Y, const float Z);

    void UpdateProjectionMatrix();
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
    std::string mDisplayName;
    std::string mDirectory;
    glm::mat4 mModelTransform = glm::mat4(1.0f);
    glm::vec3 mPosition = glm::vec3(1.0f);
    glm::vec3 mScale = glm::vec3(1.0f);
    glm::vec3 mRotationEuler = glm::vec3(0.0);
    glm::mat4 mRotationQuat = glm::mat4(1.0f);
    RotationType mRotationType = RotationType::XYZ;

    void LoadModel(const std::string path, bool bFlipTexturesVertically);
    void ProcessNode(aiNode* node, const aiScene* scene, std::vector<PFMeshCreationInfo>& InMeshCreationInfoArray,
                     std::vector<PFTextureCreationInfo>& InTextureInfoArray, const bool bFlipTexturesVertically);
    PFMeshCreationInfo GatherMeshInfo(std::vector<PFTextureCreationInfo>& InTextureInfoArray, aiMesh* InMesh, const aiScene* scene,
                                    const bool bFlipTexturesVertically);
    std::vector<unsigned int> GatherMaterialTextures(std::vector<PFTextureCreationInfo>& InTextureInfoArray, aiMaterial* InMaterial, aiTextureType textureType,
                                                     std::string textureTypeName, const aiScene* InScene, const bool bFlipTexturesVertically);
};