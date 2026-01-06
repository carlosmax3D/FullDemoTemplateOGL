#include "Utilities.h"
#if !defined(SHADERGL33_H) && !defined(ENGINE_DIRECTX)
#define SHADERGL33_H

#include "shader.h"
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "material.h"

class ShaderGL33 : public Shader {
public:
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    ShaderGL33(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    void desuse();
    // activate the shader
    // ------------------------------------------------------------------------
    void use();
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const char* name, bool value);
    // ------------------------------------------------------------------------
    void setInt(const char* name, int value);
    // ------------------------------------------------------------------------
    void setFloat(const char* name, float value);
    // ------------------------------------------------------------------------
    void setVec2(const char* name, const glm::vec2& value);
    void setVec2(const char* name, float x, float y);
    // ------------------------------------------------------------------------
    void setVec3(const char* name, const glm::vec3& value);
    void setVec3(const char* name, float x, float y, float z);
    // ------------------------------------------------------------------------
    void setVec4(const char* name, const glm::vec4& value);
    void setVec4(const char* name, float x, float y, float z, float w);
    // ------------------------------------------------------------------------
    void setMat2(const char* name, const glm::mat2& mat);
    // ------------------------------------------------------------------------
    void setMat3(const char* name, const glm::mat3& mat);
    // ------------------------------------------------------------------------
    void setMat4(const char* name, const glm::mat4& mat);
    void setMat4Array(const char* name, const glm::mat4* matrices, size_t count);

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type);
};
#endif