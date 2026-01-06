#ifndef SHADER_H
#define SHADER_H

#include "Utilities.h"
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "material.h"

class Shader {
public:
    unsigned int ID;
    static Shader* createShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

    virtual void desuse() = 0;    // deactivate the shader
    // ------------------------------------------------------------------------
    virtual void use() = 0;
    // utility uniform functions
    // ------------------------------------------------------------------------
    virtual void setBool(const char* name, bool value) = 0;
    // ------------------------------------------------------------------------
    virtual void setInt(const char* name, int value) = 0;
    // ------------------------------------------------------------------------
    virtual void setFloat(const char* name, float value) = 0;
    // ------------------------------------------------------------------------
    virtual void setVec2(const char* name, const glm::vec2& value) = 0;

    virtual void setVec2(const char* name, float x, float y) = 0;
    // ------------------------------------------------------------------------
    virtual void setVec3(const char* name, const glm::vec3& value) = 0;
    virtual void setVec3(const char* name, float x, float y, float z) = 0;
    // ------------------------------------------------------------------------
    virtual void setVec4(const char* name, const glm::vec4& value) = 0;
    virtual void setVec4(const char* name, float x, float y, float z, float w) = 0;
    // ------------------------------------------------------------------------
    virtual void setMat2(const char* name, const glm::mat2& mat) = 0;
    // ------------------------------------------------------------------------
    virtual void setMat3(const char* name, const glm::mat3& mat) = 0;
    // ------------------------------------------------------------------------
    virtual void setMat4(const char* name, const glm::mat4& mat) = 0;
    virtual void setMat4Array(const char* name, const glm::mat4* matrices, size_t count) = 0;

    virtual ~Shader();
private:
};
#endif