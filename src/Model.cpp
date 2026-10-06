#include "Model.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <iostream>
#include <filesystem>
#include <format>
#include "Utils.h"
#include "Camera.h"
#include "Shader.h"

void PFModel::Draw(PFCamera& Camera, PFShader& Shader)
{
    Shader.Use();
    Shader.SetMat4("projection", Camera.GetProjectionMatrix());
    Shader.SetMat4("view", Camera.GetViewMatrix());
    Shader.SetMat4("model", mModelTransform);

    for (PFMesh& mesh : mMeshes)
    {
        mesh.Draw(Shader);
    }
}

void PFModel::LoadModel(const std::string path, const bool bFlipTexturesVertically)
{
    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_PreTransformVertices);
    if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << Importer.GetErrorString() << std::endl;
        return;
    }

    mDirectory = path.substr(0, path.find_last_of('/'));

    std::vector<PFMeshCreationInfo> MeshCreationInfoArray;
    std::vector<PFTextureCreationInfo> TextureInfoArray;
    ProcessNode(Scene->mRootNode, Scene, MeshCreationInfoArray, TextureInfoArray, bFlipTexturesVertically);
    LoadMeshTextures(TextureInfoArray);
    AddMeshesToModel(MeshCreationInfoArray, TextureInfoArray);
}

void PFModel::ProcessNode(aiNode* InNode, const aiScene* InScene, std::vector<PFMeshCreationInfo>& InMeshCreationInfoArray,
                        std::vector<PFTextureCreationInfo>& InTextureInfoArray, bool bFlipTexturesVertically)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < InNode->mNumMeshes; i++)
    {
        aiMesh* mesh = InScene->mMeshes[InNode->mMeshes[i]];
        InMeshCreationInfoArray.push_back(GatherMeshInfo(InTextureInfoArray, mesh, InScene, bFlipTexturesVertically));
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < InNode->mNumChildren; i++)
    {
        ProcessNode(InNode->mChildren[i], InScene, InMeshCreationInfoArray, InTextureInfoArray, bFlipTexturesVertically);
    }
}

PFMeshCreationInfo PFModel::GatherMeshInfo(std::vector<PFTextureCreationInfo>& OutTextureInfoArray, aiMesh* InMesh, const aiScene* InScene,
                                       const bool bFlipTexturesVertically)
{
    std::vector<PFVertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<PFTexture> Textures;

    for (unsigned int i = 0; i < InMesh->mNumVertices; i++)
    {
        PFVertex Vert;

        Vert.SetPosition(&InMesh->mVertices[i]);
        Vert.SetNormal(&InMesh->mNormals[i]);

        if (InMesh->mTextureCoords[0])
        {
            Vert.SetTexCoords(&InMesh->mTextureCoords[0][i]);
        }
        else
        {
            Vert.mTexCoords = glm::vec2(0.f);
        }

        Vertices.push_back(Vert);
    }

    for (unsigned int i = 0; i < InMesh->mNumFaces; i++)
    {
        aiFace Face = InMesh->mFaces[i];
        for (unsigned int j = 0; j < Face.mNumIndices; j++)
        {
            Indices.push_back(Face.mIndices[j]);
        }
    }

    const auto GetAiType = [](std::string TextureTypeName)
    {
        if (TextureTypeName == TEXTURE_DIFFUSE)
            return (aiTextureType_DIFFUSE);
        if (TextureTypeName == TEXTURE_HEIGHT)
            return (aiTextureType_HEIGHT);
        if (TextureTypeName == TEXTURE_NORMAL)
            return (aiTextureType_NORMALS);
        if (TextureTypeName == TEXTURE_SPECULAR)
            return (aiTextureType_SPECULAR);
        return aiTextureType_UNKNOWN;
    };

    std::map<std::string, std::vector<unsigned int>> TextureIndices;
    if (InMesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = InScene->mMaterials[InMesh->mMaterialIndex];
        for (auto& Type : {TEXTURE_DIFFUSE, TEXTURE_SPECULAR, TEXTURE_NORMAL, TEXTURE_HEIGHT})
        {
            TextureIndices[Type] = GatherMaterialTextures(OutTextureInfoArray, material, GetAiType(Type), Type, InScene, bFlipTexturesVertically);
        }
    }

    PFMeshCreationInfo Info;
    Info.mVertices = Vertices;
    Info.mIndices = Indices;
    Info.mTextureIndices = TextureIndices;

    return Info;
}

