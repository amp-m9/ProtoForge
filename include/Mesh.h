#pragma once

#include "Shader.h"
#include <glm/glm.hpp>
#include <assimp/mesh.h>
#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec2 mTexCoords;

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
    std::string mPath;
};

const std::string TEXTURE_DIFFUSE = "texture_diffuse";
const std::string TEXTURE_SPECULAR = "texture_specular";

class Mesh
{
    public: 
        std::vector<Vertex> mVertices;
        std::vector<unsigned int> mIndices;
        std::vector<Texture> mTextures;

        Mesh(std::vector<Vertex> InVertices, std::vector<unsigned int>InIndices, std::vector<Texture> InTextures);
        void Draw(Shader &shader);
    private:
        //  render data
        unsigned int mVAO, VBO, mEBO;

        void SetupMesh();
};