#include "Utilities.h"
#if !defined(MESHGL33_H) && !defined(ENGINE_DIRECTX)
#define MESHGL33_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "material.h"
#include "shader.h"
#include "mesh.h"

#include <string>
#include <vector>
#include <list>
using namespace std;

class MeshGL33 : public Mesh {
public:
    unsigned int VAO;
    int EBOGLDrawType = GL_STATIC_DRAW;
    int TYPEGLDrawType = GL_TRIANGLES;
    ~MeshGL33();
    // constructor

    MeshGL33(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, vector<Material>& materials, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW, int TYPEGLDrawType = GL_TRIANGLES);
    MeshGL33(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW, int TYPEGLDrawType = GL_TRIANGLES);

    // render the mesh
    void Draw(Shader& shader);

private:
    // render data 
    unsigned int VBO, EBO, VBOInstance = INT_MAX;

    // initializes all the buffer objects/arrays
    void setupMesh();
    void drawMultipleInstances(int multipleInstances);
};
#endif