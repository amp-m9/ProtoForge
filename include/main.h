#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <vector>

#include "Graphics/Model.h"

class PFShader;
class PFCamera;

// time between current frame and last frame
float DeltaTime = 0.0f;
PFCamera* MainCam;
std::vector<PFModel> Models = {};
static char FilePath[1024] = "";
bool bMouseCaptured = false;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
GLbitfield ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

bool LoadTexture(const char* fileLocation, unsigned int& TextureID, const bool bUseAlpha = false, const bool bFlipVerticallyOnLoad = false);
void MouseCallback(GLFWwindow* Window, double xpos, double ypos);
void UpdateUI();
void RenderModels(std::vector<PFModel>& Models, PFShader& ModelShader);
void ProcessInput(GLFWwindow* Window);
void ScrollCallback(GLFWwindow* Window, double InXOffSet, double InYOffSet);
void WindowResizeCallback(GLFWwindow* window, int width, int height);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void DropCallback(GLFWwindow* window, int path_count, const char* paths[]);
bool LoadModel(std::string& ErrorOut);


GLFWwindow* InitOpenGLAndUI()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* Window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (Window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    glViewport(0, 0, 800, 600);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    glfwSetCursorPosCallback(Window, MouseCallback);
    glfwSetScrollCallback(Window, ScrollCallback);
    glfwSetMouseButtonCallback(Window, MouseButtonCallback);
    glfwSetDropCallback(Window, DropCallback);
    glfwSetFramebufferSizeCallback(Window, WindowResizeCallback);

    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    glEnable(GL_DEPTH_TEST);
    
    return Window;
}

void EndUIAndOpenGL()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}