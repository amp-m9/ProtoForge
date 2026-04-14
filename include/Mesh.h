#pragma once

#include "Shader.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec2 TexCoords;
};

struct Texture
{
    unsigned int id;
    std::string type;
};

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
        unsigned int VAO, VBO, EBO;

        void SetupMesh();
};