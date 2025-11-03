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
    unsigned int VAO;
    int EBOGLDrawType = GL_STATIC_DRAW;
    int VBOGLDrawType = GL_STATIC_DRAW;
    int TYPEGLDrawType = GL_TRIANGLES;

    ~Mesh(){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        if (VBOInstance != INT_MAX) glDeleteBuffers(1, &VBOInstance);
    }
    // constructor
    Mesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, vector<Material>& materials, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW, int TYPEGLDrawType = GL_TRIANGLES){
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->materials = materials;
        this->VBOGLDrawType = VBOGLDrawType;
        this->EBOGLDrawType = EBOGLDrawType;
        this->TYPEGLDrawType = TYPEGLDrawType;
        this->modelAttributes = NULL;
        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }
    Mesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW, int TYPEGLDrawType = GL_TRIANGLES) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->VBOGLDrawType = VBOGLDrawType;
        this->EBOGLDrawType = EBOGLDrawType;
        this->TYPEGLDrawType = TYPEGLDrawType;
        this->modelAttributes = NULL;
        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(Shader& shader) {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        if (this->VBOGLDrawType == GL_STATIC_DRAW && this->TYPEGLDrawType == GL_POINTS)
            glEnable(GL_PROGRAM_POINT_SIZE);
        else{
            glEnable(GL_CULL_FACE);
            if (this->VBOGLDrawType == GL_DYNAMIC_DRAW) {
                glCullFace(GL_FRONT);
                glEnable(GL_BLEND);
            }else
                glCullFace(GL_BACK);
        }
        //        glEnable(GL_TEXTURE_2D);
        for (unsigned int i = 0; i < textures.size() || i < materials.size(); i++) {
            if (i < textures.size()) {
                char textShader[255] = {0};
                glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
                // retrieve texture number (the N in diffuse_textureN)
                string number;
                if (strcmp(textures[i].type, "texture_diffuse") == 0)
                    number = std::to_string(diffuseNr++);
                else if (strcmp(textures[i].type, "texture_specular") == 0)
                    number = std::to_string(specularNr++); // transfer unsigned int to stream
                else if (strcmp(textures[i].type, "texture_normal") == 0)
                    number = std::to_string(normalNr++); // transfer unsigned int to stream
                else if (strcmp(textures[i].type, "texture_height") == 0)
                    number = std::to_string(heightNr++); // transfer unsigned int to stream
                shader.setInt("textureSample", 1);
                // now set the sampler to the correct texture unit
#ifdef _WIN32 
                strcpy_s(textShader, 255, textures[i].type);
                strcat_s(textShader, 255, number.c_str());
#else
                strcpy(textShader, textures[i].type);
                strcat(textShader, number.c_str());
#endif
                glUniform1i(glGetUniformLocation(shader.ID, textShader), i);
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            } else shader.setInt("textureSample", 0);
            if (i < materials.size()) {
                Material &m_terial = materials[i];
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
            else { // Default material for texture
                shader.setVec3("material.ambient", glm::vec3(1.0f));
                shader.setVec3("material.diffuse", glm::vec3(1.0f));
                shader.setVec3("material.specular", glm::vec3(1.0f));
                shader.setFloat("material.shininess", 1.0f);
            }
        }
        int multipleInstances = this->modelAttributes != NULL && this->modelAttributes->size() > 1;
        shader.setInt("multipleInstances", multipleInstances);
        // draw mesh
        glBindVertexArray(VAO);
        switch (this->VBOGLDrawType){
            case GL_DYNAMIC_DRAW:
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                if (nVertices < vertices.size()){
                    glBufferData(GL_ARRAY_BUFFER, (GLsizei)vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);
                    nVertices = vertices.size();
                }else{
                    // Actualiza todo el contenido del buffer con los nuevos datos
                    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
                }
                drawMultipleInstances(multipleInstances);
                glDisable(GL_BLEND);
                break;
            case GL_LINE_LOOP:
                glDrawElements(GL_LINE_LOOP, (GLsizei)indices.size(), GL_UNSIGNED_INT, (const void*)0);
                glBindVertexArray(0);
                break;
            default:
                drawMultipleInstances(multipleInstances);
                glBindVertexArray(0);
        }
        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
        if (this->VBOGLDrawType == GL_STATIC_DRAW && this->TYPEGLDrawType == GL_POINTS)
            glDisable(GL_PROGRAM_POINT_SIZE);
        //        glDisable(GL_TEXTURE_2D);
    }

private:
    // render data 
    unsigned int VBO, EBO, VBOInstance = INT_MAX;
    // initializes all the buffer objects/arrays
void setupMesh() {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &VBOInstance); // para instancing

        glBindVertexArray(VAO);

        nVertices = vertices.size();
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(Vertex), &vertices[0], this->VBOGLDrawType);

        if (indices.size() > 0){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], this->EBOGLDrawType);
        }

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,Position));//0
        // vertex texture coords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex normals
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));   
        // --- Atributos por instancia ---
        glBindBuffer(GL_ARRAY_BUFFER, VBOInstance);
        glBufferData(GL_ARRAY_BUFFER, (GLsizei)sizeof(ModelAttributes), NULL, this->VBOGLDrawType);
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(ModelAttributes), (void*)(offsetof(ModelAttributes, translate)));
        glVertexAttribDivisor(7, 1); // 1 → cambia por instancia
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(ModelAttributes), (void*)(offsetof(ModelAttributes, scale)));
        glVertexAttribDivisor(8, 1); // 1 → cambia por instancia
        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(ModelAttributes), (void*)(offsetof(ModelAttributes, rotX)));
        glVertexAttribDivisor(9, 1); // 1 → cambia por instancia
        glEnableVertexAttribArray(10);
        glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(ModelAttributes), (void*)(offsetof(ModelAttributes, rotY)));
        glVertexAttribDivisor(10, 1); // 1 → cambia por instancia
        glEnableVertexAttribArray(11);
        glVertexAttribPointer(11, 1, GL_FLOAT, GL_FALSE, sizeof(ModelAttributes), (void*)(offsetof(ModelAttributes, rotZ)));
        glVertexAttribDivisor(11, 1); // 1 → cambia por instancia
        glEnableVertexAttribArray(12);
        glVertexAttribPointer(12, 1, GL_FLOAT, GL_FALSE, sizeof(ModelAttributes), (void*)(offsetof(ModelAttributes, active)));
        glVertexAttribDivisor(12, 1); // 1 → cambia por instancia

        glBindVertexArray(0);
    }

    void drawMultipleInstances(int multipleInstances){
        if (multipleInstances == 0){
            if (indices.size() == 0)
                glDrawArrays(TYPEGLDrawType, 0, (GLsizei)vertices.size());
            else
                glDrawElements(TYPEGLDrawType, (GLsizei)indices.size(), GL_UNSIGNED_INT, (const void*)0);
            return;
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBOInstance);
        if (nModelAttributes < modelAttributes->size()){
            glBufferData(GL_ARRAY_BUFFER, (GLsizei)modelAttributes->size() * sizeof(ModelAttributes), modelAttributes->data(), this->VBOGLDrawType);
            nModelAttributes = modelAttributes->size();
        }else{
            // Subir todos los datos al VBO (reemplazando lo que había)
            glBufferSubData(GL_ARRAY_BUFFER, 0, modelAttributes->size() * sizeof(ModelAttributes), modelAttributes->data());
        }
        if (indices.size() == 0)
            glDrawArraysInstanced(TYPEGLDrawType, 0, 1, (GLsizei)modelAttributes->size());
        else
            glDrawElementsInstanced(TYPEGLDrawType, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0, (GLsizei)modelAttributes->size());
    }
};
#endif