#include "model.h"
#ifdef __linux__ 
#define ZeroMemory(x,y) memset(x,0,y)
#define strcat_s(x,y,z) strcat(x,z)
#define wcscpy_s(x,y,z) wcscpy(x,z)
#define strcpy_s(x,y,z) strcpy(x,z)
#define wcstombs_s(x, y, z, w, r) wcstombs(y,w,r)
#endif

Model_Exception::Model_Exception(){ }
const char* Model_Exception::what(){
    return "Failed at load the model";
}

Model::Model() {
    this->cameraDetails = NULL;
    this->gammaCorrection = false;
    defaultShader = false;
    ModelAttributes m{0};
    this->attributes.push_back(m);
    for (Mesh *m : meshes){
        m->modelAttributes = &this->attributes;
    }
}
Model::Model(string const& path, Camera* camera, bool rotationX, bool rotationY, bool gamma){
    this->cameraDetails = camera;
    gammaCorrection = gamma;
    loadModel(path, rotationX, rotationY);
    defaultShader = false;
    ModelAttributes m{0};
    this->attributes.push_back(m);
    buildKDtree();
    for (Mesh *m : meshes){
        m->modelAttributes = &this->attributes;
    }
}
Model::Model(vector<Vertex>& vertices, unsigned int numVertices, vector<unsigned int>& indices, unsigned int numIndices, Camera* camera) {
    vector<Texture> textures;
    vector<Material> materials;
    meshes.emplace_back(new Mesh(vertices, indices, textures, materials));
    this->defaultShader = false;
    gpuDemo = NULL;
    this->cameraDetails = camera;
    ModelAttributes m{0};
    this->attributes.push_back(m);
//    buildKDtree();
    for (Mesh *m : meshes){
        m->modelAttributes = &this->attributes;
    }
}
Model::Model(string const& path, glm::vec3& actualPosition, Camera *cam, bool rotationX, bool rotationY, bool gamma) {
    cameraDetails = cam;
    ModelAttributes m{0};
    this->attributes.push_back(m);
    this->setTranslate(&actualPosition);
    this->gammaCorrection = gamma;
    Model::loadModel(path, rotationX, rotationY);
    this->defaultShader = false;
    buildKDtree();
    for (Mesh *m : meshes){
        m->modelAttributes = &this->attributes;
    }
}

Model::~Model() {
    for (ModelAttributes &attr : *this->getModelAttributes()){
        Model *AABB = (Model*)attr.hitbox;
        if (attr.hitbox != NULL){
            delete AABB;
            attr.hitbox = NULL;
        }
    }
    if (gpuDemo != NULL) {
        delete gpuDemo;
        gpuDemo = NULL;
    }
    for (int i = 0; cleanTextures && i < textures_loaded.size(); i++) {
        glDeleteTextures(1, &(textures_loaded[i]->id));
    }
    for (int i = 0; i < meshes.size(); i++) {
        delete meshes[i];
    }
}

std::unordered_map<string, int>* Model::GetBoneInfoMap() { return &m_BoneInfoMap; }
std::vector<BoneInfo>* Model::getBonesInfo(){ return &bonesInfo; }

int& Model::GetBoneCount() { return m_BoneCounter; }    

void Model::SetVertexBoneDataToDefault(Vertex& vertex){
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        vertex.m_BoneIDs[i] = -1;
        vertex.m_Weights[i] = 0.0f;
    }
}

