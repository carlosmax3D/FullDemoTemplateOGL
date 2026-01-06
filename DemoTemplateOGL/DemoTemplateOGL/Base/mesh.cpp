#include "Utilities.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"
#ifdef ENGINE_DIRECTX
#include "MeshDX11.h"
#else
#include "MeshGL33.h"
#endif
#include "material.h"
#include "shader.h"

#include <string>
#include <vector>
#include <list>
using namespace std;

Mesh* Mesh::createMesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, vector<Material>& materials, int VBOGLDrawType, int EBOGLDrawType, int TYPEGLDrawType) {
        Mesh* mesh = NULL;
#ifdef ENGINE_DIRECTX
        mesh = (Mesh*)new MeshDX11(vertices, indices, textures, materials, VBOGLDrawType, EBOGLDrawType, TYPEGLDrawType);
#else
        mesh = (Mesh*)new MeshGL33(vertices, indices, textures, materials, VBOGLDrawType, EBOGLDrawType, TYPEGLDrawType);
#endif
        return mesh;
}

Mesh* Mesh::createMesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, int VBOGLDrawType, int EBOGLDrawType, int TYPEGLDrawType) {
    Mesh* mesh = NULL;
#ifdef ENGINE_DIRECTX
    mesh = (Mesh*)new MeshDX11(vertices, indices, textures, VBOGLDrawType, EBOGLDrawType, TYPEGLDrawType);
#else
    mesh = (Mesh*)new MeshGL33(vertices, indices, textures, VBOGLDrawType, EBOGLDrawType, TYPEGLDrawType);
#endif
    return mesh;
}

// constructor
//Mesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, vector<Material>& materials, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW, int TYPEGLDrawType = GL_TRIANGLES);
//Mesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW, int TYPEGLDrawType = GL_TRIANGLES);

Mesh::~Mesh() {

}