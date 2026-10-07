#include "Graphics/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>
#include "Utils/filesystem.h"

PFShader::PFShader(const char* vertexPath, const char* fragmentPath)
{
    std::string VertexCode, FragmentCode;
    bool bSuccessfulLoad = true;

    bSuccessfulLoad &= LoadShaderFileAtPath(FileSystem::getPath(vertexPath).c_str(), VertexCode);
    bSuccessfulLoad &= LoadShaderFileAtPath(FileSystem::getPath(fragmentPath).c_str(), FragmentCode);
    assert(bSuccessfulLoad);

    const char* VertexCodeCharArray = VertexCode.c_str();
    const char* FragmentCodeCharArray = FragmentCode.c_str();

    CreateAndSetShaderProgram(VertexCodeCharArray, FragmentCodeCharArray);
}

PFShader::PFShader(const char* shaderNameWithoutExtension)
{
    std::string vertexPath = std::string(shaderNameWithoutExtension) + ".vs";
    std::string fragmentPath = std::string(shaderNameWithoutExtension) + ".fs";

    std::string VertexCode, FragmentCode;
    bool bSuccessfulLoad = true;

    bSuccessfulLoad &= LoadShaderFileAtPath(FileSystem::getPath(vertexPath).c_str(), VertexCode);
    bSuccessfulLoad &= LoadShaderFileAtPath(FileSystem::getPath(fragmentPath).c_str(), FragmentCode);
    assert(bSuccessfulLoad);

    const char* VertexCodeCharArray = VertexCode.c_str();
    const char* FragmentCodeCharArray = FragmentCode.c_str();

    CreateAndSetShaderProgram(VertexCodeCharArray, FragmentCodeCharArray);
}

bool PFShader::LoadShaderFileAtPath(const char* Path, std::string& Code)
{
    std::ifstream ShaderFile;

    // ensure ifstream objects can throw exceptions:
    ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        ShaderFile.open(Path);
        std::stringstream ShaderStream;

        ShaderStream << ShaderFile.rdbuf();

        ShaderFile.close();

        Code = ShaderStream.str();
        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: file at" << Path << std::endl;
        return false;
    }
}

void PFShader::CreateAndSetShaderProgram(const char*& VertexCode, const char*& FragmentCode)
{
    unsigned int VertexShaderID, FragmentShaderID;
    int Success;
    char InfoLog[512];

    static const auto CreateShader = [&Success, &InfoLog](const char*& ShaderCode, const GLenum ShaderType, unsigned int& ShaderID)
    {
        ShaderID = glCreateShader(ShaderType);
        glShaderSource(ShaderID, 1, &ShaderCode, NULL);
        glCompileShader(ShaderID);

        glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Success);
        if (!Success)
        {
            glGetShaderInfoLog(ShaderID, 512, NULL, InfoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << InfoLog << std::endl;
        };
    };

    CreateShader(VertexCode, GL_VERTEX_SHADER, VertexShaderID);
    CreateShader(FragmentCode, GL_FRAGMENT_SHADER, FragmentShaderID);

    ID = glCreateProgram();
    glAttachShader(ID, VertexShaderID);
    glAttachShader(ID, FragmentShaderID);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &Success);
    if (!Success)
    {
        glGetProgramInfoLog(ID, 512, NULL, InfoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << InfoLog << std::endl;
    }

    assert(Success);

    glDeleteShader(FragmentShaderID);
    glDeleteShader(VertexShaderID);
}