std::vector<unsigned int> PFModel::GatherMaterialTextures(std::vector<PFTextureCreationInfo>& OutTextureInfoArray, aiMaterial* MaterialPtr,
                                                        aiTextureType TextureType, std::string TextureTypeName, const aiScene* InScene,
                                                        const bool bFlipTexturesVertically)
{
    std::vector<unsigned int> Textures;
    for (unsigned int i = 0; i < MaterialPtr->GetTextureCount(TextureType); i++)
    {
        aiString TexturePath;
        // auto data = InScene->mTextures[i]->pcData;
        MaterialPtr->GetTexture(TextureType, i, &TexturePath);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool bSkip = false;

        for (unsigned int j = 0; j < OutTextureInfoArray.size(); j++)
        {
            if (std::strcmp(OutTextureInfoArray[j].mPath.data(), TexturePath.C_Str()) == 0)
            {
                Textures.push_back(j);
                bSkip = true;
            }
        }

        if (!bSkip)
        {
            std::pair<const aiTexture*, int> EmbeddedTextureInfo = InScene->GetEmbeddedTextureAndIndex(TexturePath.C_Str());
            PFTextureCreationInfo texture;
            texture.mIndex = EmbeddedTextureInfo.second;
            texture.mType = TextureTypeName;
            texture.mPath = TexturePath.C_Str();
            texture.embeddedTexture = EmbeddedTextureInfo.first;
            texture.bFlipTextureOnLoad = bFlipTexturesVertically;
            OutTextureInfoArray.push_back(texture);
            Textures.push_back(OutTextureInfoArray.size() - 1);
        }
    }
    return Textures;
}

void PFModel::LoadMeshTextures(std::vector<PFTextureCreationInfo>& TextureInfoArray)
{
    // Will soon change to work with texture arrays :)))
    for (PFTextureCreationInfo& Info : TextureInfoArray)
    {
        Info.mId = Info.embeddedTexture
        ? TextureFromEmbedded(*Info.embeddedTexture, Info.bFlipTextureOnLoad)
        : TextureFromFile(Info.mPath, this->mDirectory, Info.bFlipTextureOnLoad);
    }
}

void PFModel::AddMeshesToModel(const std::vector<PFMeshCreationInfo>& MeshInfoArray, std::vector<PFTextureCreationInfo>& TextureInfoArray)
{
    for (const auto& MeshInfo : MeshInfoArray)
    {
        std::vector<PFTexture> Textures;
        for (std::string Type : {TEXTURE_DIFFUSE, TEXTURE_SPECULAR, TEXTURE_NORMAL, TEXTURE_HEIGHT})
        {
            if (MeshInfo.mTextureIndices.contains(Type))
            {
                for (const unsigned int i : MeshInfo.mTextureIndices.at(Type))
                {
                    PFTexture tex;
                    PFTextureCreationInfo TexInfo = TextureInfoArray[i];
                    tex.mID = TexInfo.mId;
                    tex.mType = Type;
                    Textures.push_back(tex);
                }
            }
        }

        mMeshes.push_back(PFMesh(MeshInfo.mVertices, MeshInfo.mIndices, Textures));
    }
}

void PFModel::SetPosition(const glm::vec3 Position) 
{
    // mModelTransform.
}

void PFModel::SetScale(const glm::vec3 Position) {}

