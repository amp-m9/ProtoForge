#pragma once

#include <assimp/vector3.h>
#include <glm/glm.hpp>

#define MAX_BONE_INFLUENCE 4

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
