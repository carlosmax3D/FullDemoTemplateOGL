#include "CollitionBox.h"

int CollitionBox::idCollition = 0;
CollitionBox::CollitionBox(float x, float y, float z, float halfWidth, float halfHeight, float halfDepth, Camera* camera) {
    buildCollider(0, 0, 0, halfWidth, halfHeight, halfDepth);
    glm::vec3 pos = glm::vec3(x,y,z);
    this->setTranslate(&pos);
    this->cameraDetails = camera;
    idCollition++;
    this->name = "CollitionBox"+std::to_string(idCollition);
    Material m{1};
    m.Ambient   = glm::vec3(0.2f);              // weak ambient
    m.Diffuse   = glm::vec3(1.0f, 0.0f, 0.0f);  // pure red for example
    m.Specular  = glm::vec3(0.0f);              // no specular at all
    m.Shininess = 32.0f;                        // any value, has no effect
    for (Mesh *mesh : this->AABB->meshes)
        mesh->materials.push_back(m);
}
CollitionBox::CollitionBox(glm::vec3 position, glm::vec3 halfWHD, Camera* camera) {
    buildCollider(0, 0, 0, halfWHD.x, halfWHD.y, halfWHD.z);
    this->setTranslate(&position);
    this->cameraDetails = camera;
    idCollition++;
    this->name = "CollitionBox"+std::to_string(idCollition);
    Material m{1};
    m.Ambient   = glm::vec3(0.2f);              // weak ambient
    m.Diffuse   = glm::vec3(1.0f, 0.0f, 0.0f);  // pure red for example
    m.Specular  = glm::vec3(0.0f);              // no specular at all
    m.Shininess = 32.0f;                        // any value, has no effect
    for (Mesh *mesh : this->AABB->meshes)
        mesh->materials.push_back(m);
}

void CollitionBox::setTranslate(glm::vec3* translate) {
    if (translate == NULL) {
        this->getModelAttributes()->at(0).translate = glm::vec3(0);
        this->getModelAttributes()->at(0).hasTranslate = false;
        this->getModelAttributes()->at(0).nextTranslate = glm::vec3(0);
    }
    else {
        this->getModelAttributes()->at(0).translate = *translate;
        this->getModelAttributes()->at(0).nextTranslate = *translate;
        this->getModelAttributes()->at(0).hasTranslate = true;
    }
    if (AABB != NULL) AABB->setTranslate(translate);
}
void CollitionBox::setNextTranslate(glm::vec3* translate) {
    if (translate == NULL) {
        this->getModelAttributes()->at(0).translate = glm::vec3(0);
        this->getModelAttributes()->at(0).nextTranslate = glm::vec3(0);
    } else {
        this->getModelAttributes()->at(0).translate = *translate;
        this->getModelAttributes()->at(0).nextTranslate = *translate;
    }
    if (AABB != NULL) AABB->setNextTranslate(translate);
}

CollitionBox::~CollitionBox() {
    //nos aseguramos de disponer de los recursos previamente reservados
}