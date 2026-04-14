#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <format>
#include <vector>
#include <assimp/Importer.hpp>
#include "Helpers.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"

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


struct VAOData
{
    GLsizei Stride = sizeof(float) * 3;
    std::vector<VertexAttributeBase> Attributes;
};

enum class TextureType 
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
    void SetPosition(glm::vec3 pos) { Position = pos; }
    void SetRotation(float X, float Y, float Z) { Rotation = glm::vec3(X, Y, Z); }
    void SetRotation(glm::vec3 rot) { Rotation = rot; }
    void SetScale(float X, float Y, float Z) { Scale = glm::vec3(X, Y, Z); }
    void SetScale(glm::vec3 scl) { Scale = scl; }

    void Draw(Camera* Camera, DirectionalLight* DirLight, std::vector<PointLight*> PointLights)
    {
        ObjectShader->Use();
        ObjectShader->SetMat4("projection", Camera->GetProjectionMatrix());
        ObjectShader->SetMat4("view", Camera->GetViewMatrix());
        ObjectShader->SetMat4("model", GetModelMatrix());
        ObjectShader->SetMat3("normalMatrix", GetNormalMatrix());

        if(DirLight)
        {
            ObjectShader->SetVec3("dirLight.direction", DirLight->Direction);
            ObjectShader->SetVec3("dirLight.ambient", DirLight->Ambient);
            ObjectShader->SetVec3("dirLight.diffuse", DirLight->Diffuse);
            ObjectShader->SetVec3("dirLight.specular", DirLight->Specular);
        }

        if(PointLights.size()>0)
        {
            for(int i = 0; i<PointLights.size(); i ++)
            {
                PointLight* InPointLight = PointLights[i];
                std::string PointLightStr {"pointLights[" + std::to_string(i) + "]"};

                ObjectShader->SetVec3(PointLightStr + ".position", InPointLight->Position);

                ObjectShader->SetVec3(PointLightStr + ".ambient", InPointLight->Ambient);
                ObjectShader->SetVec3(PointLightStr + ".diffuse", InPointLight->Diffuse);
                ObjectShader->SetVec3(PointLightStr + ".specular", InPointLight->Specular);

                ObjectShader->SetFloat(PointLightStr + ".constant", InPointLight->Constant);
                ObjectShader->SetFloat(PointLightStr + ".linear", InPointLight->Linear);
                ObjectShader->SetFloat(PointLightStr + ".quadratic", InPointLight->Quadratic);
            }
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
        model = glm::rotate(model, glm::radians(Rotation.x), glm::vec3(1.f, 0.f, 0.f));
        model = glm::rotate(model, glm::radians(Rotation.y), glm::vec3(0.f, 1.f, 0.f));
        model = glm::rotate(model, glm::radians(Rotation.z), glm::vec3(0.f, 0.f, 1.f));

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
        ObjectShader->Use();
        ObjectShader->SetVec3("material.ambient", MaterialPtr->Ambient);
        ObjectShader->SetVec3("material.diffuse", MaterialPtr->Diffuse);
        ObjectShader->SetVec3("material.specular", MaterialPtr->Specular);
        ObjectShader->SetFloat("material.shininess", MaterialPtr->Shininess);
        ObjectShader->SetBool("material.bUseDiffuseMap", MaterialPtr->bUseDiffuseMap);
        ObjectShader->SetBool("material.bUseSpecularMap", MaterialPtr->bUseSpecularMap);
        
        if(MaterialPtr->bUseDiffuseMap)
        {
            ObjectShader->SetInt("material.DiffuseMap", 0);
        }

        if(MaterialPtr->bUseSpecularMap)
        {
            ObjectShader->SetInt("material.SpecularMap", 1);
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
    ObjectShader->Use();
    ObjectShader->SetMat4(name, v);
}

template <> const void Model::SetShaderUniform<glm::mat3>(const std::string& name, const glm::mat3& v)
{
    ObjectShader->Use();
    ObjectShader->SetMat3(name, v);
}

template <> const void Model::SetShaderUniform<glm::vec4>(const std::string& name, const glm::vec4& v)
{
    ObjectShader->Use();
    ObjectShader->SetVec4(name, v);
}

template <> const void Model::SetShaderUniform<glm::vec3>(const std::string& name, const glm::vec3& v)
{
    ObjectShader->Use();
    ObjectShader->SetVec3(name, v);
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