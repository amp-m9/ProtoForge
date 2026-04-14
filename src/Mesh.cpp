#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> InVertices, std::vector<unsigned int> InIndices, std::vector<Texture> InTextures) 
    : mVertices{InVertices}, mIndices{InIndices}, mTextures{InTextures} 
{
    SetupMesh();
}

void Mesh::Draw(Shader& shader) 
{

}

void Mesh::SetupMesh() 
{

}
