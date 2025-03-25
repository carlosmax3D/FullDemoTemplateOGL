#include "CollitionBox.h"

int CollitionBox::idCollition = 0;
CollitionBox::CollitionBox(float x, float y, float z, float halfWidth, float halfHeight, float halfDepth, Camera* camera) {
    buildCollider(x, y, z, halfWidth, halfHeight, halfDepth);
    this->cameraDetails = camera;
    idCollition++;
    this->name = "CollitionBox"+std::to_string(idCollition);
}
CollitionBox::CollitionBox(glm::vec3 position, glm::vec3 halfWHD, Camera* camera) {
    buildCollider(position.x, position.y, position.z, halfWHD.x, halfWHD.y, halfWHD.z);
    this->cameraDetails = camera;
    idCollition++;
    this->name = "CollitionBox"+std::to_string(idCollition);
}

CollitionBox::~CollitionBox() {
    //nos aseguramos de disponer de los recursos previamente reservados
}