// draws the model, and thus all its meshes
void Model::prepShader(Shader& gpuDemo, ModelAttributes& attributes) {
//    lightColor.x = 3;//sin(7200 * 2.0f);
//    lightColor.y = 3;//sin(7200 * 0.7f);
//    lightColor.z = 3;//sin(7200 * 1.3f);
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
    gpuDemo.setVec3("light.ambient", ambientColor);
    gpuDemo.setVec3("light.diffuse", diffuseColor);
    gpuDemo.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    gpuDemo.setVec3("light.direction", cameraDetails->getFront());
    //        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    gpuDemo.setVec3("light.position", lightPos);
    gpuDemo.setVec3("viewPos", cameraDetails->getPosition());

    // view/projection transformations
    gpuDemo.setMat4("projection", cameraDetails->getProjection());
    gpuDemo.setMat4("view", cameraDetails->getView());

    if (getModelAttributes()->size() == 1){
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        if (attributes.hasTranslate)
            model = glm::translate(model, attributes.translate); // translate it down so it's at the center of the scene
    //			model = glm::translate(model, glm::vec3(cameraDetails.Position->x, cameraDetails.Position->y - 5, cameraDetails.Position->z)); // translate it down so it's at the center of the scene
            //model = glm::scale(model, glm::vec3(0.0025f, 0.0025f, 0.0025f));	// it's a bit too big for our scene, so scale it down
        if (attributes.rotation.x != 0)
            model = glm::rotate(model, glm::radians(attributes.rotX), glm::vec3(1, 0, 0));
        if (attributes.rotation.y != 0)
            model = glm::rotate(model, glm::radians(attributes.rotY), glm::vec3(0, 1, 0));
        if (attributes.rotation.z != 0)
            model = glm::rotate(model, glm::radians(attributes.rotZ), glm::vec3(0, 0, 1));
        if (attributes.hasScale)
            model = glm::scale(model, attributes.scale);	// it's a bit too big for our scene, so scale it down
        gpuDemo.setMat4("model", model);
    }
}
void Model::Draw() {
    if (gpuDemo == NULL) {
        gpuDemo = new Shader("shaders/models/1.model_material_loading.vs", "shaders/models/1.model_material_loading.fs");
        defaultShader = true;
    }
    if (defaultShader) {
        gpuDemo->use();
        prepShader(*gpuDemo, attributes[0]);
        Draw(*gpuDemo, 0);
        auto modelAttributes = getModelAttributes();
        for (int i = 0 ; i < getModelAttributes()->size() ; i++){
            Model *AABB = (Model*)getModelAttributes()->at(i).hitbox;
            if (showHitbox && AABB){
                AABB->prepShader(*gpuDemo, AABB->getModelAttributes()->at(0));
                AABB->Draw(*gpuDemo, 0);
            }
        }
        gpuDemo->desuse();
    }
    else gpuDemo->desuse();
}
void Model::Draw(Shader& shader, int idxAttribute) {
    ModelAttributes &attribute = attributes[idxAttribute];
    if (animatorIdx != -1 && idxAttribute == 0){
        Animator &animator = animators[animatorIdx];
        animator.UpdateAnimation(gameTime.deltaTime / 1000, glm::mat4(1));
        const glm::mat4* transforms = animator.GetFinalBoneMatrices();
        shader.setMat4Array("finalBonesMatrices", transforms, MAX_MODEL_BONES);
    }
    if ((attributes.size() == 1 && attribute.active) || attributes.size() > 1)
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i]->Draw(shader);
}
glm::mat4 Model::makeTransScale(const glm::mat4& prevTransformations, int idx) const {
    glm::mat4 model = makeTrans(idx) * prevTransformations;
    if (this->attributes[idx].rotation.x != 0 || this->attributes[idx].rotation.y != 0 || this->attributes[idx].rotation.z != 0) {
        if (this->attributes[idx].rotation.x != 0)
            model = glm::rotate(model, glm::radians(this->attributes[idx].rotX), glm::vec3(1,0,0));
        if (this->attributes[idx].rotation.y != 0)
            model = glm::rotate(model, glm::radians(this->attributes[idx].rotY), glm::vec3(0,1,0));
        if (this->attributes[idx].rotation.z != 0)
            model = glm::rotate(model, glm::radians(this->attributes[idx].rotZ), glm::vec3(0,0,1));
    }
    if (attributes[idx].hasScale)
        model = glm::scale(model, attributes[idx].scale);
    return model;
}
glm::mat4 Model::makeTrans(int idx) const {
    return  glm::translate(glm::mat4(1), attributes[idx].translate);//glm::mat4(1) *glm::mat4(1)* glm::mat4(1);
}
glm::mat4 Model::makeTransScaleNextPosition(const glm::mat4& prevTransformations, int idx) {
    glm::mat4 model = makeTransNextPosition(idx) * prevTransformations;
    if (this->attributes[idx].nextRotation.x != 0 || this->attributes[idx].nextRotation.y != 0 || this->attributes[idx].nextRotation.z != 0) {
        if (this->attributes[idx].nextRotation.x != 0)
            model = glm::rotate(model, glm::radians(this->attributes[idx].nextRotX), glm::vec3(1, 0, 0));
        if (this->attributes[idx].nextRotation.y != 0)
            model = glm::rotate(model, glm::radians(this->attributes[idx].nextRotY), glm::vec3(0, 1, 0));
        if (this->attributes[idx].nextRotation.z != 0)
            model = glm::rotate(model, glm::radians(this->attributes[idx].nextRotZ), glm::vec3(0, 0, 1));
    }
    if (attributes[idx].hasScale)
        model = glm::scale(model, attributes[idx].scale);
    return model;
}
glm::mat4 Model::makeTransNextPosition(int idx) {
    glm::vec3 pos = *this->getNextTranslate(idx);
    return  glm::translate(glm::mat4(1), pos);//glm::mat4(1) *glm::mat4(1)* glm::mat4(1);
}
bool Model::getDefaultShader() { return defaultShader; }
void Model::setDefaultShader(bool defaultShader) { this->defaultShader = defaultShader; }

