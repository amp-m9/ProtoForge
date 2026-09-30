#include "Model.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <iostream>
#include <filesystem>
#include "Utils.h"
#include "Camera.h"
#include "Shader.h"

void Model::Draw(Camera& Camera, Shader& Shader) 
{
    Shader.Use();
    Shader.SetMat4("projection", Camera.GetProjectionMatrix());
    Shader.SetMat4("view", Camera.GetViewMatrix());
    Shader.SetMat4("model", mModelTransform);

    for(Mesh& mesh : mMeshes)
    {
        mesh.Draw(Shader);
    }
}

void Model::LoadModel(const std::string path) 
{
    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(path, aiProcess_Triangulate|aiProcess_FlipUVs);
    if(!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << Importer.GetErrorString() << std::endl;
        return;
    }

    mDirectory = path.substr(0, path.find_last_of('/'));

    ProcessNode(Scene->mRootNode, Scene);
}

void Model::ProcessNode(aiNode *InNode, const aiScene *InScene) 
{
     // process all the node's meshes (if any)
    for(unsigned int i = 0; i < InNode->mNumMeshes; i++)
    {
        aiMesh *mesh = InScene->mMeshes[InNode->mMeshes[i]]; 
        mMeshes.push_back(ProcessMesh(mesh, InScene));			
    }

    // then do the same for each of its children
    for(unsigned int i = 0; i < InNode->mNumChildren; i++)
    {
        ProcessNode(InNode->mChildren[i], InScene);
    }
}

Mesh Model::ProcessMesh(aiMesh* InMesh, const aiScene* InScene)
{
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<Texture> Textures;

    for (unsigned int i = 0; i< InMesh->mNumVertices; i++)
    {
        Vertex Vert;

        Vert.SetPosition(&InMesh->mVertices[i]);
        Vert.SetNormal(&InMesh->mNormals[i]);

        if(InMesh->mTextureCoords[0])
        {
            Vert.SetTexCoords(&InMesh->mTextureCoords[0][i]);
        }
        else
        {
            Vert.mTexCoords = glm::vec2(0.f);
        }

        Vertices.push_back(Vert);
    }

    for(unsigned int i = 0; i< InMesh->mNumFaces; i++)
    {
        aiFace Face = InMesh->mFaces[i];
        for(unsigned int j = 0; j< Face.mNumIndices; j++)
        {
            Indices.push_back(Face.mIndices[j]);
        }
    }
    
    if(InMesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = InScene->mMaterials[InMesh->mMaterialIndex];

        // 1. diffuse maps
        std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, TEXTURE_DIFFUSE);
        Textures.insert(Textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, TEXTURE_SPECULAR);
        Textures.insert(Textures.end(), specularMaps.begin(), specularMaps.end());
        // // 3. normal maps
        // std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, TEXTURE_NORMAL);
        // Textures.insert(Textures.end(), normalMaps.begin(), normalMaps.end());
        // // 4. height maps
        // std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, TEXTURE_HEIGHT);
        // Textures.insert(Textures.end(), heightMaps.begin(), heightMaps.end());
    }  

    return Mesh(Vertices, Indices, Textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* InMaterial, aiTextureType InTextureType, std::string InTextureTypeName)
{
    std::vector<Texture> Textures;
    for(unsigned int i = 0; i < InMaterial->GetTextureCount(InTextureType); i++)
    {
        aiString Str;
        InMaterial->GetTexture(InTextureType, i, &Str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool bSkip = false;
        
        for(unsigned int j = 0; j < mTextures.size(); j++)
        {
            if(std::strcmp(mTextures[j].mPath.data(), Str.C_Str()) == 0)
            {
                Textures.push_back(mTextures[j]);
                bSkip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }

        if(!bSkip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.mID = TextureFromFile(Str.C_Str(), this->mDirectory);
            texture.mType = InTextureTypeName;
            texture.mPath = Str.C_Str();
            Textures.push_back(texture);
            mTextures.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }

    return Textures;
}
