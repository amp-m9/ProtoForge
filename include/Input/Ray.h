#pragma once

#include <vector>
#include <glm/glm.hpp>

class PFObject3D;
struct AABB;

struct PFRayHit
{
    PFObject3D* Object;
    glm::vec3 HitLocation;    
};

class PFRay
{
public:
    PFRay(
        const glm::vec2 ScreenSpacePixels,
        const glm::vec2 ViewportSize,
        const glm::mat4& ScreenProjectionMatrix,
        const glm::mat4& ViewMatrix,
        const glm::vec3& CameraPosition,
        const float InLength = 1000,
        bool bDrawDebug = false
    );

    std::vector<PFRayHit>Cast(std::vector<PFObject3D*> Models) const;
    void BroadPhase(std::vector<PFObject3D*>& Models) const;
    std::vector<PFRayHit> NarrowPhase(std::vector<PFObject3D*>& Models) const;
    bool Intersect(const AABB& Box) const;
    bool Intersect(PFObject3D& Object, PFRayHit& Hit) const;
    
    const glm::vec3& GetOrigin() const { return Origin; }
    const glm::vec3& GetForward() const { return Direction; }

private:
    glm::vec3 Origin;
    glm::vec3 End;
    glm::vec3 Direction;
    float Length = 0;
    bool bDrawDebug = false;
};

