#include "Utilities.h"
#if !defined(SHADERDX11_H) && defined(ENGINE_DIRECTX)
#define SHADERDX11_H

#include "shader.h"
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "material.h"

struct CB_PerObject {
    // ===== MATRICES (VS) =====
    DirectX::XMFLOAT4X4 model;       // 64 bytes
    DirectX::XMFLOAT4X4 view;        // 64 bytes
    DirectX::XMFLOAT4X4 projection;  // 64 bytes

    // ===== MATERIAL (PS) =====
    DirectX::XMFLOAT3 ambient = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);   float pad0;
    DirectX::XMFLOAT3 diffuse = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);   float time;
    DirectX::XMFLOAT3 specular = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);  float shininess = 1.0f;

    // ===== LIGHT (PS) =====
    DirectX::XMFLOAT3 lightPos;      float width;
    DirectX::XMFLOAT3 lightAmbient;  float height;
    DirectX::XMFLOAT3 lightDiffuse;  float blendFactor;
    DirectX::XMFLOAT3 lightSpecular; float pad1;
    DirectX::XMFLOAT3 lightDirection; float pad2;

    // ===== CAMERA / FLAGS (VS + PS) =====
    DirectX::XMFLOAT3 cameraPos;  int textureSample;
    DirectX::XMFLOAT3 viewPos;    int multipleInstances;
    DirectX::XMFLOAT3 billboardPos; int texture_diffuse1;
    DirectX::XMFLOAT3 camRight;    int useBlending;
    DirectX::XMFLOAT3 color;       int useDynamicLighting;
};

struct CB_Bones {
    DirectX::XMFLOAT4X4 finalBonesMatrices[MAX_MODEL_BONES];
};

class ShaderDX11 : public Shader {
public:
    ID3D11VertexShader* vertexShader = NULL;
    ID3D11PixelShader* pixelShader = NULL;
    ID3D11GeometryShader* geometryShader = NULL;
    ID3D11InputLayout* layout = NULL;
    ID3D11Buffer* constantBuffer = NULL;
    ID3D11Buffer* constantBuffer1 = NULL;
    CB_PerObject cpuData;
    CB_Bones boneData;

    ~ShaderDX11();
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    ShaderDX11(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    void desuse();
    // activate the shader
    // ------------------------------------------------------------------------
    void use();
    void apply();
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
    ID3DBlob* compileShader(std::string& filename, const char* entryPoint, const char* shaderModel, std::string type);
    void checkCompileErrors(HRESULT hr, ID3DBlob* errorBlob, std::string& type);
};
#endif