void Model::setTranslate(glm::vec3* translate, int idx) {
    if (translate == NULL) {
        this->attributes[idx].translate = glm::vec3(0);
        this->attributes[idx].hasTranslate = false;
    }
    else {
        this->attributes[idx].translate = *translate;
        this->attributes[idx].hasTranslate = true;
    }
    Model* AABB = (Model*)this->getModelAttributes()->at(idx).hitbox;
    if (AABB != NULL) AABB->setTranslate(translate);
}
void Model::setNextTranslate(glm::vec3* translate, int idx) {
    if (translate == NULL) {
        this->attributes[idx].nextTranslate = glm::vec3(0);
    } else {
        this->attributes[idx].nextTranslate = *translate;
    }
    Model* AABB = (Model*)this->getModelAttributes()->at(idx).hitbox;
    if (AABB != NULL) AABB->setNextTranslate(translate);
}
void Model::setScale(glm::vec3* scale, int idx) {
    if (scale == NULL) {
        this->attributes[idx].scale = glm::vec3(0);
        this->attributes[idx].hasScale = false;
    }
    else {
        this->attributes[idx].scale = *scale;
        this->attributes[idx].hasScale = true;
    }
    Model* AABB = (Model*)this->getModelAttributes()->at(idx).hitbox;
    if (AABB != NULL) AABB->setScale(scale);
}

void Model::setRotX(float rotationAngle, int idx) {
    this->attributes[idx].rotX = rotationAngle;
    this->attributes[idx].rotation.x = rotationAngle == 0 ? 0 : 1;
    Model* AABB = (Model*)this->getModelAttributes()->at(idx).hitbox;
    if (AABB != NULL) AABB->setRotX(rotationAngle);
}
void Model::setRotY(float rotationAngle, int idx) {
    this->attributes[idx].rotY = rotationAngle;
    this->attributes[idx].rotation.y = rotationAngle == 0 ? 0 : 1;
    Model* AABB = (Model*)this->getModelAttributes()->at(idx).hitbox;
    if (AABB != NULL) AABB->setRotY(rotationAngle);
}
void Model::setRotZ(float rotationAngle, int idx) {
    this->attributes[idx].rotZ = rotationAngle;
    this->attributes[idx].rotation.z = rotationAngle == 0 ? 0 : 1;
    Model* AABB = (Model*)this->getModelAttributes()->at(idx).hitbox;
    if (AABB != NULL) AABB->setRotZ(rotationAngle);
}
void Model::setNextRotX(float rotationAngle, int idx) {
    this->attributes[idx].nextRotX = rotationAngle;
    this->attributes[idx].nextRotation.x = rotationAngle == 0 ? 0 : 1;
    Model* AABB = (Model*)this->getModelAttributes()->at(idx).hitbox;
    if (AABB != NULL) AABB->setNextRotX(rotationAngle);
}
void Model::setNextRotY(float rotationAngle, int idx) {
    this->attributes[idx].nextRotY = rotationAngle;
    this->attributes[idx].nextRotation.y = rotationAngle == 0 ? 0 : 1;
    Model* AABB = (Model*)this->getModelAttributes()->at(idx).hitbox;
    if (AABB != NULL) AABB->setNextRotY(rotationAngle);
}
void Model::setNextRotZ(float rotationAngle, int idx) {
    this->attributes[idx].nextRotZ = rotationAngle;
    this->attributes[idx].nextRotation.z = rotationAngle == 0 ? 0 : 1;
    Model* AABB = (Model*)this->getModelAttributes()->at(idx).hitbox;
    if (AABB != NULL) AABB->setNextRotZ(rotationAngle);
}

