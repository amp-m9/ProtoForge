#pragma once

#include <vector>
#include <glad/glad.h>
#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include "Utils/Globals.h"
#include "Graphics/AABB.h"

class DebugDraw
{
public:
    static void RenderDebugShapes(float DeltaTime)
    {
        const std::size_t VertexCount = Lines.size() * 2;
        std::vector<glm::vec3> AllLines(VertexCount);

        for (int i = static_cast<int>(Lines.size()) - 1; i >= 0; --i)
        {
            AllLines[i * 2] = Lines[i].Line[0];
            AllLines[(i * 2) + 1] = Lines[i].Line[1];

            if (Lines[i].bPersists)
            {
                continue;
            }

            Lines[i].LifeTime -= DeltaTime;
            if (Lines[i].LifeTime < 0.0f)
            {
                Lines.erase(Lines.begin() + i);
            }
        }

        if (!AllLines.empty())
        {
            DrawLines_Internal(AllLines.data(), static_cast<GLsizei>(AllLines.size()));
        }
    }

    static void DrawPoint(glm::vec3 Point, glm::vec3 InColour = glm::vec3(1.0f, 0.0f, 0.0f), float InLifeTime = 3.f, bool bInPerstists = false)
    {

    }

    static void DrawBounds(const AABB& Bounds, glm::vec3 InColour = glm::vec3(1.0f, 0.0f, 0.0f), float InLifeTime = 3.f, bool bInPerstists = false)
    {
        const glm::vec3 Min = Bounds.Min();
        const glm::vec3 Max = Bounds.Max();

        DrawLine({Min.x, Min.y, Min.z}, {Max.x, Min.y, Min.z}, InColour, InLifeTime, bInPerstists);
        DrawLine({Max.x, Min.y, Min.z}, {Max.x, Max.y, Min.z}, InColour, InLifeTime, bInPerstists);
        DrawLine({Max.x, Max.y, Min.z}, {Min.x, Max.y, Min.z}, InColour, InLifeTime, bInPerstists);
        DrawLine({Min.x, Max.y, Min.z}, {Min.x, Min.y, Min.z}, InColour, InLifeTime, bInPerstists);
        DrawLine({Min.x, Min.y, Max.z}, {Max.x, Min.y, Max.z}, InColour, InLifeTime, bInPerstists);
        DrawLine({Max.x, Min.y, Max.z}, {Max.x, Max.y, Max.z}, InColour, InLifeTime, bInPerstists);
        DrawLine({Max.x, Max.y, Max.z}, {Min.x, Max.y, Max.z}, InColour, InLifeTime, bInPerstists);
        DrawLine({Min.x, Max.y, Max.z}, {Min.x, Min.y, Max.z}, InColour, InLifeTime, bInPerstists);
        DrawLine({Min.x, Min.y, Min.z}, {Min.x, Min.y, Max.z}, InColour, InLifeTime, bInPerstists);
        DrawLine({Max.x, Min.y, Min.z}, {Max.x, Min.y, Max.z}, InColour, InLifeTime, bInPerstists);
        DrawLine({Max.x, Max.y, Min.z}, {Max.x, Max.y, Max.z}, InColour, InLifeTime, bInPerstists);
        DrawLine({Min.x, Max.y, Min.z}, {Min.x, Max.y, Max.z}, InColour, InLifeTime, bInPerstists);
    }


    static void DrawLines(const glm::vec3* vertices, GLsizei vertexCount, glm::vec3 InColour = glm::vec3(1.0f, 0.0f, 0.0f), float InLifeTime = 3.f, bool bInPerstists = false)
    {
        for(int i = 0; i<vertexCount; i+=2)
        {
            Lines.push_back(DebugLine(vertices[i], vertices[i+1], InColour, InLifeTime, bInPerstists));
        }
    }

    static void DrawLine(glm::vec3 Start, glm::vec3 End, glm::vec3 InColour = glm::vec3(1.0f, 0.0f, 0.0f), float InLifeTime = 3.f, bool bInPerstists = false)
    {
        Lines.push_back(DebugLine(Start, End, InColour, InLifeTime, bInPerstists));
    }

private:
    struct BaseDebugDrawData
    {
        BaseDebugDrawData(glm::vec3 InColour = glm::vec3(1.0f, 0.0f, 0.0f), float InLifeTime = 3.f, bool bInPerstists = false)
        : Colour(InColour), LifeTime(InLifeTime), bPersists(bInPerstists){}
        glm::vec3 Colour;
        float LifeTime = -1.f;
        bool bPersists;
    };

    struct DebugLine : public BaseDebugDrawData
    {
        DebugLine(glm::vec3 Start, glm::vec3 End, glm::vec3 InColour = glm::vec3(1.0f, 0.0f, 0.0f), float InLifeTime = 3.f, bool bInPerstists = false)
        : BaseDebugDrawData(InColour, InLifeTime, bInPerstists)
        {
            Line[0] = Start;
            Line[1] = End;
        }

        glm::vec3 Line[2];
    };

    struct DebugPoint : public BaseDebugDrawData
    {
        DebugPoint(glm::vec3 InPoint, glm::vec3 InColour = glm::vec3(1.0f, 0.0f, 0.0f), float InLifeTime = 3.f, bool bInPerstists = false) 
        : BaseDebugDrawData(InColour, InLifeTime, bInPerstists), Point(InPoint){}
        glm::vec3 Point;
    };

    // struct DebugBounds : public BaseDebugDrawData
    // {
    //     DebugBounds(const AABB& InBounds, glm::vec3 InColour = glm::vec3(1.0f, 0.0f, 0.0f), float InLifeTime = 3.f, bool bInPerstists = false)
    //     : Bounds(InBounds), BaseDebugDrawData(InColour, InLifeTime, bInPerstists){};
    //     AABB Bounds;
    // };
    // inline static std::vector<DebugBounds> Bounds;

    inline static unsigned int lineVAO = 0;
    inline static unsigned int lineVBO = 0;
    inline static PFShader* LineShader = nullptr;

    inline static std::vector<DebugLine> Lines;
    inline static std::vector<DebugPoint> Points;

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

    static void DrawLines_Internal(const glm::vec3* vertices, GLsizei vertexCount)
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
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexCount) * sizeof(*vertices), vertices, GL_STATIC_DRAW);

        glDrawArrays(GL_LINES, 0, vertexCount);
        glBindVertexArray(0);
    }
};