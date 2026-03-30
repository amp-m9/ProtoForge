#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Helpers.h"
#include "Shader.h"
#include "Camera.h"

// clang-format off
// Specialisations for each type
template <typename T> struct GLType;
template <> struct GLType<float>         { static constexpr GLenum value = GL_FLOAT; };
template <> struct GLType<double>        { static constexpr GLenum value = GL_DOUBLE; };
template <> struct GLType<int>           { static constexpr GLenum value = GL_INT; };
template <> struct GLType<unsigned int>  { static constexpr GLenum value = GL_UNSIGNED_INT; };
template <> struct GLType<short>         { static constexpr GLenum value = GL_SHORT; };
template <> struct GLType<unsigned char> { static constexpr GLenum value = GL_UNSIGNED_BYTE; };
// clang-format on

struct VertexAttributeBase
{
  public:
    GLuint Position = 0;
    size_t Offset = 0;
    GLint CountAtPosition = 3;
    GLboolean NormalizedAtPosition = GL_FALSE;
    GLenum TypeAtPosition = GL_FLOAT;
};

template <typename T> struct VertexAttribute : VertexAttributeBase
{
    VertexAttribute<T>() { TypeAtPosition = GLType<T>::value; }
    size_t GetSize() { return sizeof(T) * CountAtPosition; }
    GLenum GetType() { return TypeAtPosition; }
};

struct Material
{
    bool bUseDiffuseMap = false;
    unsigned int DiffuseMap = 0;

    bool bUseSpecularMap = false;
    unsigned int SpecularMap = 0;
    
    glm::vec3 Ambient{1.0f, 0.5f, 0.31f};
    glm::vec3 Diffuse{1.0f, 0.5f, 0.31f};
    glm::vec3 Specular{0.5f};
    float Shininess = 32.f;
};

struct Light 
{
    glm::vec3 Position {0.f};
    glm::vec3 ambient{0.2f, 0.2f, 0.2f};
    glm::vec3 diffuse{0.5f, 0.5f, 0.5f};
    glm::vec3 specular{1.f};
};

struct VAOData
{
    GLsizei Stride = sizeof(float) * 3;
    std::vector<VertexAttributeBase> Attributes;
};

enum TextureType
{
    Diffuse,
    Specular
};

class Model
{
  private:
    unsigned int VBO, VAO;
    Shader* ObjectShader;
    Material* MaterialPtr;
    
    glm::vec3 Position{glm::vec3(0.f)};
    glm::vec3 Rotation{glm::vec3(0.f)};
    glm::vec3 Scale{glm::vec3(1.f)};
    
    size_t VertexCount = 0;

