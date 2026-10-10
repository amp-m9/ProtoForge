#pragma once

#include <assimp/mesh.h>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include "Shader.h"
#include <string>
#include <vector>

#include "Graphics/AABB.h"
#include "Graphics/Object3D.h"
#include "Graphics/Rotation.h"
#include "Graphics/Texture.h"
#include "Graphics/Vertex.h"

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

class PFMesh : public PFObject3D
{
public:
    PFMesh(std::vector<PFVertex> InVertices, std::vector<unsigned int> InIndices, std::vector<PFTexture> InTextures);
    void Draw(PFShader& shader);
    
public:
    std::vector<PFVertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::vector<PFTexture> mTextures;
    unsigned int mVAO;

private:
    unsigned int mVBO, mEBO;
    void SetupMesh();
};