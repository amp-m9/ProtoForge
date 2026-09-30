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
#include "DefaultModel.h"
#include "Model.h"
#include "Camera.h"
#include "Helpers.h"
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

    // if((glfwGetKey(Window, GLFW_KEY_F1) & glfwGetKey(Window, GLFW_KEY_F1)  == GLFW_PRESS) && bMouseCaptured)
    // {
    //     glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //     bMouseCaptured = false;
    // }
}

void ScrollCallback(GLFWwindow* Window, double InXOffSet, double InYOffSet)
{
    if(bMouseCaptured)
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


    VAOData LightVAOData;
    LightVAOData.Stride = 8 * sizeof(float);

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

    LightVAOData.Attributes.emplace_back(posAttr);
    LightVAOData.Attributes.emplace_back(normalAttribute);
    LightVAOData.Attributes.emplace_back(textureAttribute);

   
    DefaultModel LightModel = DefaultModel(DefaultModels::TexturedCube, sizeof(DefaultModels::TexturedCube), LightVAOData, &WhiteLightShader);
    LightModel.SetPosition(1.f, 1.f, -2.0f);
    LightModel.SetScale(.2f, .2f, .2f);

    DirectionalLight* DirLight = new DirectionalLight();
    DirLight->Direction = glm::vec3(-0.2f, -1.0f, -0.3f);

    glm::vec3 pointLightPositions[] = {
    	glm::vec3( 0.7f,  0.2f,  2.0f),
	    glm::vec3( 2.3f, -3.3f, -4.0f),
	    glm::vec3(-4.0f,  2.0f, -12.0f),
	    glm::vec3( 0.0f,  0.0f, -3.0f)
    };  

    glm::vec3 pointLightColours[] = {
    	glm::vec3( 0.f,  0.f,  1.0f),
	    glm::vec3( 0.0f, 1.0f, 0.0f),
	    glm::vec3(1.0f,  .0f, .0f),
	    glm::vec3( 1.0f)
    };  

    std::vector<PointLight*> PointLights;
    PointLights.resize(4);

    for(int i = 0; i<4; i ++)
    {
        PointLights[i] = new PointLight();
        PointLights[i]->Position = pointLightPositions[i];
        PointLights[i]->Ambient = pointLightColours[i];
        PointLights[i]->Diffuse = pointLightColours[i];
    }
    
    MainCam = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), FOV,
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);

    // rotate about x axis.
    float LastTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);

    GLbitfield ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(Window, MouseCallback);
    glfwSetScrollCallback(Window, ScrollCallback);
    glfwSetMouseButtonCallback(Window, MouseButtonCallback);
    glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    
    const glm::vec3 cubePositions[] = 
    {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };


    stbi_set_flip_vertically_on_load(true);
    Shader ModelShader("assets/shaders/03_Models/model_loading");
    Model BackPackModel(FileSystem::getPath("assets/models/backpack/backpack.obj").c_str());
    
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

        for(unsigned int i = 0; i<4; ++i)
        {
            LightModel.SetPosition(pointLightPositions[i]);
            LightModel.SetShaderUniform<glm::vec3>("lightColor",pointLightColours[i]);
            LightModel.Draw(MainCam, nullptr, {});
        }

        BackPackModel.Draw(*MainCam, ModelShader);
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }


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
    if(bMouseCaptured)
    {
        MainCam->HandleMouseMove(InXPos, InYPos, DeltaTime);
    }
}