glm::vec3* Model::getTranslate(int idx) {
    return &this->attributes[idx].translate;
}
glm::vec3* Model::getNextTranslate(int idx) {
    return &this->attributes[idx].nextTranslate;
}

glm::vec3* Model::getScale(int idx) {
    return &this->attributes[idx].scale;
}

float Model::getRotX(int idx) {
    return this->attributes[idx].rotX;
}
float Model::getRotY(int idx) {
    return this->attributes[idx].rotY;
}
float Model::getRotZ(int idx) {
    return this->attributes[idx].rotZ;
}

glm::vec3* Model::getRotationVector(int idx) {
    return &this->attributes[idx].rotation;
}
float Model::getNextRotX(int idx) {
    return this->attributes[idx].nextRotX;
}
float Model::getNextRotY(int idx) {
    return this->attributes[idx].nextRotY;
}
float Model::getNextRotZ(int idx) {
    return this->attributes[idx].nextRotZ;
}

glm::vec3* Model::getNextRotationVector(int idx) {
    return &this->attributes[idx].nextRotation;
}

bool Model::getActive(int idx){
    return attributes[idx].active;
}

void Model::setActive(bool active, int idx){
    this->attributes[idx].active = active;
}

std::vector<Animator>& Model::getAnimator(){
    return this->animators;
}
void Model::setAnimator(Animator animator){
    this->animators.emplace_back(animator);
    animatorIdx = 0;
}
void Model::setAnimator(std::vector<Animator>& animators){
    for (Animator &animator : animators){
        this->animators.emplace_back(animator);
        Animation &anim = this->animators.back().getAnimation();
        anim.setBoneInfoMap(this->GetBoneInfoMap());
        anim.setBonesInfo(this->getBonesInfo());
    }
    animatorIdx = 0;
}
void Model::setAnimation(unsigned int id){
    if (this->animatorIdx != -1 && this->animatorIdx != id)
        this->animators[animatorIdx].PlayAnimation();
    this->animatorIdx = id >= animators.size() ? -1 : id;
}

void Model::setVelocity(glm::vec3* velocity){
    this->velocity = *velocity;
}

glm::vec3* Model::getVelocity(){
    return &velocity;
}

