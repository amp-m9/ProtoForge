#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Mesh.h"
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>

class aiScene;
class Camera;
class Shader;

class Model
{
    public:
        Model(const char* path)
        {
            LoadModel(path);
        }
        
        void Draw(Camera& Camera, Shader &shader);
    
    private:
        std::vector<Mesh> mMeshes;
        std::vector<Texture> mTextures; 

        std::string  mDirectory;
        glm::mat4 mModelTransform = glm::mat4(1.0f);
        void LoadModel(const std::string path);
        void ProcessNode(aiNode *node, const aiScene *scene);
        Mesh ProcessMesh(aiMesh* InMesh, const aiScene* scene);
        std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType textureType, std::string textureTypeName);
};