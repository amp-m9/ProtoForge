#include <algorithm>
#include <assert.h>
#include "filesystem.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stb_image.h>
#include "Shader.h"
#include "Utils.h"
#include "main.h"
#include "Model.h"
#include "Camera.h"
#include "Helpers.h"
Camera* MainCam;

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
    MainCam->HandleMouseScroll(InXOffSet, InYOffSet);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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

    Shader LitObjectShader("assets/shaders/01_Lighting/00_Colours/colours.vs", "assets/shaders/01_Lighting/00_Colours/colours.fs");
    Shader WhiteLightShader("assets/shaders/01_Lighting/00_Colours/colours.vs", "assets/shaders/01_Lighting/00_Colours/whiteLight.fs");

    glm::vec3 cubePosition = glm::vec3(0.0f, 0.0f, 0.0f);

    unsigned int indices[] = {0, 1, 2, 1, 2, 3};

    VAOData BoxVAOData;
    BoxVAOData.Stride = 8 * sizeof(float);

    VertexAttribute<float> posAttr, normalAttribute, textureAttribute;
    posAttr.CountAtPosition = 3;
    posAttr.NormalizedAtPosition = false;
    posAttr.Position = 0;
    posAttr.Offset = 0;

    normalAttribute = posAttr;
    normalAttribute.Position = 1; 
    normalAttribute.Offset = posAttr.GetSize();

    textureAttribute = posAttr;
    textureAttribute.Position = 2;  
    textureAttribute.Offset = posAttr.GetSize() + normalAttribute.GetSize();

    BoxVAOData.Attributes.emplace_back(posAttr);
    BoxVAOData.Attributes.emplace_back(normalAttribute);
    BoxVAOData.Attributes.emplace_back(textureAttribute);

    Model BoxModel = Model(DefaultModels::TexturedCube, sizeof(DefaultModels::TexturedCube), BoxVAOData, &LitObjectShader);
    BoxModel.SetShaderUniform<glm::vec3>("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    Material* BoxMaterial = new Material();
    BoxModel.SetMaterial(BoxMaterial);
    BoxModel.LoadTexture("assets/textures/woodAndSteelContainer_diffuse.png", true, false, TextureType::Diffuse);
    BoxModel.LoadTexture("assets/textures/woodAndSteelContainer_specular.png", true, false, TextureType::Specular);

    Model LightModel = Model(DefaultModels::TexturedCube, sizeof(DefaultModels::TexturedCube), BoxVAOData, &WhiteLightShader);
    LightModel.SetPosition(1.f, 1.f, -2.0f);
    LightModel.SetScale(.2f, .2f, .2f);

    Light* LightPtr = new Light();
    LightPtr->Position = LightModel.GetPosition();

    MainCam = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), FOV,
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);

    // rotate about x axis.
    float LastTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);

    GLbitfield ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(Window, MouseCallback);
    glfwSetScrollCallback(Window, ScrollCallback);
    glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(Window))
    {
        DeltaTime = glfwGetTime() - LastTime;
        LastTime = glfwGetTime();

        ProcessInput(Window);
        MainCam->ProcessInput(Window, DeltaTime);
        BoxModel.SetShaderUniform<glm::vec3>("viewPos", MainCam->GetPosition());

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(ClearMask);

        BoxModel.Draw(MainCam, LightPtr);
        LightModel.Draw(MainCam);

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    BoxModel.FreeAll();

    glfwTerminate();
    return 0;
}

bool LoadTexture(const char* fileLocation, unsigned int& TextureID, const bool bUseAlpha, const bool bFlipVerticallyOnLoad)
{
    std::string path = FileSystem::getPath(fileLocation).c_str();
    stbi_set_flip_vertically_on_load(bFlipVerticallyOnLoad);

    int width, height, nrChannels;
    unsigned char* TextureData = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (!TextureData)
    {
        return false;
    }

    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // actually loads the texture data

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, bUseAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, TextureData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(TextureData);
    return true;
}

void MouseCallback(GLFWwindow* Window, double InXPos, double InYPos)
{
    MainCam->HandleMouseMove(InXPos, InYPos, DeltaTime);
}
