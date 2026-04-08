#pragma once
#include <glm/glm.hpp>

enum class LightType
{
    PointLight,
    DirectionalLight,
    SpotLight
};

struct Light 
{
    LightType Type = LightType::SpotLight;
    glm::vec3 Direction {-1.0f, 0.0f, 0.0f};
    glm::vec3 Position {0.f};

    glm::vec3 Ambient{0.2f, 0.2f, 0.2f};
    glm::vec3 Diffuse{0.5f, 0.5f, 0.5f};
    glm::vec3 Specular{1.f};
    
    float Constant = 1.0f;
    float Linear = 0.09f;
    float Quadratic = 0.032f;
    float CutOff = glm::cos(glm::radians(12.5f));

    glm::vec4 GetPosition()
    {
        return glm::vec4(Position, Type == LightType::PointLight ? 1.f:0 );
    }
    float GetCutOff() {return Type != LightType::SpotLight ? glm::cos(glm::radians(360.0f)) : CutOff;};
};
