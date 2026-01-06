#include "Utilities.h"
#include "shaderDX11.h"
#if defined(SHADERDX11_H) && defined(ENGINE_DIRECTX)
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "material.h"

ShaderDX11::~ShaderDX11() {
    if (constantBuffer) constantBuffer->Release();
    if (constantBuffer1) constantBuffer1->Release();
    if (layout) layout->Release();
    vertexShader->Release();
    pixelShader->Release();
    if (geometryShader) geometryShader->Release();
}

ShaderDX11::ShaderDX11(const char* vertexP, const char* fragmentP, const char* geometryP) {
    D3D11_INPUT_ELEMENT_DESC layout[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(Vertex, TexCoords), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Normal),   D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Tangent),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Bitangent),D3D11_INPUT_PER_VERTEX_DATA, 0 },

    { "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, offsetof(Vertex, m_BoneIDs), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BLENDWEIGHT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, offsetof(Vertex, m_Weights), D3D11_INPUT_PER_VERTEX_DATA, 0 },

    // INSTANCE DATA (slot 1)
    { "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof(ModelAttributes, translate), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "TEXCOORD", 2, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof(ModelAttributes, scale),     D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "TEXCOORD", 3, DXGI_FORMAT_R32_FLOAT,       1, offsetof(ModelAttributes, rotX),      D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "TEXCOORD", 4, DXGI_FORMAT_R32_FLOAT,       1, offsetof(ModelAttributes, rotY),      D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "TEXCOORD", 5, DXGI_FORMAT_R32_FLOAT,       1, offsetof(ModelAttributes, rotZ),      D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "TEXCOORD", 6, DXGI_FORMAT_R32_FLOAT,       1, offsetof(ModelAttributes, active),    D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexPath(vertexP), fragmentPath(fragmentP), geometryPath(geometryP == NULL ? "\0" : geometryP);
    vertexPath.append("dx");
    fragmentPath.append("dx");
    geometryPath.append("dx");

    try {
        ID3DBlob* gsBlob = NULL;
        ID3DBlob* vsBlob = compileShader(
            vertexPath,   // archivo
            "main",         // entry point
            "vs_5_0",          // shader model
            "VERTEX");

        vertexShader = nullptr;
        device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
        
        device->CreateInputLayout(layout, _countof(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &this->layout);

        ID3DBlob* fsBlob = compileShader(
            fragmentPath,   // archivo
            "main",         // entry point
            "ps_5_0",          // shader model
            "PIXELSHADER");

        pixelShader = nullptr;
        device->CreatePixelShader(fsBlob->GetBufferPointer(), fsBlob->GetBufferSize(), nullptr, &pixelShader);

        // if geometry shader path is present, also load a geometry shader
        if (geometryP != nullptr) {
            gsBlob = compileShader(
                geometryPath,   // archivo
                "main",         // entry point
                "gs_5_0",          // shader model
                "GEOMETRYSHADER");

            geometryShader = nullptr;
            device->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &geometryShader);
            gsBlob->Release();
        }
        fsBlob->Release();
        vsBlob->Release();
//        ZeroMemory(&cpuData, sizeof(cpuData));
        for (int i = 0; i < 200; i++)
            XMStoreFloat4x4(&boneData.finalBonesMatrices[i], DirectX::XMMatrixIdentity());

        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(CB_PerObject);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        device->CreateBuffer(&bd, nullptr, &constantBuffer);

        D3D11_BUFFER_DESC bd1 = {};
        bd1.Usage = D3D11_USAGE_DYNAMIC;
        bd1.ByteWidth = sizeof(CB_Bones);
        bd1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd1.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        device->CreateBuffer(&bd1, nullptr, &constantBuffer1);

    } catch (...) { //std::ifstream::failure& e
        LOGGER::LOGS::getLOGGER("shader").info("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ", "ERROR SHADER");
    }
}

void ShaderDX11::desuse() {
    ctx->VSSetConstantBuffers(0, 0, nullptr);
    ctx->PSSetConstantBuffers(0, 0, nullptr);
    ctx->VSSetConstantBuffers(1, 0, nullptr);
    ctx->IASetInputLayout(NULL);
    ctx->VSSetShader(NULL, nullptr, 0);
    ctx->PSSetShader(NULL, nullptr, 0);
    ctx->GSSetShader(NULL, nullptr, 0);
}

void ShaderDX11::use() {
    ctx->IASetInputLayout(layout);
    ctx->VSSetShader(vertexShader, nullptr, 0);
    ctx->PSSetShader(pixelShader, nullptr, 0);
    ctx->GSSetShader(geometryShader, nullptr, 0); // o nullptr
}

void ShaderDX11::apply() {
    D3D11_MAPPED_SUBRESOURCE mapped;
    ctx->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &cpuData, sizeof(cpuData));
    ctx->Unmap(constantBuffer, 0);
    ctx->VSSetConstantBuffers(0, 1, &constantBuffer);
    ctx->PSSetConstantBuffers(0, 1, &constantBuffer);
    ctx->Map(constantBuffer1, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &boneData, sizeof(boneData));
    ctx->Unmap(constantBuffer1, 0);
    ctx->VSSetConstantBuffers(1, 1, &constantBuffer1);
}

