#include "CollitionBox.h"

int CollitionBox::idCollition = 0;
CollitionBox::CollitionBox(float x, float y, float z, float halfWidth, float halfHeight, float halfDepth, Camera* camera) {
    initCollitionBox(x, y, z, 0, 0, 0, halfWidth, halfHeight, halfDepth, camera);
}

CollitionBox::CollitionBox(glm::vec3& position, Node& node, Camera* camera) {
    initCollitionBox(position.x, position.y, position.z, node.m_center.x, node.m_center.y, node.m_center.z, node.m_halfWidth, node.m_halfHeight, node.m_halfDepth, camera);
}
CollitionBox::CollitionBox(glm::vec3& position, glm::vec3& halfWHD, Camera* camera) {
    initCollitionBox(position.x, position.y, position.z, 0, 0, 0, halfWHD.x, halfWHD.y, halfWHD.z , camera);
}

void CollitionBox::initCollitionBox(float x, float y, float z, float centerx, float centery, float centerz, float halfWidth, float halfHeight, float halfDepth, Camera* camera){
    this->cameraDetails = camera;
    buildCollider(centerx, centery, centerz, halfWidth, halfHeight, halfDepth);
    glm::vec3 pos = glm::vec3(x,y,z); 
    this->setTranslate(&pos);
    idCollition++;
    this->name = "CollitionBox"+std::to_string(idCollition);
    Material m{1};
    m.Ambient   = glm::vec3(0.2f);              // weak ambient
    m.Diffuse   = glm::vec3(1.0f, 0.0f, 0.0f);  // pure red for example
    m.Specular  = glm::vec3(0.0f);              // no specular at all
    m.Shininess = 32.0f;                        // any value, has no effect
    Model* AABB = (Model*)this->getModelAttributes()->at(0).hitbox;
    for (Mesh *mesh : AABB->meshes)
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
    Model* AABB = (Model*)this->getModelAttributes()->at(0).hitbox;
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
    Model* AABB = (Model*)this->getModelAttributes()->at(0).hitbox;
    if (AABB != NULL) AABB->setNextTranslate(translate);
}

CollitionBox::~CollitionBox() {
    //nos aseguramos de disponer de los recursos previamente reservados
}

Model* CollitionBox::GenerateAABB(glm::vec3 &position, Node& node, Camera* camera){
    vector<Vertex> cuboAABB = init_cube(node.m_center.x, node.m_center.y, node.m_center.z, node.m_halfWidth, node.m_halfHeight, node.m_halfDepth);
    vector<unsigned int> cuboIndex = getCubeIndex();
    Model *AABB = new Model(cuboAABB, cuboAABB.size(), cuboIndex, cuboIndex.size(), camera);
    Material m{1};
    m.Ambient   = glm::vec3(0.2f);              // weak ambient
    m.Diffuse   = glm::vec3(1.0f, 0.0f, 0.0f);  // pure red for example
    m.Specular  = glm::vec3(0.0f);              // no specular at all
    m.Shininess = 32.0f;                        // any value, has no effect
    AABB->setTranslate(&position);
    AABB->setNextTranslate(&position);
    for (Mesh *mesh : AABB->meshes){
        mesh->VBOGLDrawType = GL_LINE_LOOP;
        mesh->materials.push_back(m);
    }
    return AABB;
}