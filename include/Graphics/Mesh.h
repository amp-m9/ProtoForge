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
    std::string mName;
};

struct PFObjectTriangleData
{
    std::vector<glm::vec3> Vertices;
    std::vector<glm::uint> Indices;
};

class PFModel;

class PFMesh : public PFObject3D
{
public:
    PFMesh(std::vector<PFVertex> InVertices, std::vector<unsigned int> InIndices, std::vector<PFTexture> InTextures, const std::string InName = std::string());
    void Draw(PFShader& shader);
    void SetParent(PFModel* InParent);
    virtual void UpdateBounds() override;
    void UpdateBounds(const glm::mat4& Transform);

    virtual std::vector<glm::vec3>GetVerticesInWorldSpace() override;
    virtual const uint GetVertexCount() override;
    virtual std::vector<uint>GetIndices() override;
    virtual const glm::mat4 GetWorldTransform() override;
    const PFModel* GetParent(){ return mParent;};
    
    
public:
    std::vector<PFVertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::vector<PFTexture> mTextures;
    unsigned int mVAO;

private:
    PFModel* mParent;
    void SetupMesh();
    unsigned int mVBO, mEBO;
    unsigned int mBoundsVAO, mBoundsVBO;
    bool mbDrawBounds = true;
};