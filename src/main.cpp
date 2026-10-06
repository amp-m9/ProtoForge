#include <algorithm>
#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <stb_image.h>
#include <format>

#include "Camera.h"
#include "filesystem.h"
#include "main.h"
#include "Model.h"
#include "Shader.h"
#include "Utils.h"

PFCamera* MainCam;
bool bMouseCaptured = false;

void ProcessInput(GLFWwindow* Window)
{
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Window, true);
    }
}

void ScrollCallback(GLFWwindow* Window, double InXOffSet, double InYOffSet)
{
    if (bMouseCaptured)
    {
        MainCam->HandleMouseScroll(InXOffSet, InYOffSet);
    }
}

void WindowResizeCallback(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height); // lower left corner followed by w*h
    if(MainCam)
    {
        MainCam->SetAspectRatio(width, height);
    }
}

bool LoadModel(std::string& ErrorOut)
{
    PFModel Model(FilePath);
    // if(Model.IsLoaded())
    {
        Models.push_back(Model);
        FilePath[0] = '\0';
        return true;

    }

    ErrorOut = std::format("Failed to load model at {}",  FilePath);
    return false;
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button != GLFW_MOUSE_BUTTON_RIGHT)
    {
        return;
    }

    if (action == GLFW_PRESS && !ImGui::GetIO().WantCaptureMouse)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        bMouseCaptured = true;
        MainCam->ResetMousePosition();
    }
    else if (action == GLFW_RELEASE && bMouseCaptured)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        bMouseCaptured = false;
    }
}

void DropCallback(GLFWwindow* window, int path_count, const char* paths[])
{
    if (path_count != 1 || paths == nullptr || paths[0] == nullptr)
    {
        return;
    }
    
    const size_t length = std::strlen(paths[0]);
    if (length >= sizeof(FilePath))
    {
        // Report that the path is too long.
        return;
    }

    std::memcpy(FilePath, paths[0], length + 1);
    std::string ErrorOut;
    if (!LoadModel(ErrorOut))
    {
    // set error out.
    }

}

int main()
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
        return -1;
    }

    glfwMakeContextCurrent(Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
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

    MainCam = new PFCamera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), FOV,
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);

    // rotate about x axis.
    float LastTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    GLbitfield ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    stbi_set_flip_vertically_on_load(false);
    PFShader ModelShader("assets/shaders/03_Models/model_loading");
    
    while (!glfwWindowShouldClose(Window))
    {
        DeltaTime = glfwGetTime() - LastTime;
        LastTime = glfwGetTime();
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(ClearMask);

        ProcessInput(Window);
        if (bMouseCaptured)
        {
            MainCam->ProcessInput(Window, DeltaTime);
        }

        RenderModels(Models, ModelShader);

        UpdateUI();

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwTerminate();
    return 0;
}

void RenderModels(std::vector<PFModel>& Models, PFShader& ModelShader)
{
    for(auto& InModel: Models)
    {
        InModel.Draw(*MainCam, ModelShader);
    }
}

void UpdateUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    const ImGuiViewport* Viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(Viewport->WorkPos);
    ImGui::SetNextWindowSize(ImVec2(300.0f, 200.0f));
    ImGui::Begin("LearnOpenGL", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Hello from Dear ImGui!");

    if (ImGui::InputTextWithHint("##Filter", "incl,-excl", FilePath, IM_ARRAYSIZE(FilePath), ImGuiInputTextFlags_EscapeClearsAll))
    {
        // su'in
    }
    ImGui::SameLine();
    if (ImGui::Button("Click me"))
    {
        // Do something
    }
    for(auto Model : Models)
    {
        ImGui::Text(Model.GetDisplayName());
    }
    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MouseCallback(GLFWwindow* Window, double InXPos, double InYPos)
{
    if (bMouseCaptured)
    {
        MainCam->HandleMouseMove(InXPos, InYPos, DeltaTime);
    }
}
