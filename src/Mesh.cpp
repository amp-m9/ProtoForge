#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> InVertices, std::vector<unsigned int> InIndices, std::vector<Texture> InTextures) 
    : mVertices{InVertices}, mIndices{InIndices}, mTextures{InTextures} 
{
    SetupMesh();
}

void Mesh::Draw(Shader& shader) 
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
}

void Mesh::SetupMesh() 
{
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);
    
    //vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);

    //vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mNormal)); 

    //vertex texture coord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mTexCoords)); 

        // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mTangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mBitangent));
    {
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
    
        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    }

    glBindVertexArray(0);
}
