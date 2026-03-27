#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Helpers.h"
#include "Shader.h"
#include "Camera.h"

struct VAOData
{
    GLuint Position = 0;
    GLint CountAtPosition = 3; // Size of the vertex attribute at position.

    // Float etc?
    GLenum TypeAtPosition = GL_FLOAT;

    // Byte offset between consecutive vertex attributes. If 0, the vertex attributes are understood to be tightly packed.
    GLsizei StrideAtPosition = sizeof(float) * 3;
    GLboolean NormalizedAtPosition = GL_FALSE;
};

class Model
{
  private:
    unsigned int VBO, VAO, Texture;
    Shader* ObjectShader;
    glm::vec3 Position{glm::vec3(0.f)}, Rotation{glm::vec3(0.f)}, Scale{glm::vec3(1.f)};
    size_t VertexCount = 0;

  public:
    Model(float data[], size_t dataSize, VAOData vaoData, Shader* shader = nullptr, GLenum usage = GL_STATIC_DRAW)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, dataSize, data, usage);
        VertexCount = dataSize / vaoData.StrideAtPosition;

        // change to do for each attribute in vaoData, where each VAOData is for a different attribute. For now, we just assume one attribute.
        glVertexAttribPointer(vaoData.Position, vaoData.CountAtPosition, vaoData.TypeAtPosition, GL_FALSE, vaoData.StrideAtPosition, (void*)0);
        glEnableVertexAttribArray(0);

        if (shader != nullptr)
        {
            ObjectShader = shader;
        }
    };

    void Draw(Camera* Camera)
    {
        glBindVertexArray(VAO);
        ObjectShader->use();
        ObjectShader->setMat4("projection", Camera->GetProjectionMatrix());
        ObjectShader->setMat4("view", Camera->GetViewMatrix());
        ObjectShader->setMat4("model", GetModelMatrix());

        // if (Texture)
        // {
        //     glBindTexture(GL_TEXTURE_2D, Texture);
        // }
        glDrawArrays(GL_TRIANGLES, 0, VertexCount);
    };

    glm::mat4 GetModelMatrix()
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, Position);
        model = glm::scale(model, Scale);
        return model;
    };

    void SetShader(Shader* shader) { ObjectShader = shader; };

    void SetShader(const char* vertexPath, const char* fragmentPath) { ObjectShader = new Shader(vertexPath, fragmentPath); };

    bool LoadTexture(const char* fileLocation, const bool bUseAlpha, const bool bFlipVerticallyOnLoad)
    {
        std::string path = FileSystem::getPath(fileLocation).c_str();
        stbi_set_flip_vertically_on_load(bFlipVerticallyOnLoad);

        int width, height, nrChannels;
        unsigned char* TextureData = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

        if (!TextureData)
        {
            return false;
        }

        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // actually loads the texture data

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, bUseAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, TextureData);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(TextureData);
        return true;
    }
    Shader* GetShader() { return ObjectShader; };
    void FreeAll()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteTextures(1, &Texture);
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
float TexturedCube[] = {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
                        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

                        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

                        -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

                        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
                        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
                        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

                        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

float TexturedSquare[] = {
    // Positions         // Texture coords
    0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
    0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
    -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
};
} // namespace DefaultModels