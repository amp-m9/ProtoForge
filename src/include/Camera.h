#pragma once

#include <algorithm>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
  public:
    Camera(glm::vec3 Position, glm::vec3 CamUp, glm::vec3 CamForward, float FOV, float AspectRatio, float NearPlane, float FarPlane)
        : Position(Position), CamUp(CamUp), CamForward(CamForward), FOV(FOV), AspectRatio(AspectRatio), NearPlane(NearPlane), FarPlane(FarPlane) {};

    const glm::mat4& GetViewMatrix() const { return ViewMatrix; }

    // Returns perspective projection matrix using the camera's FOV, aspect ratio, near plane and far plane.
    const glm::mat4& GetProjectionMatrix() const { return ProjectionMatrix; }
    const glm::vec3& GetPosition() const { return Position; }
    const glm::vec3& GetForward() const { return CamForward; }
    const glm::vec3& GetUp() const { return CamUp; }
    
    void ProcessInput(GLFWwindow* Window, float DeltaTime)
    {
        const float CameraSpeed = 10.f * DeltaTime;
        glm::vec3 MovementDirection = glm::vec3(0.0f);
        if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
        {
            MovementDirection += CamForward;
        }
        if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
        {
            MovementDirection -= CamForward;
        }
        if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
        {
            MovementDirection -= glm::normalize(glm::cross(CamForward, CamUp));
        }
        if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
        {
            MovementDirection += glm::normalize(glm::cross(CamForward, CamUp));
        }
        if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            MovementDirection -= GlobalUp;
        }
        if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
        {
            MovementDirection += GlobalUp;
        }

        if (MovementDirection != glm::vec3(0.0f))
        {
            Position += glm::normalize(MovementDirection) * CameraSpeed;
            UpdateCameraMatrices();
        }
    }
    void HandleMouseMove(double InXPos, double InYPos, float DeltaTime)
    {
        if (!bMouseInit)
        {
            LastX = InXPos;
            LastY = InYPos;
            bMouseInit = true;
        }

        float XOffSet = InXPos - LastX;
        float YOffset = InYPos - LastY;

        LastX = InXPos;
        LastY = InYPos;

        XOffSet *= Sensitivity * DeltaTime;
        YOffset *= Sensitivity * DeltaTime;

        Yaw += XOffSet;
        Pitch -= YOffset;

        Pitch = std::clamp(Pitch, -89.0f, 89.0f);

        UpdateCameraMatrices();
    };

    void HandleMouseScroll(double InXOffSet, double InYOffSet)
    {
        FOV -= (float)InYOffSet;
        FOV = std::clamp(FOV, 1.0f, 60.f);

        UpdateCameraMatrices();
    }

    void UpdateCameraMatrices()
    {
        ViewMatrix = glm::lookAt(Position, Position + CamForward, CamUp);

        glm::vec3 Direction;
        Direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Direction.y = sin(glm::radians(Pitch));
        Direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

        CamForward = glm::normalize(Direction);
        CamRight = glm::normalize(glm::cross(CamForward, glm::vec3(0.0f, 1.0f, 0.0f)));
        CamUp = glm::normalize(glm::cross(CamRight, CamForward));

        ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, NearPlane, FarPlane);
    }

  private:
    float Sensitivity = 1.f;
    float Yaw = 0.0f;
    ;
    float Pitch = 0.0f;
    float LastX = 0.0f;
    float LastY = 0.0f;

    glm::vec3 Position;
    glm::vec3 CamUp, CamForward, CamRight;
    glm::mat4 ProjectionMatrix, ViewMatrix;

    const glm::vec3 GlobalUp = glm::vec3(0.f, 1.0f, 0.f);
    const glm::vec3 GlobalForward{0, 1, 0};
    float FOV;
    float AspectRatio;
    float NearPlane, FarPlane;
    bool bMouseInit = false;
};