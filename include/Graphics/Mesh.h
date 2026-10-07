#pragma once

#include "Shader.h"
#include <glm/glm.hpp>
#include <assimp/mesh.h>
#include <string>
#include <vector>
#include <map>

#define MAX_BONE_INFLUENCE 4
#define TEXTURE_DIFFUSE "texture_diffuse"
#define TEXTURE_SPECULAR "texture_specular"
#define TEXTURE_NORMAL "texture_normal"
#define TEXTURE_HEIGHT "texture_height"

struct PFVertex
{
    glm::vec3 mPosition{0.0f};
    glm::vec3 mNormal{0.0f};
    glm::vec2 mTexCoords{0.0f};
    glm::vec3 mTangent{0.0f};
    glm::vec3 mBitangent{0.0f};

    PFVertex(
        const glm::vec3& Position = glm::vec3(0.0f),
        const glm::vec3& Normal = glm::vec3(0.0f),
        const glm::vec2& TexCoords = glm::vec2(0.0f),
        const glm::vec3& Tangent = glm::vec3(0.0f),
        const glm::vec3& Bitangent = glm::vec3(0.0f))
        : mPosition(Position),
          mNormal(Normal),
          mTexCoords(TexCoords),
          mTangent(Tangent),
          mBitangent(Bitangent)
    {
    }

    // bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];

    void SetPosition(aiVector3D* InVector)
    {
        mPosition.x = InVector->x;
        mPosition.y = InVector->y;
        mPosition.z = InVector->z;
    }
    void SetTexCoords(aiVector3D* InTexCoords)
    {
        mTexCoords.x = InTexCoords->x;
        mTexCoords.y = InTexCoords->y;
    }
    void SetNormal(aiVector3D* InNormal)
    {
        mNormal.x = InNormal->x;
        mNormal.y = InNormal->y;
        mNormal.z = InNormal->z;
    }
};

struct PFTexture
{
    unsigned int mID;
    std::string mType;
};

struct PFTextureCreationInfo
{
    unsigned int mIndex = 0;
    unsigned int mId = 0;
    std::string mPath;
    std::string mType;
    const class aiTexture* embeddedTexture = nullptr;
    bool bFlipTextureOnLoad = false;
};

struct PFMeshCreationInfo
{
    std::vector<PFVertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::map<std::string, std::vector<unsigned int>> mTextureIndices;
};

class PFMesh
{

  public:
    std::vector<PFVertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::vector<PFTexture> mTextures;
    unsigned int mVAO;

  public:
    PFMesh(std::vector<PFVertex> InVertices, std::vector<unsigned int> InIndices, std::vector<PFTexture> InTextures);
    void Draw(PFShader& shader);

  private:
    //  render data
    unsigned int mVBO, mEBO;
    void SetupMesh();
};