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

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

    Shader CMYKShader("assets/shaders/TransformShader/TransformShader.vs", "assets/shaders/TransformShader/TransformShader.fs");

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

    // Load texture
    int width, height, nrChannels;

    std::string fileLoc = FileSystem::getPath("assets/textures/awesomeface.png").c_str();
    stbi_set_flip_vertically_on_load(true);
    unsigned char* TextureData = stbi_load(fileLoc.c_str(), &width, &height, &nrChannels, 0);

    assert(TextureData);
    unsigned int FaceTextureID;
    glGenTextures(1, &FaceTextureID);
    glBindTexture(GL_TEXTURE_2D, FaceTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenBuffers(1, &ElementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the
    // vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CMYKShader.Use();
    CMYKShader.setInt("texture1", 0);

    glm::mat4 Transform = glm::mat4(1.0f);
    Transform = glm::rotate(Transform, glm::radians((float)glfwGetTime()), glm::vec3(0.0, 0.0, 1.0));
    Transform = glm::scale(Transform, glm::vec3(0.5, 0.5, 0.5));

    unsigned int TransformLocation = glGetUniformLocation(CMYKShader.ID, "transform");
    glUniformMatrix4fv(TransformLocation, 1, GL_FALSE, glm::value_ptr(Transform));

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        CMYKShader.Use();
        Transform = glm::rotate(Transform, glm::radians((float)(glfwGetTime())), glm::vec3(0.0, 0.0, 1.0));
        glUniformMatrix4fv(TransformLocation, 1, GL_FALSE, glm::value_ptr(Transform));

        glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, CrateTextureID);
        // glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, FaceTextureID);

        glBindVertexArray(VertexArrayObject);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VertexArrayObject);
    glDeleteBuffers(1, &VertexBufferObject);
    glDeleteBuffers(1, &ElementBufferObject);

    glfwTerminate();
    return 0;
}