void ShaderDX11::setBool(const char* name, bool value) {
    if (strcmp(name, "textureSample") == 0)
        cpuData.textureSample = value ? 1 : 0; 
    else if (strcmp(name, "useBlending") == 0)
        cpuData.useBlending = value ? 1 : 0;
    else if (strcmp(name, "useDynamicLighting") == 0)
        cpuData.useDynamicLighting = value ? 1 : 0;
    else
        ERRORL("Missing Bool param name", name);
}

void ShaderDX11::setInt(const char* name, int value) {
    if (strcmp(name, "textureSample") == 0)
        cpuData.textureSample = value;
    else if (strcmp(name, "multipleInstances") == 0)
        cpuData.multipleInstances = value;
    else if (strcmp(name, "texture_diffuse1") == 0)
        cpuData.texture_diffuse1 = value; 
    else if (strcmp(name, "useBlending") == 0)
        cpuData.useBlending = value;
    else if (strcmp(name, "useDynamicLighting") == 0)
        cpuData.useDynamicLighting = value;
    else
        ERRORL("Missing Int param name", name);
}

void ShaderDX11::setFloat(const char* name, float value) {
    if (strcmp(name, "material.shininess") == 0)
        cpuData.shininess = value == 0 ? 1.0f : value;
    else if (strcmp(name, "time") == 0)
        cpuData.time = value;
    else if (strcmp(name, "width") == 0)
        cpuData.width = value;
    else if (strcmp(name, "height") == 0)
        cpuData.height = value;
    else if (strcmp(name, "blendFactor") == 0)
        cpuData.blendFactor = value;
    else
        ERRORL("Missing Float param name", name);
}

void ShaderDX11::setVec2(const char* name, const glm::vec2& value) {
    setVec2(name, value.x, value.y);
}
void ShaderDX11::setVec2(const char* name, float x, float y) {
    DirectX::XMFLOAT2 v(x, y);
    ERRORL("Missing V2 param name", name);
}

