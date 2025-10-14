#pragma once
#include "Base/Utilities.h"
#include "Base/model.h"

class Ammo : public Model{
private:
    float acceleration = 1000.0f;
public:
    Ammo() : Model(){}
    Ammo(string const& path, Camera* camera, bool rotationX = false, bool rotationY = true, bool gamma = false)
     : Model(path, camera, rotationX, rotationY, gamma) {
        
    }
    Ammo(vector<Vertex>& vertices, unsigned int numVertices, vector<unsigned int>& indices, unsigned int numIndices, Camera* camera)
     : Model(vertices, numVertices, indices, numIndices, camera){

    }
    Ammo(string const& path, glm::vec3& actualPosition, Camera* cam, bool rotationX = false, bool rotationY = true, bool gamma = false)
     : Model(path, actualPosition, cam, rotationX, rotationY, gamma){

     }

    virtual ModelCollider update(float terrainY, std::vector<Model*>& models, glm::vec3 &ejeColision, bool gravityEnable = true, int idx = 0){
        ModelCollider collide = {0};
        if (idx != 0){
            ModelAttributes &mattr = this->getModelAttributes()->at(idx);
            glm::vec3 trans = mattr.nextTranslate;
            trans += mattr.forward * ( acceleration * (float)(gameTime.deltaTime / 1000));
            this->setNextTranslate(&trans,idx);
            collide = Model::update(terrainY, models, ejeColision, true, idx);
            collide.hitGround = collide.model != NULL? true : collide.hitGround;
        }
        return collide;
    }

    virtual void Draw(){
        if (this->gpuDemo == NULL) {
            this->gpuDemo = new Shader("shaders/models/1.model_material_loading.vs", "shaders/models/1.model_material_loading.fs");
            setDefaultShader(true);
        }
        if (getDefaultShader()) {
            this->gpuDemo->use();
            for (int i = 1 ; i < this->getModelAttributes()->size() ; i++){
                Model::prepShader(*gpuDemo, this->getModelAttributes()->at(i));
                Model::Draw(*gpuDemo, i);
            }
            gpuDemo->desuse();
        }
        else gpuDemo->desuse();
    }

};