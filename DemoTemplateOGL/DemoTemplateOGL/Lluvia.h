#ifndef LLUVIA_H
#define LLUVIA_H

#include "Base/Utilities.h"
#include "Base/Billboard.h"
#include "Base/camera.h"
#include <glm/glm.hpp>
#include "Base/shader.h"
#include <vector>
#include <cstdlib>
#include <ctime>

#define NUM_GOTAS 750

class Lluvia : public Billboard{
private:
    float anchoEscena, largoEscena, alturaMaxima;
    vector<float> velocidad;
public:
    Lluvia(float ancho, float largo, float altura = 90.0f, Camera *camDetails = NULL)
        : Billboard(0, (WCHAR*)L"Lluvia", 0, 0, 0, camDetails)
    {
        this->cameraDetails = camDetails;
        anchoEscena = ancho;
        largoEscena = largo;
        alturaMaxima = altura;
        srand(static_cast<unsigned int>(time(nullptr)));
        vector<Vertex> gotas;
        vector<unsigned int> indices;
        vector<Texture> textures;
        gotas.resize(NUM_GOTAS * 2);
        velocidad.resize(NUM_GOTAS);
        for (int i = 0; i < NUM_GOTAS * 2; i = i + 2) {
            gotas[i].Position.x = static_cast<float>(rand() % (int)anchoEscena) - anchoEscena / 2;
            gotas[i].Position.y = static_cast<float>(rand() % (int)alturaMaxima);
            gotas[i].Position.z = static_cast<float>(rand() % (int)largoEscena) - largoEscena / 2;
            velocidad[i/2] = 0.75f + static_cast<float>(rand() % 100) / 500.0f * (gameTime.deltaTime / 10000);
        }
        meshes.push_back(new Mesh(gotas, indices, textures, GL_DYNAMIC_DRAW, GL_STATIC_DRAW, GL_LINES));
    }

    void Update() {
        vector<Vertex>& gotas = meshes.at(0)->vertices;
        for (int i = 0; i < NUM_GOTAS * 2; i = i + 2) {
            gotas[i].Position.y -= velocidad[i/2];
            if (gotas[i].Position.y < 0) {
                gotas[i].Position.y = alturaMaxima;
                gotas[i].Position.x = static_cast<float>(rand() % (int)anchoEscena) - anchoEscena / 2;
                gotas[i].Position.z = static_cast<float>(rand() % (int)largoEscena) - largoEscena / 2;
            }
            gotas[i+1].Position = (gotas[i].Position + glm::vec3(0.0f, -0.5f, 0.0f)); // short line down
        }
    }

    void Draw(){
        if (gpuDemo == NULL){
            gpuDemo = new Shader("shaders/lluvia.vs", "shaders/lluvia.fs");
        }
        Draw(*gpuDemo);
    }

    void Draw(Shader& shader) {
        // Draw
        shader.use();
        glm::mat4 proj = cameraDetails->getProjection();
        glm::mat4 view = cameraDetails->getView();
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "proj"), 1, GL_FALSE, &proj[0][0]);
        shader.setVec3("rainColor", glm::vec3(0.5f, 0.5f, 1.0f));

        glLineWidth(3.0f); // grosor de lluvia
        Model::Draw(shader, 0);
        glLineWidth(1.0f); // grosor de lluvia
        shader.desuse();
    }
};

#endif