PFModel PFModel::Utils::Cube()
{
    const std::vector<PFVertex> TexturedCubeVertices = 
    {
        // positions          // normals           // texture coords
        PFVertex(glm::vec3(-0.5f, -0.5f, -0.5f) , glm::vec3(  0.0f,  0.0f, -1.0f) , glm::vec2( 0.0f, 0.0f)),
        PFVertex(glm::vec3(0.5f, -0.5f, -0.5f)  , glm::vec3( 0.0f,  0.0f, -1.0f)  , glm::vec2(1.0f, 0.0f)),
        PFVertex(glm::vec3(0.5f,  0.5f, -0.5f)  , glm::vec3( 0.0f,  0.0f, -1.0f)  , glm::vec2(1.0f, 1.0f)),
        PFVertex(glm::vec3(0.5f,  0.5f, -0.5f)  , glm::vec3( 0.0f,  0.0f, -1.0f)  , glm::vec2(1.0f, 1.0f)),
        PFVertex(glm::vec3(-0.5f,  0.5f, -0.5f) , glm::vec3(  0.0f,  0.0f, -1.0f) , glm::vec2( 0.0f, 1.0f)),
        PFVertex(glm::vec3(-0.5f, -0.5f, -0.5f) , glm::vec3(  0.0f,  0.0f, -1.0f) , glm::vec2( 0.0f, 0.0f)),
        PFVertex(glm::vec3(-0.5f, -0.5f,  0.5f) , glm::vec3(  0.0f,  0.0f, 1.0f)  , glm::vec2( 0.0f, 0.0f)),
        PFVertex(glm::vec3(0.5f, -0.5f,  0.5f)  , glm::vec3( 0.0f,  0.0f, 1.0f)   , glm::vec2(1.0f, 0.0f)),
        PFVertex(glm::vec3(0.5f,  0.5f,  0.5f)  , glm::vec3( 0.0f,  0.0f, 1.0f)   , glm::vec2(1.0f, 1.0f)),
        PFVertex(glm::vec3(0.5f,  0.5f,  0.5f)  , glm::vec3( 0.0f,  0.0f, 1.0f)   , glm::vec2(1.0f, 1.0f)),
        PFVertex(glm::vec3(-0.5f,  0.5f,  0.5f) , glm::vec3(  0.0f,  0.0f, 1.0f)  , glm::vec2( 0.0f, 1.0f)),
        PFVertex(glm::vec3(-0.5f, -0.5f,  0.5f) , glm::vec3(  0.0f,  0.0f, 1.0f)  , glm::vec2( 0.0f, 0.0f)),
        PFVertex(glm::vec3(-0.5f,  0.5f,  0.5f) , glm::vec3( -1.0f,  0.0f,  0.0f) , glm::vec2( 1.0f, 0.0f)),
        PFVertex(glm::vec3(-0.5f,  0.5f, -0.5f) , glm::vec3( -1.0f,  0.0f,  0.0f) , glm::vec2( 1.0f, 1.0f)),
        PFVertex(glm::vec3(-0.5f, -0.5f, -0.5f) , glm::vec3( -1.0f,  0.0f,  0.0f) , glm::vec2( 0.0f, 1.0f)),
        PFVertex(glm::vec3(-0.5f, -0.5f, -0.5f) , glm::vec3( -1.0f,  0.0f,  0.0f) , glm::vec2( 0.0f, 1.0f)),
        PFVertex(glm::vec3(-0.5f, -0.5f,  0.5f) , glm::vec3( -1.0f,  0.0f,  0.0f) , glm::vec2( 0.0f, 0.0f)),
        PFVertex(glm::vec3(-0.5f,  0.5f,  0.5f) , glm::vec3( -1.0f,  0.0f,  0.0f) , glm::vec2( 1.0f, 0.0f)),
        PFVertex(glm::vec3(0.5f,  0.5f,  0.5f)  , glm::vec3( 1.0f,  0.0f,  0.0f)  , glm::vec2(1.0f, 0.0f)),
        PFVertex(glm::vec3(0.5f,  0.5f, -0.5f)  , glm::vec3( 1.0f,  0.0f,  0.0f)  , glm::vec2(1.0f, 1.0f)),
        PFVertex(glm::vec3(0.5f, -0.5f, -0.5f)  , glm::vec3( 1.0f,  0.0f,  0.0f)  , glm::vec2(0.0f, 1.0f)),
        PFVertex(glm::vec3(0.5f, -0.5f, -0.5f)  , glm::vec3( 1.0f,  0.0f,  0.0f)  , glm::vec2(0.0f, 1.0f)),
        PFVertex(glm::vec3(0.5f, -0.5f,  0.5f)  , glm::vec3( 1.0f,  0.0f,  0.0f)  , glm::vec2(0.0f, 0.0f)),
        PFVertex(glm::vec3(0.5f,  0.5f,  0.5f)  , glm::vec3( 1.0f,  0.0f,  0.0f)  , glm::vec2(1.0f, 0.0f)),
        PFVertex(glm::vec3(-0.5f, -0.5f, -0.5f) , glm::vec3(  0.0f, -1.0f,  0.0f) , glm::vec2( 0.0f, 1.0f)),
        PFVertex(glm::vec3(0.5f, -0.5f, -0.5f)  , glm::vec3( 0.0f, -1.0f,  0.0f)  , glm::vec2(1.0f, 1.0f)),
        PFVertex(glm::vec3(0.5f, -0.5f,  0.5f)  , glm::vec3( 0.0f, -1.0f,  0.0f)  , glm::vec2(1.0f, 0.0f)),
        PFVertex(glm::vec3(0.5f, -0.5f,  0.5f)  , glm::vec3( 0.0f, -1.0f,  0.0f)  , glm::vec2(1.0f, 0.0f)),
        PFVertex(glm::vec3(-0.5f, -0.5f,  0.5f) , glm::vec3(  0.0f, -1.0f,  0.0f) , glm::vec2( 0.0f, 0.0f)),
        PFVertex(glm::vec3(-0.5f, -0.5f, -0.5f) , glm::vec3(  0.0f, -1.0f,  0.0f) , glm::vec2( 0.0f, 1.0f)),
        PFVertex(glm::vec3(-0.5f,  0.5f, -0.5f) , glm::vec3(  0.0f,  1.0f,  0.0f) , glm::vec2( 0.0f, 1.0f)),
        PFVertex(glm::vec3(0.5f,  0.5f, -0.5f)  , glm::vec3( 0.0f,  1.0f,  0.0f)  , glm::vec2(1.0f, 1.0f)),
        PFVertex(glm::vec3(0.5f,  0.5f,  0.5f)  , glm::vec3( 0.0f,  1.0f,  0.0f)  , glm::vec2(1.0f, 0.0f)),
        PFVertex(glm::vec3(0.5f,  0.5f,  0.5f)  , glm::vec3( 0.0f,  1.0f,  0.0f)  , glm::vec2(1.0f, 0.0f)),
        PFVertex(glm::vec3(-0.5f,  0.5f,  0.5f) , glm::vec3(  0.0f,  1.0f,  0.0f) , glm::vec2( 0.0f, 0.0f)),
        PFVertex(glm::vec3(-0.5f,  0.5f, -0.5f) , glm::vec3(  0.0f,  1.0f,  0.0f) , glm::vec2( 0.0f, 1.0f))
    };

    std::vector<unsigned int> cubeIndices = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35
    };

    PFMeshCreationInfo MeshInfo;
    MeshInfo.mIndices = cubeIndices;
    MeshInfo.mVertices = TexturedCubeVertices;
    MeshInfo.mTextureIndices[TEXTURE_DIFFUSE] = {0,1};

    PFTextureCreationInfo TexInfo1;
    TexInfo1.mId = TextureFromData(CreateSolidTexture(255,0,0,255), GL_RGBA8, GL_RGBA, 100,100);
    TexInfo1.mType = TEXTURE_DIFFUSE;

    PFTextureCreationInfo TexInfo2;
    TexInfo2.mId = TextureFromData(CreateSolidTexture(255,255,0,255), GL_RGBA8, GL_RGBA, 100,100);
    TexInfo2.mType = TEXTURE_DIFFUSE;

    std::vector<PFMeshCreationInfo> MeshInfoArray = {MeshInfo}; 
    std::vector<PFTextureCreationInfo> TextureInfoArray = {TexInfo1, TexInfo2};

    PFModel DefaultCubeModel;
    DefaultCubeModel.AddMeshesToModel(MeshInfoArray, TextureInfoArray);
    DefaultCubeModel.SetDisplayName("Cube");

    return DefaultCubeModel;
}
