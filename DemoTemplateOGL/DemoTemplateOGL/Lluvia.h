#ifndef LLUVIA_H
#define LLUVIA_H

#include "Base/Utilities.h"
#include "Base/camera.h"
#include <glm/glm.hpp>
#include "Base/shader.h"
#include <vector>
#include <cstdlib>
#include <ctime>

#define NUM_GOTAS 750

class Lluvia {
private:
    struct Gota {
        glm::vec3 pos;
        float velocidad;
    };

    std::vector<Gota> gotas;
    unsigned int VAO, VBO;
    float anchoEscena, largoEscena, alturaMaxima;
    Shader *gpuDemo = NULL;
    Camera *cam = NULL;
public:
    Lluvia(float ancho, float largo, float altura = 90.0f, Camera *camDetails = NULL)
        : anchoEscena(ancho), largoEscena(largo), alturaMaxima(altura)
    {
        cam = camDetails;
        srand(static_cast<unsigned int>(time(nullptr)));
        gotas.resize(NUM_GOTAS);
        for (int i = 0; i < NUM_GOTAS; ++i) {
            gotas[i].pos.x = static_cast<float>(rand() % (int)anchoEscena) - anchoEscena / 2;
            gotas[i].pos.y = static_cast<float>(rand() % (int)alturaMaxima);
            gotas[i].pos.z = static_cast<float>(rand() % (int)largoEscena) - largoEscena / 2;
            gotas[i].velocidad = 0.75f + static_cast<float>(rand() % 100) / 500.0f * (gameTime.deltaTime / 10000);
        }

        // OpenGL setup
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Each drop has 2 vertices (start + end of line)
        glBufferData(GL_ARRAY_BUFFER, NUM_GOTAS * 2 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glBindVertexArray(0);
    }

    void Update() {
        for (int i = 0; i < NUM_GOTAS; ++i) {
            gotas[i].pos.y -= gotas[i].velocidad;
            if (gotas[i].pos.y < 0) {
                gotas[i].pos.y = alturaMaxima;
                gotas[i].pos.x = static_cast<float>(rand() % (int)anchoEscena) - anchoEscena / 2;
                gotas[i].pos.z = static_cast<float>(rand() % (int)largoEscena) - largoEscena / 2;
            }
        }
    }

    void Draw(){
        if (gpuDemo == NULL){
            gpuDemo = new Shader("shaders/lluvia.vs", "shaders/lluvia.fs");
        }
        Draw(*gpuDemo);
    }

    void Draw(Shader& shader) {
        std::vector<glm::vec3> lineVertices;
        lineVertices.reserve(NUM_GOTAS * 2);

        for (auto& g : gotas) {
            lineVertices.push_back(g.pos);
            lineVertices.push_back(g.pos + glm::vec3(0.0f, -0.5f, 0.0f)); // short line down
        }

        // Upload updated positions
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, lineVertices.size() * sizeof(glm::vec3), lineVertices.data());

        // Draw
        shader.use();
        glm::mat4 proj = cam->getProjection();
        glm::mat4 view = cam->getView();
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "proj"), 1, GL_FALSE, &proj[0][0]);
        shader.setVec3("rainColor", glm::vec3(0.5f, 0.5f, 1.0f));

        glBindVertexArray(VAO);
        glLineWidth(3.0f); // grosor de lluvia
        glDrawArrays(GL_LINES, 0, (GLsizei)lineVertices.size());
        glBindVertexArray(0);
        glLineWidth(1.0f); // grosor de lluvia
    }
};

#endif