ModelCollider Model::update(float terrainY, std::vector<Model*>& models, glm::vec3 &ejeColision, bool gravityEnable, int idx){
    ModelCollider collide = { 0 };
    if (this->ignoreAABB)
        return collide;
    // Apply gravity
    this->velocity.y += GRAVITY * gameTime.deltaTime/1000;
    if (this->velocity.y < TERMINAL_VELOCITY) {
        this->velocity.y = TERMINAL_VELOCITY;
    }

    // Predict next position
    glm::vec3 nextGPosition = *this->getNextTranslate(idx);
    glm::vec3 prevGPosition = *this->getTranslate(idx);
    glm::vec3 nextPosition = *this->getNextTranslate(idx);
    glm::vec3 prevPosition = *this->getTranslate(idx);
    if (gravityEnable) {
        nextGPosition += this->velocity;
        prevGPosition += this->velocity;
    }

    bool thisInMovement = true;//(*getNextTranslate()) != (*getTranslate());
    // Check terrain collision
    if (gravityEnable && prevGPosition.y < terrainY) {
        prevGPosition.y = terrainY;
    }
    if (gravityEnable && nextGPosition.y < terrainY) {
        nextGPosition.y = terrainY;
        this->velocity.y = 0.0f;
    }
    setNextTranslate(&nextGPosition, idx);

    glm::vec3 yPos;
    // Check collisions with objects
    yPos = glm::vec3(0);
    for (int i = 0; i < models.size(); i++) {
        Model *other = models[i];
        for (int j = 0; j < other->getModelAttributes()->size(); j++) {
            if (this != other && this->colisionaCon(other->getModelAttributes()->at(j), yPos, thisInMovement, idx)) {
    //            bool objInMovement = (*other->getNextTranslate()) != (*other->getTranslate());
                // If colliding, place object on top of the other object
    //            glm::vec3 &otherPos = objInMovement ? *other->getNextTranslate() : *other->getTranslate();
    //            nextPosition.y = otherPos.y + other->AABBsize.m_halfHeight + this->AABBsize.m_halfHeight / 2;
                this->velocity.y = 0.0f;  // Stop downward movement
                ejeColision.x = 1;
                ejeColision.z = 1;
                collide.model = other;
                collide.attrIdx = j;
                if (!other->walkeable){
                    setNextTranslate(getTranslate(idx), idx);
                    setNextRotX(getRotX(idx), idx);
                    setNextRotY(getRotY(idx), idx);
                    setNextRotZ(getRotZ(idx), idx);
                    break;
                }
                if (nextGPosition.y > (yPos.y * 0.90)){
                    nextGPosition.y = yPos.y;
                    setNextTranslate(&nextGPosition, idx);
                    ejeColision.y = 1;
                    break;
                }
                setNextTranslate(&nextPosition, idx);
                if (!this->colisionaCon(other->getModelAttributes()->at(j), yPos, thisInMovement, idx)) {
                    break;
                }
                if (nextPosition.y > (yPos.y * 0.90)) {
                    nextPosition.y = yPos.y;
                    setNextTranslate(&nextPosition, idx);
                    ejeColision.y = 1;
                    break;
                }
                setNextTranslate(&prevGPosition, idx);
                if (!this->colisionaCon(other->getModelAttributes()->at(j), yPos, thisInMovement, idx)) {
                    break;
                }
                if (prevGPosition.y > (yPos.y * 0.90)) {
                    prevGPosition.y = yPos.y;
                    setNextTranslate(&prevGPosition, idx);
                    ejeColision.y = 1;
                    break;
                }
                setNextTranslate(&prevPosition, idx);
                if (!this->colisionaCon(other->getModelAttributes()->at(j), yPos, thisInMovement, idx)){
                    break;
                }
                setNextTranslate(getTranslate(idx), idx);
                setNextRotX(getRotX(idx), idx);
                setNextRotY(getRotY(idx), idx);
                setNextRotZ(getRotZ(idx), idx);
                break;
            }
        }
        if (collide.model != NULL)
            break;
        if (i < 0) i = 0;
    }

    // Apply final position
    this->setTranslate(getNextTranslate(idx), idx);
    setRotX(getNextRotX(idx), idx);
    setRotY(getNextRotY(idx), idx);
    setRotZ(getNextRotZ(idx), idx);
    return collide;
}

std::vector<ModelAttributes>* Model::getModelAttributes(){
    return &this->attributes;
}


void Model::buildKDtree() {
    Model* AABB = (Model*)this->getModelAttributes()->at(0).hitbox;
	if (AABB != NULL)
		delete AABB;
    // Creamos el cubo AABB apartir del arbol de puntos del modelo cargado
    Node &head = AABBsize;
    KDTree::setHeadVariables(&head,meshes);
    buildCollider(head.m_center.x, head.m_center.y, head.m_center.z, head.m_halfWidth, head.m_halfHeight, head.m_halfDepth);
}

