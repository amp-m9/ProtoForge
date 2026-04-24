#include "Model.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <iostream>
#include <filesystem>
#include "Utils.h"

void Model::Draw(Shader& shader) 
{
    for(auto& mesh : mMeshes)
    {
        mesh.Draw(shader);
    }
}

void Model::LoadModel(std::string path) 
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

void Model::ProcessNode(aiNode *node, const aiScene *scene) 
{
        // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        mMeshes.push_back(ProcessMesh(mesh, scene));			
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }

}

Mesh Model::ProcessMesh(aiMesh* InMesh, const aiScene* scene)
{
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<Texture> InTextures;

    for (unsigned int i = 0; i< InMesh->mNumVertices; i++)
    {
        Vertex Vert;

        Vert.SetPosition(&InMesh->mVertices[i]);
        Vert.SetNormal(&InMesh->mNormals[i]);

        if(InMesh->mTextureCoords[0])
        {
            Vert.SetTexCoords(&InMesh->mTextureCoords[0][i]);
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
        aiMaterial *material = scene->mMaterials[InMesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        InTextures.insert(InTextures.end(), diffuseMaps.begin(), diffuseMaps.end());
        
        std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        InTextures.insert(InTextures.end(), specularMaps.begin(), specularMaps.end());
    }  

    return Mesh(Vertices, Indices, InTextures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType textureType, std::string textureTypeName)
{
    std::vector<Texture> Textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(textureType); i++)
    {
        aiString Str;
        mat->GetTexture(textureType, i, &Str);
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
            texture.mType = textureType;
            texture.mPath = Str.C_Str();
            Textures.push_back(texture);
            mTextures.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }

    return Textures;
}