  public:
    Model(float data[], size_t dataSize, VAOData vaoData, Shader* shader = nullptr, GLenum usage = GL_STATIC_DRAW)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, dataSize, data, usage);

        VertexCount = dataSize / vaoData.Stride;
        for (auto& attr : vaoData.Attributes)
        {
            glVertexAttribPointer(attr.Position, attr.CountAtPosition, attr.TypeAtPosition, GL_FALSE, vaoData.Stride, (void*)(attr.Offset));
            glEnableVertexAttribArray(attr.Position);
        }

        if (shader != nullptr)
        {
            ObjectShader = shader;
        }
    };

    const glm::vec3& GetPosition() { return Position; }
    const glm::vec3& GetRotation() { return Rotation; }
    const glm::vec3& GetScale() { return Scale; }

    void SetPosition(float X, float Y, float Z) { Position = glm::vec3(X, Y, Z); }
    void SetRotation(float X, float Y, float Z) { Rotation = glm::vec3(X, Y, Z); }
    void SetScale(float X, float Y, float Z) { Scale = glm::vec3(X, Y, Z); }

    void Draw(Camera* Camera, Light* SceneLight = nullptr)
    {
        ObjectShader->use();
        ObjectShader->setMat4("projection", Camera->GetProjectionMatrix());
        ObjectShader->setMat4("view", Camera->GetViewMatrix());
        ObjectShader->setMat4("model", GetModelMatrix());
        ObjectShader->setMat3("normalMatrix", GetNormalMatrix());

        if(SceneLight)
        {
            ObjectShader->setVec3("light.position", SceneLight->Position);
            ObjectShader->setVec3("light.ambient", SceneLight->ambient);
            ObjectShader->setVec3("light.diffuse", SceneLight->diffuse);
            ObjectShader->setVec3("light.specular", SceneLight->specular);
        }

        if (MaterialPtr)
        {
            if(MaterialPtr->bUseDiffuseMap)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, MaterialPtr->DiffuseMap);
            }

            if(MaterialPtr->bUseSpecularMap)
            {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, MaterialPtr->SpecularMap);
            }
        }

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, VertexCount);
    };

    glm::mat4 GetModelMatrix()
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, Position);
        model = glm::scale(model, Scale);
        return model;
    };

    glm::mat3 GetNormalMatrix()
    {
        glm::mat4 model = GetModelMatrix();
        return glm::transpose(glm::inverse(glm::mat3(model)));
    }

    void SetShader(Shader* shader) { ObjectShader = shader; };

    void SetShader(const char* vertexPath, const char* fragmentPath) { ObjectShader = new Shader(vertexPath, fragmentPath); };
    void SetMaterial(Material* material) { MaterialPtr = material; UpdateMaterialUniforms(); };
    
    void UpdateMaterialUniforms()
    {
        ObjectShader->use();
        ObjectShader->setVec3("material.ambient", MaterialPtr->Ambient);
        ObjectShader->setVec3("material.diffuse", MaterialPtr->Diffuse);
        ObjectShader->setVec3("material.specular", MaterialPtr->Specular);
        ObjectShader->setFloat("material.shininess", MaterialPtr->Shininess);
        ObjectShader->setBool("material.bUseDiffuseMap", MaterialPtr->bUseDiffuseMap);
        ObjectShader->setBool("material.bUseSpecularMap", MaterialPtr->bUseSpecularMap);
        
        if(MaterialPtr->bUseDiffuseMap)
        {
            ObjectShader->setInt("material.DiffuseMap", 0);
        }

        if(MaterialPtr->bUseSpecularMap)
        {
            ObjectShader->setInt("material.SpecularMap", 1);
        }

    };

    bool LoadTexture(const char* fileLocation, const bool bUseAlpha, const bool bFlipVerticallyOnLoad, TextureType type = TextureType::Diffuse)
    {
        std::string path = FileSystem::getPath(fileLocation).c_str();
        stbi_set_flip_vertically_on_load(bFlipVerticallyOnLoad);

        int width, height, nrChannels;
        unsigned char* TextureData = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

        if (!TextureData)
        {
            return false;
        }

        if(!MaterialPtr)
        {
            MaterialPtr = new Material();
        }

        const bool bIsDiffuseMap = (type == TextureType::Diffuse);
        if(bIsDiffuseMap)
        {
            MaterialPtr->bUseDiffuseMap = true;
        }
        else if(!bIsDiffuseMap)
        {
            MaterialPtr->bUseSpecularMap = true;
        }

        unsigned int& TextureID = (bIsDiffuseMap ? MaterialPtr->DiffuseMap : MaterialPtr->SpecularMap);

        glGenTextures(1, &TextureID);
        glBindTexture(GL_TEXTURE_2D, TextureID);
        
        // actually loads the texture data into OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, bUseAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, TextureData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        UpdateMaterialUniforms();
        stbi_image_free(TextureData);
        return true;
    }
    Shader* GetShader() { return ObjectShader; };
    void FreeAll()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        if(MaterialPtr)
        {
            glDeleteTextures(1, &MaterialPtr->DiffuseMap);
        }
    }

  public:
    template <typename T> const void SetShaderUniform(const std::string& name, const T& v) {}
};

template <> const void Model::SetShaderUniform<glm::mat4>(const std::string& name, const glm::mat4& v)
{
    ObjectShader->use();
    ObjectShader->setMat4(name, v);
}

template <> const void Model::SetShaderUniform<glm::mat3>(const std::string& name, const glm::mat3& v)
{
    ObjectShader->use();
    ObjectShader->setMat3(name, v);
}

template <> const void Model::SetShaderUniform<glm::vec4>(const std::string& name, const glm::vec4& v)
{
    ObjectShader->use();
    ObjectShader->setVec4(name, v);
}

template <> const void Model::SetShaderUniform<glm::vec3>(const std::string& name, const glm::vec3& v)
{
    ObjectShader->use();
    ObjectShader->setVec3(name, v);
}

namespace DefaultModels
{
// clang-format off
float TexturedCube[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

float TexturedSquare[] = {
    // Positions         // Texture coords
    0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
    0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
    -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
};
} // namespace DefaultModels