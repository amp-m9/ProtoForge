#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>

class Shader
{
  public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);

    void Use() { glUseProgram(ID); }; //  activates/set the shader as active.

    void setBool(const std::string& name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); }
    void setInt(const std::string& name, int value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
    void setFloat(const std::string& name, float value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), value); }

  private:
    bool LoadShaderFileAtPath(const char* Path, std::string& Code);
    void CreateAndSetShaderProgram(const char*& VertexCode, const char*& FragmentCode);
};
#endif