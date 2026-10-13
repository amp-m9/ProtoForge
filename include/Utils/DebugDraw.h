#pragma once

#include <glad/glad.h>
#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include "Utils/Globals.h"

class DebugDraw
{
private:
    inline static unsigned int lineVAO = 0;
    inline static unsigned int lineVBO = 0;
    inline static PFShader* LineShader = nullptr;

    static void InitShadersAndBuffers()
    {
        LineShader = new PFShader("assets/shaders/debug/Line");
        glGenVertexArrays(1, &lineVAO);
        glGenBuffers(1, &lineVBO);
        glBindVertexArray(lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 24, nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
        glBindVertexArray(0);
    }

    static void EnsureInit()
    {
        if (LineShader == nullptr)
        {
            InitShadersAndBuffers();
        }
    }

public:
    static void DrawBounds(AABB Bounds)
    {
        const glm::vec3 Min = Bounds.Min();
        const glm::vec3 Max = Bounds.Max();
        const glm::vec3 LineVertices[] =
        {
            {Min.x, Min.y, Min.z}, {Max.x, Min.y, Min.z},
            {Max.x, Min.y, Min.z}, {Max.x, Max.y, Min.z},
            {Max.x, Max.y, Min.z}, {Min.x, Max.y, Min.z},
            {Min.x, Max.y, Min.z}, {Min.x, Min.y, Min.z},
            {Min.x, Min.y, Max.z}, {Max.x, Min.y, Max.z},
            {Max.x, Min.y, Max.z}, {Max.x, Max.y, Max.z},
            {Max.x, Max.y, Max.z}, {Min.x, Max.y, Max.z},
            {Min.x, Max.y, Max.z}, {Min.x, Min.y, Max.z},
            {Min.x, Min.y, Min.z}, {Min.x, Min.y, Max.z},
            {Max.x, Min.y, Min.z}, {Max.x, Min.y, Max.z},
            {Max.x, Max.y, Min.z}, {Max.x, Max.y, Max.z},
            {Min.x, Max.y, Min.z}, {Min.x, Max.y, Max.z}
        };
        
        DrawLines(LineVertices, 24);
    }

    static void DrawLines(const glm::vec3* vertices, GLsizei vertexCount)
    {
        if(!Globals::MainCam)
        {
            return;
        }

        // This VBO is allocated for 24 vertices; GL_LINES needs vertex pairs.
        assert(vertices != nullptr);
        assert(vertexCount > 0 && vertexCount % 2 == 0);
        assert(Globals::MainCam != nullptr);

        EnsureInit();
        LineShader->Use();
        LineShader->SetVec4("lineColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        LineShader->SetMat4("view", Globals::MainCam->GetViewMatrix());
        LineShader->SetMat4("projection", Globals::MainCam->GetProjectionMatrix());

        glBindVertexArray(lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(vertexCount) * sizeof(*vertices),
            vertices,
            GL_STATIC_DRAW
        );
        
        glDrawArrays(GL_LINES, 0, vertexCount);
        glBindVertexArray(0);
    }
    
    static void DrawLine(glm::vec3 Start, glm::vec3 End)
    {
        if(!Globals::MainCam)
        {
            return;
        }
        EnsureInit();
        LineShader->Use();

        const glm::vec3 LineVertices[] = {Start, End};
        DrawLines(LineVertices, 2);
    }
};