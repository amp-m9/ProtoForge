#include "main.h"

#include <algorithm>
#include <assert.h>
#include <format>
#include <iostream>

#include "Camera.h"
#include "Shader.h"
#include "Utils.h"

int main()
{
    auto* Window = InitOpenGLAndUI(); 
    if(!Window)
    {
        return -1;
    }

    MainCam = new PFCamera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        45.0f,
        (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f
    );

    float LastTime = glfwGetTime();
    PFShader ModelShader("assets/shaders/03_Models/model_loading");
    Models.push_back(PFModel("assets/models/barbarian.glb"));
    Models[0].SetScale(glm::vec3(30.0));
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

    EndUIAndOpenGL();
    return 0;
}

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

void RenderModels(std::vector<PFModel>& Models, PFShader& ModelShader)
{
    float Time = glfwGetTime();

    for(auto& InModel: Models)
    {
        float x = sin(Time);
        float y = cos(Time);
        glm::vec3 Position(x, y, 0);
        Position *= 3;
        
        InModel.SetPosition(Position);
        InModel.SetRotation(0, y, 0);
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