void ShaderDX11::setVec3(const char* name, const glm::vec3& value) {
    setVec3(name, value.x, value.y, value.z);
}
void ShaderDX11::setVec3(const char* name, float x, float y, float z) {
    DirectX::XMFLOAT3 v(x, y, z);
    if (strcmp(name, "cameraPos") == 0)
        cpuData.cameraPos = v;
    else if (strcmp(name, "material.ambient") == 0)
        cpuData.ambient = v;
    else if (strcmp(name, "material.diffuse") == 0)
        cpuData.diffuse = v;
    else if (strcmp(name, "material.specular") == 0)
        cpuData.specular = v;
    else if (strcmp(name, "light.ambient") == 0)
        cpuData.lightAmbient = v;
    else if (strcmp(name, "light.diffuse") == 0)
        cpuData.lightDiffuse = v;
    else if (strcmp(name, "light.specular") == 0)
        cpuData.lightSpecular = v;
    else if (strcmp(name, "light.position") == 0)
        cpuData.lightPos = v;
    else if (strcmp(name, "light.direction") == 0)
        cpuData.lightDirection = v;
    else if (strcmp(name, "viewPos") == 0)
        cpuData.viewPos = v;
    else if (strcmp(name, "billboardPos") == 0)
        cpuData.billboardPos = v;
    else if (strcmp(name, "camRight") == 0)
        cpuData.camRight = v;
    else if (strcmp(name, "color") == 0)
        cpuData.color = v;
    else
        ERRORL("Missing V3 param name", name);
}

void ShaderDX11::setVec4(const char* name, const glm::vec4& value) {
    setVec4(name, value.x, value.y, value.z, value.w);
}
void ShaderDX11::setVec4(const char* name, float x, float y, float z, float w) {
    DirectX::XMFLOAT4 v(x, y, z, w);
    ERRORL("Missing V4 param name", name);
}

void ShaderDX11::setMat2(const char* name, const glm::mat2& mat) {
    ERRORL("Missing M2 param name", name);
}

void ShaderDX11::setMat3(const char* name, const glm::mat3& mat) {
    DirectX::XMMATRIX m(&mat[0][0]);
    ERRORL("Missing M3 param name", name);
}

void ShaderDX11::setMat4(const char* name, const glm::mat4& mat) {
    DirectX::XMMATRIX m(&mat[0][0]);
    if (strcmp(name, "model") == 0)
        XMStoreFloat4x4(&cpuData.model, XMMatrixTranspose(m));
    else if (strcmp(name, "view") == 0)
        XMStoreFloat4x4(&cpuData.view, XMMatrixTranspose(m));
    else if (strcmp(name, "projection") == 0)
        XMStoreFloat4x4(&cpuData.projection, XMMatrixTranspose(m));
    else
        ERRORL("Missing M4 param name", name);
}
void ShaderDX11::setMat4Array(const char* name, const glm::mat4* matrices, size_t count) {
    for (size_t i = 0; i < count; i++) {
        const glm::mat4 &mat = matrices[i];
        DirectX::XMMATRIX m(&mat[0][0]);
        if (strcmp(name, "finalBonesMatrices") == 0) {
            // Mejor usar otro cbuffer solo para huesos
            XMStoreFloat4x4(&boneData.finalBonesMatrices[i], XMMatrixTranspose(m));
        } else {
            ERRORL("Missing param name", name);
            break;
        }
    }
}

ID3DBlob* ShaderDX11::compileShader(std::string& filename, const char* entryPoint, const char* shaderModel, std::string type) {
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    std::wstring wfilename = s2ws(filename);
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(ENGINE_DEBUG)
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = D3DCompileFromFile(
        wfilename.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint,
        shaderModel,
        flags,
        0,
        &shaderBlob,
        &errorBlob
    );

    checkCompileErrors(hr, errorBlob, type);
    return FAILED(hr) ? NULL : shaderBlob;
}

void ShaderDX11::checkCompileErrors(HRESULT hr, ID3DBlob* errorBlob, std::string& type) {
    if (FAILED(hr)) {
        if (errorBlob) {
            std::string error = "";
            error.append("ERROR::PROGRAM_LINKING_ERROR of type: ")
                .append(type)
                .append("\n")
                .append((char*)errorBlob->GetBufferPointer())
                .append("\n -- --------------------------------------------------- -- ").append("\n\r");
            LOGGER::LOGS::getLOGGER("shader").info(error, "ERROR SHADER");
            errorBlob->Release();
        }
    }
}
#endif