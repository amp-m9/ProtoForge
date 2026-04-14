#pragma once
#include <glm/glm.hpp>

struct Light 
{
    virtual ~Light() = default;
public:
    glm::vec3 Ambient{0.2f, 0.2f, 0.2f};
    glm::vec3 Diffuse{0.5f, 0.5f, 0.5f};
    glm::vec3 Specular{1.f};
};

struct DirectionalLight : public Light
{
    glm::vec3 Direction {-1.0f, 0.0f, 0.0f};
};

struct PointLight : public Light
{
public:
    glm::vec3 Position {0.f};
    float Constant = 1.0f;
    float Linear = 0.09f;
    float Quadratic = 0.032f;
};

struct SpotLight : public PointLight, public DirectionalLight
{
    float InnerCutOff = glm::cos(glm::radians(12.5f));
    float OuterCutOff = glm::cos(glm::radians(17.5f));
};