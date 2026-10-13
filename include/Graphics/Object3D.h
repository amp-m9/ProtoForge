#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Graphics/Rotation.h"
#include "Graphics/Texture.h"
#include "Graphics/AABB.h"

class PFObject3D
{
public:
    const char* GetDisplayName(){ return mName.c_str(); }
    void SetDisplayName(std::string InDisplayName){  mName = InDisplayName; }

    void SetPosition(const glm::vec3 Position);
    void SetScale(const glm::vec3 Scale);
    void SetRotation(const float X, const float Y, const float Z);
    void UpdateTransformMatrix();
    const glm::mat4 GetTransform(){ return mLocalTransform; };
    virtual const glm::mat4 GetWorldTransform();
    const AABB GetBounds(){ return mBounds; };
    virtual void UpdateBounds() = 0;
    virtual std::vector<glm::vec3>GetVerticesInWorldSpace(){ return std::vector<glm::vec3>(); };
    virtual const uint GetVertexCount(){ return 0; };
    virtual std::vector<uint>GetIndices(){ return std::vector<uint>(); };

protected:
    AABB mBounds;
    std::string mName;
    glm::mat4 mLocalTransform = glm::mat4(1.0f);
    glm::vec3 mPosition = glm::vec3(1.0f);
    glm::vec3 mScale = glm::vec3(1.0f);
    glm::vec3 mRotationEuler = glm::vec3(0.0);
    glm::mat4 mRotationQuat = glm::mat4(1.0f);
    RotationType mRotationType = RotationType::XYZ;
};