#ifndef MESH_H
#define MESH_H

#include "Utilities.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "material.h"
#include "shader.h"

#include <string>
#include <vector>
#include <list>
using namespace std;

class Mesh {
public:
    //En honor a nuestros ancestros llamaremos "Maya" a la malla
    //e suna estructura que contiene a los indices y vertices de la figura
    //el nombre numerico de la textura en cuestion, por lo pronto una

    // mesh Data
    vector<Vertex>       vertices;
    unsigned int nVertices = 0;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    vector<Material>     materials;
    vector<ModelAttributes>*
                         modelAttributes;
    unsigned int nModelAttributes = 0;
    int VBOGLDrawType = GL_STATIC_DRAW;

    static Mesh* createMesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, vector<Material>& materials, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW, int TYPEGLDrawType = GL_TRIANGLES);
    
    static Mesh* createMesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW, int TYPEGLDrawType = GL_TRIANGLES);
        
    virtual ~Mesh();
    // constructor
    //Mesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, vector<Material>& materials, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW, int TYPEGLDrawType = GL_TRIANGLES);
    //Mesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW, int TYPEGLDrawType = GL_TRIANGLES);

    // render the mesh
    virtual void Draw(Shader& shader) = 0;

private:
};
#endif