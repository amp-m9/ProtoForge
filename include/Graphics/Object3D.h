#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Graphics/Rotation.h"
#include "Graphics/Texture.h"

class PFObject3D
{
public:
    std::weak_ptr<PFObject3D> GetParent(){ return mParent;};
    const std::vector<std::weak_ptr<PFObject3D>> GetChildren() { return mChildren; };
    const char* GetDisplayName(){ return mDisplayName.c_str(); }
    void SetDisplayName(std::string InDisplayName){  mDisplayName = InDisplayName; }

    //transformations
    void SetPosition(const glm::vec3 Position);
    void SetScale(const glm::vec3 Scale);
    void SetRotation(const float X, const float Y, const float Z);
    void UpdateTransformMatrix();
    const glm::mat4 GetTransform(){ return mLocalTransform; };

protected:
    std::weak_ptr<PFObject3D> mParent;
    std::vector<std::weak_ptr<PFObject3D>> mChildren;
    std::string mDisplayName;
    glm::mat4 mLocalTransform = glm::mat4(1.0f);
    glm::vec3 mPosition = glm::vec3(1.0f);
    glm::vec3 mScale = glm::vec3(1.0f);
    glm::vec3 mRotationEuler = glm::vec3(0.0);
    glm::mat4 mRotationQuat = glm::mat4(1.0f);
    RotationType mRotationType = RotationType::XYZ;
};