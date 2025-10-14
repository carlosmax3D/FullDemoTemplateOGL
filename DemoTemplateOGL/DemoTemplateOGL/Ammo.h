#pragma once
#include "Base/Utilities.h"
#include "Base/model.h"

class Ammo : public Model{
private:
    float acceleration = 10.0f;
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

    ModelCollider update(float terrainY, std::vector<Model*>& models, glm::vec3 &ejeColision, bool gravityEnable = false, int idx = 0){
        ModelCollider collide = {0};
        if (idx == 0)
            return collide;
        ModelAttributes &mattr = this->getModelAttributes()->at(idx);
        glm::vec3 trans = mattr.nextTranslate;
        trans += mattr.forward * ( acceleration * (float)(gameTime.deltaTime / 1000));
        glm::vec3 normDir = glm::normalize(mattr.forward);
        this->setNextTranslate(&trans,idx);
        if (this->ignoreAABB)
            return collide;
        // Apply gravity
        this->getVelocity()->y += GRAVITY * gameTime.deltaTime / 1000;
        if (this->getVelocity()->y < TERMINAL_VELOCITY) {
            this->getVelocity()->y = TERMINAL_VELOCITY;
        }

        // Predict next position
        glm::vec3 nextGPosition = *this->getNextTranslate(idx);
        glm::vec3 prevGPosition = *this->getTranslate(idx);
        glm::vec3 nextPosition = *this->getNextTranslate(idx);
        glm::vec3 prevPosition = *this->getTranslate(idx);
        if (gravityEnable) {
            nextGPosition += *this->getVelocity();
            prevGPosition += *this->getVelocity();
        }

        bool thisInMovement = true;//(*getNextTranslate()) != (*getTranslate());
        // Check terrain collision
        if (gravityEnable && prevGPosition.y < terrainY) {
            prevGPosition.y = terrainY;
            collide.hitGround = true;
        }
        if (gravityEnable && nextGPosition.y < terrainY) {
            nextGPosition.y = terrainY;
            this->getVelocity()->y = 0.0f;
            collide.hitGround = true;
        }
        setNextTranslate(&nextGPosition, idx);
        if (mattr.life > 0)
            mattr.life -= gameTime.deltaTime / 100;
        else {
            collide.model = NULL;
            collide.attrIdx = -1;
            collide.hitGround = true;
        }
        glm::vec3 yPos;
        // Check collisions with objects
        yPos = glm::vec3(0);
        float closestP = 100000.0f, closestN = 0.0f;
        for (int i = 0; i < models.size(); i++) {
            Model* other = models[i];
            for (int j = 0; j < other->getModelAttributes()->size(); j++) {
                if (this != other && other != mattr.owner && other->intersectaRayo(mattr.origin, normDir, false, closestN, j)) {
                    this->getVelocity()->y = 0.0f;  // Stop downward movement
                    if (closestN > closestP) continue; // nos quedamos con la colision mas cercana
                    closestP = closestN;
                    ejeColision.x = 1;
                    ejeColision.z = 1;
                    collide.model = other;
                    collide.attrIdx = j;
                    collide.hitGround = true;
                    break;
                }
            }
            if (i < 0) i = 0;
        }

        // Apply final position
        this->setTranslate(getNextTranslate(idx), idx);
        setRotX(getNextRotX(idx), idx);
        setRotY(getNextRotY(idx), idx);
        setRotZ(getNextRotZ(idx), idx);
        collide.hitGround = collide.model != NULL? true : collide.hitGround;
        return collide;
    }

    virtual void Draw(){
        if (this->gpuDemo == NULL) {
            this->gpuDemo = new Shader("shaders/models/1.model_material_loading.vs", "shaders/models/1.model_material_loading.fs");
            setDefaultShader(true);
        }
        if (getDefaultShader()) {
            this->gpuDemo->use();
            this->getModelAttributes()->at(0).active = false;
            for (int i = 0 ; i < 1 ; i++){
                Model::prepShader(*gpuDemo, this->getModelAttributes()->at(i));
                Model::Draw(*gpuDemo, i);
            }
            this->getModelAttributes()->at(0).active = true;
            gpuDemo->desuse();
        }
        else gpuDemo->desuse();
    }

};