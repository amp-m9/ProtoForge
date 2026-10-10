#include "Graphics/Object3D.h"
#include "Utils/Vec3Constants.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtx/quaternion.hpp>

void PFObject3D::SetPosition(const glm::vec3 Position) 
{
    mPosition = Position;
    UpdateTransformMatrix();
}

void PFObject3D::SetScale(const glm::vec3 Scale)
{
    mScale = Scale;
    UpdateTransformMatrix();
}

void PFObject3D::SetRotation(const float X, const float Y, const float Z) 
{
    // x, y, z
    std::pair<float, glm::vec3> rotations[3] = 
    {
        { X, Vec3::XAxis},
        { Y, Vec3::YAxis},
        { Z, Vec3::ZAxis}
    };

    mRotationQuat = glm::mat4(1.0);

    for(auto index : GetRotationOrderFromEnum(mRotationType))
    {
        auto [angle, axis] = rotations[index];
        mRotationQuat = glm::rotate(mRotationQuat, angle, axis);
    }

    UpdateTransformMatrix();
}

void PFObject3D::UpdateTransformMatrix()
{
    mLocalTransform = glm::mat4(1.0f);
    
    mLocalTransform = glm::translate(mLocalTransform, mPosition);
    mLocalTransform *= mRotationQuat;
    mLocalTransform = glm::scale(mLocalTransform, mScale);
}