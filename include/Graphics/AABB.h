#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <limits>
#include "Graphics/Vertex.h"
#include "Utils/Mat4Constants.h"

struct AABB 
{
    AABB() = default;
    
    glm::vec3 mHalfVector = glm::vec3(0.0f);
    glm::vec3 mOrigin = glm::vec3(0.0f);

    AABB(const std::vector<PFVertex>& Vertices, const glm::mat4& Transform = Mat4::Identity)
    {
        if (Vertices.empty())
        {
            return;
        }

        float MinX = std::numeric_limits<float>::max();
        float MaxX = std::numeric_limits<float>::lowest();
        float MinY = std::numeric_limits<float>::max();
        float MaxY = std::numeric_limits<float>::lowest();
        float MinZ = std::numeric_limits<float>::max();
        float MaxZ = std::numeric_limits<float>::lowest();
        
        for (const auto& Vertex : Vertices)
        {
            glm::vec3 Pos = Transform * glm::vec4(Vertex.mPosition, 1.0f);
            
            MinX = glm::min(Pos.x, MinX);
            MinY = glm::min(Pos.y, MinY);
            MinZ = glm::min(Pos.z, MinZ);
            MaxX = glm::max(Pos.x, MaxX);
            MaxY = glm::max(Pos.y, MaxY);
            MaxZ = glm::max(Pos.z, MaxZ);
        }

        float XPos = (MinX + MaxX) / 2;
        float YPos = (MinY + MaxY) / 2;
        float ZPos = (MinZ + MaxZ) / 2;

        mOrigin = glm::vec3(XPos, YPos, ZPos);
        mHalfVector = glm::vec3
            (
                glm::abs(MaxX - MinX) / 2,
                glm::abs(MaxY - MinY) / 2,
                glm::abs(MaxZ - MinZ) / 2
            );
    };
    
    glm::vec3 Min() const { return mOrigin - mHalfVector; }
    glm::vec3 Max() const { return mOrigin + mHalfVector; } 

    AABB operator+(const AABB& Other) const
    {
        AABB Result(*this);
        glm::vec3 Lower = glm::min((Other.mOrigin - Other.mHalfVector), (mOrigin - mHalfVector));
        glm::vec3 Upper = glm::max((Other.mOrigin + Other.mHalfVector), (mOrigin + mHalfVector));

        Result.mOrigin = glm::vec3
            (
                (Lower.x + Upper.x) / 2,
                (Lower.y + Upper.y) / 2,
                (Lower.z + Upper.z) / 2
            );

        Result.mHalfVector = glm::vec3
            (
                (Upper.x - Lower.x) / 2,
                (Upper.y - Lower.y) / 2,
                (Upper.z - Lower.z) / 2
            );

        return Result;
    }
};