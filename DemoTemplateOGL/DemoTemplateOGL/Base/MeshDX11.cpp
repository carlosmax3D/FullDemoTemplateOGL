#include "Utilities.h"
#include "MeshDX11.h"
#if defined(MESHDX11_H) && defined(ENGINE_DIRECTX)

#include <d3d11.h>
#include <wrl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "material.h"
#include "shaderDX11.h"

#include <string>
#include <vector>
#include <list>

#ifndef _WIN32 
#define strcpy_s(x, y, z) strcpy(x, z)
#define strcat_s(x, y, z) strcat(x, z)
#endif

MeshDX11::MeshDX11(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, vector<Material>& materials, int VBOGLDrawType, int EBOGLDrawType, int TYPEGLDrawType) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->materials = materials;
    this->modelAttributes = NULL;
    this->EBOGLDrawType = EBOGLDrawType != GL_STATIC_DRAW;
    this->VBOGLDrawType = VBOGLDrawType;
    this->TYPEGLDrawType = TYPEGLDrawType;
    setupMesh();
}
MeshDX11::MeshDX11(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, int VBOGLDrawType, int EBOGLDrawType, int TYPEGLDrawType) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->modelAttributes = NULL;
    this->EBOGLDrawType = EBOGLDrawType != GL_STATIC_DRAW;
    this->VBOGLDrawType = VBOGLDrawType;
    this->TYPEGLDrawType = TYPEGLDrawType;
    setupMesh();
}

MeshDX11::~MeshDX11() {
    vertexBuffer->Release(); // vertex buffer
    indexBuffer->Release();       // index buffer
    instanceBuffer->Release(); // instance buffer
}

void MeshDX11::setupMesh() {
    nVertices = vertices.size();
    nIndices = indices.size();

    // ===============================
    // Vertex Buffer
    // ===============================
    D3D11_BUFFER_DESC vbd = {};
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.ByteWidth = UINT(sizeof(Vertex) * nVertices);
    vbd.Usage = EBOGLDrawType ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    vbd.CPUAccessFlags = EBOGLDrawType ? D3D11_CPU_ACCESS_WRITE : 0;

    D3D11_SUBRESOURCE_DATA vinit = {};
    vinit.pSysMem = vertices.data();

    device->CreateBuffer(&vbd, &vinit, &vertexBuffer);

    // ===============================
    // Index Buffer
    // ===============================
    if (nIndices > 0) {
        D3D11_BUFFER_DESC ibd = {};
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.ByteWidth = UINT(sizeof(unsigned int) * nIndices);
        ibd.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA iinit = {};
        iinit.pSysMem = indices.data();

        device->CreateBuffer(&ibd, &iinit, &indexBuffer);
    }

    // ===============================
    // Instance Buffer (vacío inicial)
    // ===============================
    D3D11_BUFFER_DESC instbd = {};
    instbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    instbd.Usage = D3D11_USAGE_DYNAMIC;
    instbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    instbd.ByteWidth = max<UINT>(sizeof(ModelAttributes), 1);

    device->CreateBuffer(&instbd, nullptr, &instanceBuffer);
}

void MeshDX11::updateDynamicVertexBuffer() {
    if (!EBOGLDrawType) return;
    D3D11_MAPPED_SUBRESOURCE mapped;
    ctx->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, vertices.data(), sizeof(Vertex) * vertices.size());
    ctx->Unmap(vertexBuffer, 0);
}

