#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include"Helpers.h"
#include "Shader.h"

struct VAOData
{
        GLuint Position = 0;
        GLint CountAtPosition = 3; // Size of the vertex attribute at position.
        GLenum TypeAtPosition = GL_FLOAT;
        GLsizei StrideAtPosition = sizeof(float) * 3; // Byte offset between consecutive vertex attributes. If 0, the vertex attributes are understood to be tightly packed.
        GLboolean NormalizedAtPosition = GL_FALSE;
};

class Model
{
    private:
        unsigned int VBO, VAO;
        Shader* ObjectShader;
        glm::vec3 Position, Rotation, Scale;

    public:
        Model(float data[], VAOData vaoData, GLenum usage = GL_STATIC_DRAW)
        {
                
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, usage);

            // change to do for each attribute in vaoData, where each VAOData is for a different attribute. For now, we just assume one attribute.
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        };

        void Draw(Camera& Camera)
        {
            ObjectShader->use();
            ObjectShader->setMat4("projection", Camera.GetProjectionMatrix());
            ObjectShader->setMat4("view", Camera.GetViewMatrix());
            ObjectShader->setMat4("model", GetModelMatrix());
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        };
        
        glm::mat4 GetModelMatrix()
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, Position);
            model = glm::scale(model, Scale);
            return model;
        };
        
        void SetShader(Shader* shader)
        {
            ObjectShader = shader;
        };

        void SetShader(const char* vertexPath, const char* fragmentPath)
        {
            ObjectShader = new Shader(vertexPath, fragmentPath);
        };
        
        const Shader* GetShader()
        {
            return ObjectShader;
        };
        
    public:
        template<typename T>
        const void SetShaderUniform(const std::string& name, const T& v){}
        template<>
        const void SetShaderUniform<glm::mat4>(const std::string& name, const glm::mat4& v)
        {
            ObjectShader->use();
            ObjectShader->setMat4(name, v);
        }
        template<>
        const void SetShaderUniform<glm::mat3>(const std::string& name, const glm::mat3& v)
        {
            ObjectShader->use();
            ObjectShader->setMat3(name, v);
        }
        template<>
        const void SetShaderUniform<glm::vec4>(const std::string& name, const glm::vec4& v)
        {
            ObjectShader->use();
            ObjectShader->setVec4(name, v);
        }
        template<>
        const void SetShaderUniform<glm::vec3>(const std::string& name, const glm::vec3& v)
        {
            ObjectShader->use();
            ObjectShader->setVec3(name, v);
        }
};

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
    }
}