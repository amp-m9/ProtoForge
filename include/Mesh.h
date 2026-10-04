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

struct Vertex
{
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec2 mTexCoords;
    glm::vec3 mTangent;
    glm::vec3 mBitangent;

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

struct Texture
{
    unsigned int mID;
    std::string mType;
};

struct TextureInfo
{
    unsigned int mIndex;
    unsigned int mId;
    std::string mPath;
    std::string mType;
    class aiTexture* embeddedTexture;
    bool bFlipTextureOnLoad = false;
};

struct MeshCreationInfo
{
    std::vector<Vertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::map<std::string, std::vector<unsigned int>> mTextureIndices;
};

class Mesh
{

  public:
    std::vector<Vertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::vector<Texture> mTextures;
    unsigned int mVAO;

  public:
    Mesh(std::vector<Vertex> InVertices, std::vector<unsigned int> InIndices, std::vector<Texture> InTextures);
    void Draw(Shader& shader);

  private:
    //  render data
    unsigned int mVBO, mEBO;

    void SetupMesh();
};