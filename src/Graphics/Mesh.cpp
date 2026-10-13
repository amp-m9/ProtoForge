#include <algorithm> 
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "Graphics/Mesh.h"
#include "Graphics/Model.h"
#include "Utils/DebugDraw.h"
#include "Utils/Vec3Constants.h"

PFMesh::PFMesh(std::vector<PFVertex> InVertices, std::vector<unsigned int> InIndices, std::vector<PFTexture> InTextures, const std::string InName) 
    : mVertices{InVertices}, mIndices{InIndices}, mTextures{InTextures}
{
    mName = InName;
    SetupMesh();
}

void PFMesh::Draw(PFShader& shader) 
{
    unsigned int DiffuseNr = 1;
    unsigned int SpecularNr = 1;
    unsigned int NormalNr   = 1;
    unsigned int HeightNr   = 1;

    for (unsigned int i = 0; i < mTextures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string Name = mTextures[i].mType;
        std::string Number;

        if (Name == TEXTURE_DIFFUSE)
        {
            Number = std::to_string(DiffuseNr++);
        }
        else if (Name == TEXTURE_SPECULAR)
        {
            Number = std::to_string(SpecularNr++);
        }
        else if (Name == TEXTURE_HEIGHT)
        {
            Number = std::to_string(HeightNr++);
        }
        else if (Name == TEXTURE_NORMAL)
        {
            Number = std::to_string(NormalNr++);
        }

        shader.SetInt((Name + Number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, mTextures[i].mID);
    }

    glActiveTexture(GL_TEXTURE0);

    //draw
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if (mbDrawBounds)
    {
        DebugDraw::DrawBounds(mBounds);
    }
}

void PFMesh::SetParent(PFModel* InParent)
{
    mParent = InParent;
    UpdateBounds();
}

void PFMesh::UpdateBounds()
{
    UpdateBounds(GetWorldTransform());
}

void PFMesh::UpdateBounds(const glm::mat4& Transform)
{
    mBounds = AABB(mVertices, Transform);
}

std::vector<glm::vec3> PFMesh::GetVerticesInWorldSpace()
{
    std::vector<glm::vec3> WorldSpaceVertices;
    glm::mat4 WorldTransform = GetWorldTransform();
    for(auto& Vertex : mVertices)
    {
        WorldSpaceVertices.push_back(WorldTransform * glm::vec4(Vertex.mPosition, 1.0f));
    }

    return WorldSpaceVertices;
}

const uint PFMesh::GetVertexCount()
{
    return mVertices.size();
}

std::vector<uint> PFMesh::GetIndices()
{
    return mIndices;
}

const glm::mat4 PFMesh::GetWorldTransform()
{
    glm::mat4 ResultingTransform = mLocalTransform;
    if (mParent)
    {
        ResultingTransform = mParent->GetWorldTransform() * ResultingTransform;
    }

    return ResultingTransform;
}

void PFMesh::SetupMesh()
{
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(PFVertex), &mVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);
    
    //vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PFVertex), (void*) 0);

    //vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PFVertex), (void*)offsetof(PFVertex, mNormal)); 

    //vertex texture coord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(PFVertex), (void*)offsetof(PFVertex, mTexCoords)); 

        // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(PFVertex), (void*)offsetof(PFVertex, mTangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(PFVertex), (void*)offsetof(PFVertex, mBitangent));
    {
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(PFVertex), (void*)offsetof(PFVertex, m_BoneIDs));
    
        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(PFVertex), (void*)offsetof(PFVertex, m_Weights));
    }

    glBindVertexArray(0);
}
