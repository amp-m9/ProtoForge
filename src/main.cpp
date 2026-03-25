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
#include "Camera.h"
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

    Shader Shader_Going3D("assets/shaders/Coordinates_01/going3D.vs", "assets/shaders/Coordinates_01/going3D.fs");
    constexpr size_t arrLen = ArrayLength(CubeVertices);
    float vertices[arrLen];
    for (size_t i = 0; i < arrLen; ++i)
    {
        vertices[i] = CubeVertices[i];
    }

    glm::vec3 cubePositions[] = {glm::vec3(0.0f, 0.0f, 0.0f),     glm::vec3(2.0f, 5.0f, -15.0f), glm::vec3(-1.5f, -2.2f, -2.5f),
                                 glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
                                 glm::vec3(1.3f, -2.0f, -2.5f),   glm::vec3(1.5f, 2.0f, -2.5f),  glm::vec3(1.5f, 0.2f, -1.5f),
                                 glm::vec3(-1.3f, 1.0f, -1.5f)};

    unsigned int indices[] = {0, 1, 2, 1, 2, 3};

    unsigned int VertexBufferObject, VertexArrayObject, ElementBufferObject;
    glGenVertexArrays(1, &VertexArrayObject);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VertexArrayObject);

    glGenBuffers(1, &VertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load textures
    unsigned int FaceTextureID, CrateTextureID;
    LoadTexture("assets/textures/wooden_container.jpg", CrateTextureID, false, true);
    LoadTexture("assets/textures/awesomeface.png", FaceTextureID, true);

    glGenBuffers(1, &ElementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the
    // vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader_Going3D.use();
    Shader_Going3D.setInt("texture1", 0);
    Shader_Going3D.setInt("texture2", 1);
    Shader_Going3D.setFloat("blend", 0.2f);
    
    MainCam = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), FOV, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);
    // rotate about x axis.

    Shader_Going3D.setMat4("projection", MainCam->GetProjectionMatrix());
    Shader_Going3D.setMat4("view", MainCam->GetViewMatrix());

    float LastTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);

    // Set Active Textures for draw calls, usually done in loop if plannig to change em (i think?)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, CrateTextureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, FaceTextureID);

    GLbitfield ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(Window, MouseCallback);
    glfwSetScrollCallback(Window, ScrollCallback);

    while (!glfwWindowShouldClose(Window))
    {
        DeltaTime = glfwGetTime() - LastTime;
        LastTime = glfwGetTime();

        ProcessInput(Window);
        MainCam->ProcessInput(Window, DeltaTime);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(ClearMask);

        Shader_Going3D.use();
        // CameraRight = glm::normalize(glm::cross(CameraFront, CameraUp));

        Shader_Going3D.setMat4("projection", MainCam->GetProjectionMatrix());
        Shader_Going3D.setMat4("view", MainCam->GetViewMatrix());

        glBindVertexArray(VertexArrayObject);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = glfwGetTime() * (i + 1);
            if (i % 3 == 0)
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            Shader_Going3D.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VertexArrayObject);
    glDeleteBuffers(1, &VertexBufferObject);
    glDeleteBuffers(1, &ElementBufferObject);

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
