#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Mesh.h"
#include "Shader.h"
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>

class aiScene;

class Model
{
    public:
        Model(char* path)
        {
            LoadModel(path);
        }
        
        void Draw(Shader &shader);
    
    private:
        std::vector<Mesh> mMeshes;
        std::vector<Texture> mTextures; 

        std::string  mDirectory;

        void LoadModel(std::string path);
        void ProcessNode(aiNode *node, const aiScene *scene);
        Mesh ProcessMesh(aiMesh* InMesh, const aiScene* scene);
        std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType textureType, std::string textureTypeName);
};