#pragma once
#include "Base/model.h"
#include "Billboard2D.h"

class Principal : public Model {
private:
    Billboard2D* vidas = NULL;
    int vida = 0;
    void initVidas(int vida = 1){
        glm::vec3 scale = glm::vec3(100.0f, 100.0f, 0.0f);	// it's a bit too big for our scene, so scale it down
        glm::vec3 trans = glm::vec3(50.00f, 50.0f, 0.0f);
        this->vida = vida;
        for (int i = 0; i < vida; i++){
            if (vidas == NULL)
                vidas = new Billboard2D((WCHAR*)L"textures/corazon1.png", 6, 6, 50, 100, 0, this->cameraDetails);
            auto attr = vidas->getModelAttributes();
            if (i == 0){
                vidas->setTranslate(&trans);
                vidas->setScale(&scale);
            } else {
                ModelAttributes m = attr->at(0);
                m.setTranslate(&trans);
                attr->push_back(m);
            }
            trans.x += 100.0f;
        }
    }
public:
    Principal() : Model(){
        initVidas(4);
    }
    Principal(string const& path, Camera* camera, bool rotationX = false, bool rotationY = true, bool gamma = false) 
        : Model(path, camera, rotationX, rotationY, gamma){
        initVidas(4);
    }
    Principal(vector<Vertex>& vertices, unsigned int numVertices, vector<unsigned int>& indices, unsigned int numIndices, Camera* camera)
        : Model(vertices, numVertices, indices, numIndices, camera){
        initVidas(4);
    }
    
    Principal (string const& path, glm::vec3& actualPosition, Camera* cam, bool rotationX = false, bool rotationY = true, bool gamma = false)
        : Model(path, actualPosition, cam, rotationX, rotationY, gamma){
        initVidas(4);
    }

    float restaVida(){
        if (vida > 0){
            vida--;
            auto attr = vidas->getModelAttributes();
            attr->at(vida).active = false;
        }
        return vida;
    }

    void Draw(){
        Model::Draw();
        vidas->Draw();            
    }

    ~Principal(){
        if (vidas != NULL)
            delete vidas;
    }
};