#include "Utilities.h"
#if !defined(MESHDX11_H) && defined(ENGINE_DIRECTX)
#define MESHDX11_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "material.h"
#include "shader.h"
#include "mesh.h"

#include <string>
#include <vector>
#include <list>
using namespace std;

class MeshDX11 : public Mesh{
public:
    // D3D11 resources
    ID3D11Buffer* vertexBuffer = NULL;       // vertex buffer
    ID3D11Buffer* indexBuffer = NULL;       // index buffer
    ID3D11Buffer* instanceBuffer = NULL; // instance buffer

    // flags / counts
    bool fontType = false;
    bool EBOGLDrawType = false;
    int TYPEGLDrawType = 0;

    size_t nVertices = 0;
    size_t nIndices = 0;
    size_t nModelAttributes = 0;

    ~MeshDX11();
    // constructor

    MeshDX11(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, vector<Material>& materials, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW, int TYPEGLDrawType = GL_TRIANGLES);
    MeshDX11(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW, int TYPEGLDrawType = GL_TRIANGLES);

    // render the mesh
    void Draw(Shader& sh, vector<ModelAttributes>* modelAttributes = NULL);

private:
    // render data 
    unsigned int VBO, EBO, VBOInstance = INT_MAX;

    // initializes all the buffer objects/arrays
    void setupMesh();
    void drawMultipleInstances(int multipleInstances, vector<ModelAttributes>* modelAttributes = NULL);
    void updateInstanceBuffer(vector<ModelAttributes>* modelAttributes = NULL);
    void updateDynamicVertexBuffer();

};
#endif