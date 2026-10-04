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

#include "Camera.h"
#include "filesystem.h"
#include "Helpers.h"
#include "main.h"
#include "Model.h"
#include "Shader.h"
#include "Utils.h"

Camera* MainCam;
bool bMouseCaptured = true;

void FramebufferSizeCallback(GLFWwindow* Window, int width, int height)
{
    glViewport(0, 0, width, height); // lower left corner followed by w*h
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

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !bMouseCaptured)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        bMouseCaptured = true;
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

    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");



    MainCam = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), FOV,
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);

    // rotate about x axis.
    float LastTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Use GL_FILL to restore solid drawing

    GLbitfield ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(Window, MouseCallback);
    glfwSetScrollCallback(Window, ScrollCallback);
    glfwSetMouseButtonCallback(Window, MouseButtonCallback);

    stbi_set_flip_vertically_on_load(false);
    Shader ModelShader("assets/shaders/03_Models/model_loading");
    
    // Model Cube = Model::Utils::Cube();
    std::vector<Model> Models = {
        Model(FileSystem::getPath("assets/models/barbarian.glb").c_str())
    };
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

void RenderModels(std::vector<Model>& Models, Shader& ModelShader)
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

    ImGui::Begin("LearnOpenGL");

    ImGui::Text("Hello from Dear ImGui!");

    if (ImGui::Button("Click me"))
    {
        // Do something
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
