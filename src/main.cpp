#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stb_image.h>
#include <assert.h>
#include "Utils.h"
#include "Shader.h"
#include "filesystem.h"
#include "main.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); // lower left corner followed by w*h
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader Shader_Going3D("assets/shaders/Coordinates_01/going3D.vs", "assets/shaders/Coordinates_01/going3D.fs");

    float vertices[] = {
        // Positions         // Texture coords
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
    };

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
    Shader_Going3D.setFloat("blend", 0.2);

    // rotate about x axis.
    glm::mat4 Model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 View = glm::mat4(1.0f);
    glm::mat4 Projection = glm::mat4(1.0f);
    Model = glm::rotate(Model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    View = glm::translate(View, glm::vec3(0.0f, 0.0f, -3.0f));
    Projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glm::mat4 Transform1 = glm::mat4(1.0f);
    Transform1 = glm::rotate(Transform1, glm::radians((float)glfwGetTime()), glm::vec3(0.0, 0.0, 1.0));
    Transform1 = glm::scale(Transform1, glm::vec3(0.5, 0.5, 0.5));

    Shader_Going3D.setMat4("projection", Projection);
    Shader_Going3D.setMat4("model", Model);
    Shader_Going3D.setMat4("view", View);

    float Delta = 0.f;
    float LastTime = glfwGetTime();

    // Set Active Textures for draw calls, usually done in loop if plannig to change em (i think?)

    while (!glfwWindowShouldClose(window))
    {
        Delta = glfwGetTime() - LastTime;
        LastTime = glfwGetTime();

        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, CrateTextureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, FaceTextureID);

        Shader_Going3D.use();

        glBindVertexArray(VertexArrayObject);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
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