void MeshDX11::updateInstanceBuffer() {
    if (!modelAttributes || modelAttributes->empty())
        return;
    size_t needed = modelAttributes->size();
    size_t byteSize = sizeof(ModelAttributes) * needed;
    if (!instanceBuffer || nModelAttributes < needed) {
        instanceBuffer->Release();
        D3D11_BUFFER_DESC bd = {};
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.ByteWidth = UINT(byteSize);
        HRESULT hr = device->CreateBuffer(&bd, nullptr, &instanceBuffer);
        if (FAILED(hr)) return;
        nModelAttributes = needed;
    }
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (SUCCEEDED(ctx->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        memcpy(mapped.pData, modelAttributes->data(), byteSize);
        ctx->Unmap(instanceBuffer, 0);
    }
}
void MeshDX11::Draw(Shader &sh) {
    float blendFactor[4] = { 0,0,0,0 };
    ShaderDX11& shader = (ShaderDX11&)sh;
    ID3D11ShaderResourceView* srvTextures[5] = { 0 };
    ID3D11SamplerState* samplers[2] = { this->fontType ? createFontSampler() : createDefaultSampler(), createDefaultAlphaSampler() };
    if (this->VBOGLDrawType == GL_STATIC_DRAW && this->TYPEGLDrawType == GL_POINTS)
        TYPEGLDrawType = GL_POINTS;
    else {
        if (this->VBOGLDrawType == GL_DYNAMIC_DRAW) {
            ctx->RSSetState(createCullRasterizer(true));
            ctx->OMSetBlendState(createAlphaBlend(), blendFactor, 0xffffffff);
        } else
            ctx->RSSetState(createCullRasterizer(false));
    }
    int textureSample = 0;
    for (unsigned int i = 0; i < textures.size() || i < materials.size(); i++) {
        if (i < textures.size()) {
            char textShader[255] = { 0 };
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            if (strcmp(textures[i].type, "texture_diffuse") == 0) {
                if (srvTextures[0] != NULL) srvTextures[4] = textures[i].idDX11;
                else srvTextures[0] = textures[i].idDX11;
            } else if (strcmp(textures[i].type, "texture_specular") == 0)
                srvTextures[1] = textures[i].idDX11; // transfer unsigned int to stream
            else if (strcmp(textures[i].type, "texture_normal") == 0)
                srvTextures[2] = textures[i].idDX11; // transfer unsigned int to stream
            else if (strcmp(textures[i].type, "texture_height") == 0) {
                srvTextures[3] = textures[i].idDX11; // transfer unsigned int to stream
                if (srvTextures[0] == NULL) srvTextures[0] = textures[i].idDX11;
            }
            textureSample = 1;
            // now set the sampler to the correct texture unit
            strcpy_s(textShader, 255, textures[i].type);
            strcat_s(textShader, 255, number.c_str());
        }
        if (i < materials.size()) {
            Material& m_terial = materials[i];
            if (m_terial.hasAmbient && !(zeroVec3(m_terial.Ambient)))
                shader.setVec3("material.ambient", m_terial.Ambient);
            else shader.setVec3("material.ambient", glm::vec3(1.0f));
            if (m_terial.hasDiffuse && !(zeroVec3(m_terial.Diffuse)))
                shader.setVec3("material.diffuse", m_terial.Diffuse);
            else shader.setVec3("material.diffuse", glm::vec3(1.0f));
            if (m_terial.hasSpecular && !(zeroVec3(m_terial.Specular)))
                shader.setVec3("material.specular", m_terial.Specular); // specular lighting doesn't have full effect on this object's material
            else shader.setVec3("material.specular", glm::vec3(1.0f));
            shader.setFloat("material.shininess", m_terial.Shininess == 0 ? 1.0f : m_terial.Shininess);
        }
    }
    ID3D11VertexShader* vs = shader.vertexShader;
    ID3D11PixelShader* ps = shader.pixelShader;
    ID3D11GeometryShader* gs = shader.geometryShader;
    ID3D11InputLayout* layout = shader.layout;
    ctx->VSSetShader(vs, nullptr, 0);
    ctx->PSSetShader(ps, nullptr, 0);
    ctx->GSSetShader(gs, nullptr, 0);

    ctx->PSSetShaderResources(0, this->fontType ? 1 : 5, srvTextures);
    ctx->PSSetSamplers(0, this->fontType ? 1 : 2, samplers);
    shader.setInt("textureSample", textureSample);
    int multipleInstances = this->modelAttributes != NULL && this->modelAttributes->size() > 1;
    shader.setInt("multipleInstances", multipleInstances);
    shader.apply();
    updateDynamicVertexBuffer();
    updateInstanceBuffer();

    UINT strides[2] = { sizeof(Vertex), sizeof(ModelAttributes) };
    UINT offsets[2] = { 0, 0 };

    ID3D11Buffer* buffers[2] = { vertexBuffer, instanceBuffer };

    ctx->IASetInputLayout(layout);
    if (this->VBOGLDrawType == GL_STATIC_DRAW && this->TYPEGLDrawType == GL_POINTS)
        ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    else ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->IASetVertexBuffers(0, 2, buffers, strides, offsets);

    if (indexBuffer)
        ctx->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    switch (this->VBOGLDrawType) {
        case GL_DYNAMIC_DRAW:
            drawMultipleInstances(multipleInstances);
            ctx->OMSetBlendState(nullptr, nullptr, 0xffffffff);
            break;
        case GL_LINE_LOOP:
//            glDrawElements(GL_LINE_LOOP, (GLsizei)indices.size(), GL_UNSIGNED_INT, (const void*)0);
            ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
            ctx->DrawIndexed(nIndices, 0, 0);
            break;
        default:
            drawMultipleInstances(multipleInstances);
    }

    if (this->VBOGLDrawType == GL_STATIC_DRAW && this->TYPEGLDrawType == GL_POINTS)
        this->TYPEGLDrawType = GL_POINTS; //glDisable(GL_PROGRAM_POINT_SIZE);

}

void MeshDX11::drawMultipleInstances(int multipleInstances) {
    if (!multipleInstances) {
        if (indexBuffer)
            ctx->DrawIndexed(nIndices, 0, 0);
        else
            ctx->Draw(nVertices, 0);
    }
    else {
        if (indexBuffer)
            ctx->DrawIndexedInstanced(nIndices, modelAttributes->size(), 0, 0, 0);
        else
            ctx->DrawInstanced(nVertices, modelAttributes->size(), 0, 0);
    }
}

#endif