#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
  private:
#define LOCATION(name) glGetUniformLocation(ID, name.c_str())
  public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);

    void use() { glUseProgram(ID); }; //  activates/set the shader as active.
    // utility uniform functions

    void setBool(const std::string& name, bool value) const { glUniform1i(LOCATION(name), (int)value); }

    void setInt(const std::string& name, int value) const { glUniform1i(LOCATION(name), value); }

    void setFloat(const std::string& name, float value) const { glUniform1f(LOCATION(name), value); }

    void setVec2(const std::string& name, const glm::vec2& value) const { glUniform2fv(LOCATION(name), 1, &value[0]); }
    void setVec2(const std::string& name, float x, float y) const { glUniform2f(LOCATION(name), x, y); }

    void setVec3(const std::string& name, const glm::vec3& value) const { glUniform3fv(LOCATION(name), 1, &value[0]); }
    void setVec3(const std::string& name, float x, float y, float z) const { glUniform3f(LOCATION(name), x, y, z); }

    void setVec4(const std::string& name, const glm::vec4& value) const { glUniform4fv(LOCATION(name), 1, &value[0]); }
    void setVec4(const std::string& name, float x, float y, float z, float w) const { glUniform4f(LOCATION(name), x, y, z, w); }

    void setMat2(const std::string& name, const glm::mat2& mat) const { glUniformMatrix2fv(LOCATION(name), 1, GL_FALSE, &mat[0][0]); }

    void setMat3(const std::string& name, const glm::mat3& mat) const { glUniformMatrix3fv(LOCATION(name), 1, GL_FALSE, &mat[0][0]); }

    void setMat4(const std::string& name, const glm::mat4& mat) const { glUniformMatrix4fv(LOCATION(name), 1, GL_FALSE, &mat[0][0]); }

  private:
    bool LoadShaderFileAtPath(const char* Path, std::string& Code);
    void CreateAndSetShaderProgram(const char*& VertexCode, const char*& FragmentCode);
};
