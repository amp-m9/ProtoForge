#pragma once

#include "Shader.h"
#include <glm/glm.hpp>
#include <assimp/mesh.h>
#include <string>
#include <vector>
#include <map>

#include "Graphics/Vertex.h"
#include "Graphics/Texture.h"
#include "Graphics/AABB.h"

#define TEXTURE_DIFFUSE "texture_diffuse"
#define TEXTURE_SPECULAR "texture_specular"
#define TEXTURE_NORMAL "texture_normal"
#define TEXTURE_HEIGHT "texture_height"

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