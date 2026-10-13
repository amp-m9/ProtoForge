#include "Input/Ray.h"

#include <algorithm>
#include <glm/gtc/matrix_inverse.hpp>
#include <stdexcept>

#include "Graphics/AABB.h"
#include "Graphics/Object3D.h"
#include "Utils/DebugDraw.h"

PFRay::PFRay(
    const glm::vec2 ScreenSpacePixels,
    const glm::vec2 ViewportSize,
    const glm::mat4& ScreenProjectionMatrix,
    const glm::mat4& ViewMatrix,
    const glm::vec3& CameraPosition,
    const float InLength
)
    : Origin(CameraPosition), Length(InLength)
{
    if (ViewportSize.x <= 0.0f || ViewportSize.y <= 0.0f)
    {
        throw std::invalid_argument("Ray viewport size must be positive");
    }

    const glm::vec2 ndc(
        (2.0f * ScreenSpacePixels.x) / ViewportSize.x - 1.0f,
        1.0f - (2.0f * ScreenSpacePixels.y) / ViewportSize.y
    );

    const glm::vec4 clipPos(ndc.x, ndc.y, -1.0f, 1.0f);

    const glm::mat4 inverseViewProjection =
        glm::inverse(ScreenProjectionMatrix * ViewMatrix);

    glm::vec4 worldPos = inverseViewProjection * clipPos;
    worldPos /= worldPos.w;

    Direction = glm::normalize(glm::vec3(worldPos) - Origin);}

std::vector<PFRayHit> PFRay::Cast(std::vector<PFObject3D*> Models, bool bDebug) const
{
    auto end = Direction;
    end.x *= Length;
    end.y *= Length;
    end.z *= Length;
    if(bDebug)
    {
        DebugDraw::DrawLine(Origin, Origin + end);
    }

    BroadPhase(Models);
    return NarrowPhase(Models);
    
    // std::cout<< "Models Hit: " << (Models.size()>0) ? "Yes" : "No";
}

void PFRay::BroadPhase(std::vector<PFObject3D*>& Models) const
{
    for(int i = Models.size()-1; i > -1; i--)
    {
        if(!Intersect(Models[i]->GetBounds()))
        {
            Models.erase(Models.begin() + i);
        }
    }
}

std::vector<PFRayHit> PFRay::NarrowPhase(std::vector<PFObject3D*>& Models) const
{

    return std::vector<PFRayHit>();
}

bool PFRay::Intersect(const AABB& Box) const
{
    float TMin = 0;
    float TMax = FLT_MAX;
    glm::vec3 BoxMinExtent = Box.Min();
    glm::vec3 BoxMaxExtent = Box.Max();
    for (int i = 0; i< 3; i ++)
    {
        if(glm::abs(Direction[i]) < std::numeric_limits<float>::epsilon())
        {
            if(Origin[i] < BoxMinExtent[i] || Origin[i]>BoxMinExtent[i])
            {
                return false;
            }
        }

        float OOD = 1.0f / Direction[i]; 
        float t1 = (BoxMinExtent[i] - Origin[i]) * OOD;
        float t2 = (BoxMaxExtent[i] - Origin[i]) * OOD;

        if(t1 >  t2)
        {
            std::swap(t1, t2);
        }

        TMin = std::max(TMin, t1);
        TMax = std::min(TMax, t2);

        if(TMin>TMax)
        {
            return false;
        }
    }

    return true;
}