void Model::buildCollider(float x, float y, float z, float halfWidth, float halfHeight, float halfDepth){
    vector<Vertex> cuboAABB = init_cube(x, y, z, halfWidth, halfHeight, halfDepth);
    vector<unsigned int> cuboIndex = getCubeIndex();
    ModelAttributes& attr = this->getModelAttributes()->at(0);
    attr.hitbox = new Model(cuboAABB, cuboAABB.size(), cuboIndex, cuboIndex.size(), this->cameraDetails);
    ((Model*)attr.hitbox)->name = this->name;
    for (Mesh *m : ((Model*)attr.hitbox)->meshes)
        m->VBOGLDrawType = GL_LINE_LOOP;
}

void Model::loadMaterial(vector<Material> &m, aiMaterial* mat) {
    Material material;
    ZeroMemory(&material, sizeof(material));
    aiColor3D color(0.f, 0.f, 0.f);
    float shininess;
    bool matFound = false;
    if (!mat->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
        material.Diffuse = glm::vec3(color.r, color.g, color.b);
        matFound = true;
        material.hasDiffuse = true;
    }
    else material.hasDiffuse = false;

    if (!mat->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
        material.Ambient = glm::vec3(color.r, color.g, color.b);
        matFound = true;
        material.hasAmbient = true;
    }
    else material.hasAmbient = false;

    if (!mat->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
        material.Specular = glm::vec3(color.r, color.g, color.b);
        matFound = true;
        material.hasSpecular = true;
    }
    else material.hasSpecular = false;

    if (!mat->Get(AI_MATKEY_SHININESS, shininess)) {
        material.Shininess = shininess;
        matFound = true;
        material.hasShininess = true;
    }
    else material.hasShininess = false;
    if (matFound) {
        m.emplace_back(material);
    }
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(string const& path, bool rotationX, bool rotationY)
{
    // read file via ASSIMP
    Assimp::Importer importer;
//    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    const aiScene* scene = importer.ReadFile(path, ASSIMP_READFILE);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        string err("ERROR::ASSIMP:: ");
        err.append(importer.GetErrorString());
        INFO(err, "ERROR LOAD OBJ");
        throw Model_Exception();
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));
    name.assign(path.substr(path.find_last_of('/') + 1));
    name.erase(name.find_last_of('.'));
    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene, rotationX, rotationY);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode* node, const aiScene* scene, bool rotationX, bool rotationY)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, rotationX, rotationY);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, rotationX, rotationY);
    }

}

