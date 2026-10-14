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
    const float InLength,
    bool bInDrawDebug
)
    : Origin(CameraPosition), Length(InLength), bDrawDebug(bInDrawDebug)
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

    Direction = glm::normalize(glm::vec3(worldPos) - Origin);
    End = Origin + (Direction * Length);
}

std::vector<PFRayHit> PFRay::Cast(std::vector<PFObject3D*> Models) const
{
    auto end = Direction;
    end.x *= Length;
    end.y *= Length;
    end.z *= Length;
    if(bDrawDebug)
    {
        DebugDraw::DrawLine(Origin, Origin + end, glm::vec3(1.0, 0, 0), 0, true);
    }

    BroadPhase(Models);
    return NarrowPhase(Models);
    
    // std::cout<< "Models Hit: " << (Models.size()>0) ? "Yes" : "No";
}

void PFRay::BroadPhase(std::vector<PFObject3D*>& Models) const
{
    for(int i = Models.size()-1; i > -1; i--)
    {
        if(!Models[i] || !Intersect(Models[i]->GetBounds()))
        {
            Models.erase(Models.begin() + i);
        }
    }
}

std::vector<PFRayHit> PFRay::NarrowPhase(std::vector<PFObject3D*>& Models) const
{
    std::vector<PFRayHit> Hits;
    std::vector<glm::uint> Indices;
    std::vector<glm::vec3> Vertices;
    PFRayHit Hit;

    for(PFObject3D* Model : Models)
    {
        if(!Model)
        {
            continue;
        }

        if(Intersect(*Model, Hit))
        {
            Hits.push_back(Hit);
            if(bDrawDebug)
            {
                DebugDraw::DrawPoint(Hit.HitLocation);
            }
        }
    }

    if (!Hits.empty())
    {
        const auto Furthest = std::max_element(
            Hits.begin(), Hits.end(),
            [](const PFRayHit& a, const PFRayHit& b)
            {
                return a.Dist > b.Dist;
            }
        );
    }

    return Hits;
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
            if(Origin[i] < BoxMinExtent[i] || Origin[i]>BoxMaxExtent[i])
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

bool PFRay::Intersect(PFObject3D& Object, PFRayHit& Hit) const
{
    std::vector<glm::uint> Indices = Object.GetIndices();
    std::vector<glm::vec3> Vertices = Object.GetVerticesInWorldSpace();
    
    const auto MaxIndex = std::max_element(Indices.begin(), Indices.end());
    if(MaxIndex == Indices.end())
    {
        return false;
    }
    
    if(Indices.size() % 3 != 0 || *MaxIndex >= Vertices.size())
    {
        return false;
    }

    const glm::vec3 Ray = Direction * Length;
    const glm::vec3 O = Origin;
    glm::vec3 A , B , C , 
              OA, OB, OC, 
              M;

    float U, V, W;

    Hit.Dist = FLT_MAX;

    for(int i = 0; i<Indices.size(); i+=3)
    {
        A = Vertices[Indices[i]];
        B = Vertices[Indices[i + 1]];
        C = Vertices[Indices[i + 2]];

        OA = A - O;
        OB = B - O;
        OC = C - O;

        M = glm::cross(Ray, OC);
        U = glm::dot(OB, M);
        if(U < 0.0)
        {
            continue;
        }

        V = -glm::dot(OA, M);
        if(V < 0.0f)
        {
            continue;
        }

        W = glm::dot(glm::cross(Ray, OB), OA);
        if(W < 0.0f)
        {
            continue;
        }

        const float Denom = U + V + W;
        if (Denom <= 0.0f)
        {
            continue;
        }

        const float InvDenom = 1.0f / Denom;
        U *= InvDenom;
        V *= InvDenom;
        W *= InvDenom;

        glm::vec3 Intersection = U*A + V*B + W*C;
        float IntersectionDistance = glm::length(Intersection - Origin);

        if(IntersectionDistance < Hit.Dist)
        {
            Hit.HitLocation = Intersection;
            Hit.Object = &Object;
            Hit.Dist = IntersectionDistance;
        }
    }

    return Hit.Dist != FLT_MAX;
}
