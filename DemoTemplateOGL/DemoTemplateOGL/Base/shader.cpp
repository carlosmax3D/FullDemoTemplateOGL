#include "Utilities.h"
#include "shader.h"
#ifdef ENGINE_DIRECTX
#include "shaderDX11.h"
#else
#include "shaderGL33.h"
#endif

Shader::~Shader(){
}

Shader* Shader::createShader(const char* vertexPath, const char* fragPixPath, const char* geometryPath) {
    Shader* shader = NULL;
#ifdef ENGINE_DIRECTX
    shader = (Shader*)new ShaderDX11(vertexPath, fragPixPath, geometryPath);
#else
    shader = (Shader*)new ShaderGL33(vertexPath, fragPixPath, geometryPath);
#endif
    return shader;
}