void Model::processMesh(aiMesh* mesh, const aiScene* scene, bool rotationX, bool rotationY)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    vector<Material> materials;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        SetVertexBoneDataToDefault(vertex);
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            if (mesh->HasTangentsAndBitangents() && mesh->mTangents != NULL) {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
            }
            // bitangent
            if (mesh->HasTangentsAndBitangents() && mesh->mBitangents != NULL) {
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.emplace_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.emplace_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    loadMaterial(materials, material);
    loadMaterialTextures(textures, material, aiTextureType_DIFFUSE, "texture_diffuse", scene, rotationX, rotationY);
    // 2. specular maps
    loadMaterialTextures(textures, material, aiTextureType_SPECULAR, "texture_specular", scene, rotationX, rotationY);
    // 3. base maps
    loadMaterialTextures(textures, material, aiTextureType_BASE_COLOR, "texture_base", scene, rotationX, rotationY);
    // 4. metalness maps
    loadMaterialTextures(textures, material, aiTextureType_METALNESS, "texture_metalness", scene, rotationX, rotationY);
    // 5. roughness maps
    loadMaterialTextures(textures, material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness", scene, rotationX, rotationY);
    // 6. emissive maps
    loadMaterialTextures(textures, material, aiTextureType_EMISSIVE, "texture_emissive", scene, rotationX, rotationY);
    // 7. normals maps
    loadMaterialTextures(textures, material, aiTextureType_NORMALS, "texture_normals", scene, rotationX, rotationY);
    // 8. normal maps
    loadMaterialTextures(textures, material, aiTextureType_HEIGHT, "texture_normal", scene, rotationX, rotationY);
    // 9. height maps
    loadMaterialTextures(textures, material, aiTextureType_AMBIENT, "texture_height", scene, rotationX, rotationY);
    //10. UNKNOWN
    loadMaterialTextures(textures, material, aiTextureType_UNKNOWN, "texture_unknown", scene, rotationX, rotationY);

    ExtractBoneWeightForVertices(vertices,mesh,scene);
    // return a mesh object created from the extracted mesh data
    meshes.emplace_back(new Mesh(vertices, indices, textures, materials));
    if (textures_loaded.capacity() < textures.size())
        textures_loaded.reserve(textures_loaded.capacity() + textures.size());
    for (int i = 0; i < textures.size(); i++)
        textures_loaded.emplace_back(&meshes.back()->textures.data()[i]);
    if (material_loaded.capacity() < materials.size())
        material_loaded.reserve(material_loaded.capacity() + materials.size());
    for (int i = 0; i < materials.size(); i++)
        material_loaded.emplace_back(&meshes.back()->materials.data()[i]);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
void Model::loadMaterialTextures(vector<Texture> &textures, aiMaterial* mat, aiTextureType type, string typeName, const aiScene *scene, bool rotationX, bool rotationY)
{
    if (textures.capacity() < mat->GetTextureCount(type))
        textures.reserve(textures.capacity() + mat->GetTextureCount(type));
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        if (mat->GetTexture(type, i, &str) != AI_SUCCESS){
            std::string name = str.C_Str();
            WARNING("Error at reading texture "+ name, "Model loading");
            continue;
        }
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
            if (std::strcmp(textures_loaded[j]->path, str.C_Str()) == 0) {
                textures.emplace_back(*(textures_loaded[j]));
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        for (unsigned int j = 0; j < textures.size(); j++)
            if (std::strcmp(textures[j].path, str.C_Str()) == 0) {
                textures.emplace_back(textures[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        if (!skip) {   // if texture hasn't been loaded already, load it
            Texture texture;
            const aiTexture *paiTexture = scene->GetEmbeddedTexture(str.C_Str());
            texture.id = paiTexture != NULL ? TextureFromMemory(paiTexture, rotationX, rotationY) :
                                TextureFromFile(str.C_Str(), this->directory, rotationX, rotationY);
            strcpy_s(texture.type, 255, typeName.c_str());
            strcpy_s(texture.path, 1024, str.C_Str());
            textures.emplace_back(texture);
        }
    }
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight){
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i){
        if (vertex.m_BoneIDs[i] == boneID || weight == 0.0f)
            return;
        if (vertex.m_BoneIDs[i] == -1){
            vertex.m_Weights[i] = weight;
            vertex.m_BoneIDs[i] = boneID;
            return;
        }
    }
    ERRORL("This model has more bone influence than expected " + to_string(MAX_BONE_INFLUENCE), "Load model "+this->name);
}

void Model::ExtractBoneWeightForVertices(vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene){
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex){
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()){
            BoneInfo newBoneInfo;
            newBoneInfo.id = m_BoneCounter;
            newBoneInfo.offset = UTILITIES_OGL::aiMatrix4x4ToGlm(mesh->mBones[boneIndex]->mOffsetMatrix);
            bonesInfo.emplace_back(newBoneInfo);
            m_BoneInfoMap[boneName] = bonesInfo.size() - 1;
            boneID = m_BoneCounter;
            m_BoneCounter++;
        } else {
            boneID = bonesInfo[m_BoneInfoMap[boneName]].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex){
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

bool Model::colisionaCon(ModelAttributes& objeto, glm::vec3 &yPos, bool collitionMove, int idx) {
    return Model::colisionaCon(this->getModelAttributes()->at(idx), objeto, yPos, collitionMove);
}
bool Model::colisionaCon(ModelAttributes& objeto0, ModelAttributes& objeto, glm::vec3 &yPos, bool collitionMove) {
    bool collide = false;
    if (objeto0.hitbox == NULL || objeto.hitbox == NULL)
        return false;
    if (!(objeto0.active && objeto.active))
        return false;
    Model* AABB = (Model*)objeto.hitbox;
    Model* AABB0 = (Model*)objeto0.hitbox;
    // Obtener las matrices de transformación para ambos modelos
    // collitionMove sirve para saber si el modelo principal a comparar va avanzar(true)
    // o esta quieto(false)
    glm::mat4 transform1 = collitionMove ? AABB0->makeTransScaleNextPosition(glm::mat4(1)) : AABB0->makeTransScale(glm::mat4(1)) ; // Para el cubo A
    // Asumimos que el modelo a comparar esta quieto y no se esta moviendo
    glm::mat4 transform2 = collitionMove ? AABB->makeTransScaleNextPosition(glm::mat4(1)) : AABB->makeTransScale(glm::mat4(1)); // Para el cubo B
    if (collitionMove) {
        glm::vec3 prev = *AABB0->getTranslate();      // posición actual
        glm::vec3 next = *AABB0->getNextTranslate();  // posición siguiente
        glm::vec3 dir  = next - prev;
        float dist = glm::length(dir);

        if (dist > 0.0001f) {
            // Determinar cantidad de subpasos dinámicamente
            // (cada 0.1 unidades de distancia = 1 substep, hasta 10 máx)
            int steps = glm::clamp(int(dist / 0.1f), 1, 10);
            glm::vec3 stepDir = dir / float(steps);

            for (int s = 0; s < steps; ++s) {
                glm::vec3 partialPos = prev + stepDir * float(s);
                float tMin, tMax;

                // Test de ray vs OBB con el modelo de destino
                if (rayIntersectsOBB(partialPos, stepDir, AABB->meshes.at(0)->vertices, transform2, tMin, tMax)) {
                    if (tMin <= 1.0f && tMax >= 0.0f) {
                        collide = true; // colisión durante el trayecto
                        break;
                    }
                }
            }
        }
    }

    // Obtener los vértices de ambos cubos AABB
//    vector<Vertex> verticesCubo1 = objeto0.AABB->meshes[0]->vertices;
    Vertex verticesCubo1[24];
//    vector<Vertex> verticesCubo2 = objeto.AABB->meshes[0]->vertices;
    Vertex verticesCubo2[24];

    // Transformar los vértices de cada cubo usando sus respectivas matrices de transformación
    Vertex *idx = verticesCubo1;
    yPos.z = AABB0->meshes[0]->vertices[0].Position.y;
    for (Vertex& vertex : AABB0->meshes[0]->vertices) {
        idx->Position = glm::vec3(transform1 * glm::vec4(vertex.Position, 1.0f));
        if (yPos.z > idx->Position.y) // Min cube position
            yPos.z = idx->Position.y;
        idx++;
    }
    idx = verticesCubo2;
    yPos.x = AABB->meshes[0]->vertices[0].Position.y;
    yPos.y = AABB->meshes[0]->vertices[0].Position.y;
    for (Vertex& vertex : AABB->meshes[0]->vertices) {
        idx->Position = glm::vec3(transform2 * glm::vec4(vertex.Position, 1.0f));
        if (yPos.x > idx->Position.y) // Min cube position
            yPos.x = idx->Position.y;
        if (yPos.y < idx->Position.y) // Max cube position
            yPos.y = idx->Position.y;
        idx++;
    }
    if (collide)
        return collide;

    // Obtener los ejes de separación
//            std::vector<glm::vec3> ejes = obtenerEjesSeparacion(transform1, transform2);
    glm::vec3 ejes[15];
    obtenerEjesSeparacion(ejes, transform1, transform2);
    // Verificar si las proyecciones de los cubos se solapan en cada eje
    for (int i = 0; i < 15; i++) {
        if (!proyectarYComprobarSolapamiento(verticesCubo1, verticesCubo2, ejes[i])) {
            return false; // No hay solapamiento en este eje, no hay colisión
        }
    }

    // Si las proyecciones se solapan en todos los ejes, hay colisión
    return true;
}

void Model::setCleanTextures(bool flag){
    cleanTextures = flag;
}