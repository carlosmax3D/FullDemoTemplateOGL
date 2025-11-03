#ifndef MODEL_H
#define MODEL_H

#include "Utilities.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "material.h"
#include "mesh.h"
#include "shader.h"
#include "../KDTree/CollitionDetection.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "camera.h"
#include "Animator.h"
using namespace std;

class Model_Exception : std::exception {
    public:
        Model_Exception();
        const char* what();
};
class Model {
private:
    std::vector<ModelAttributes> attributes;
    std::unordered_map<string, int> m_BoneInfoMap;
    std::vector<BoneInfo> bonesInfo;
    int m_BoneCounter = 0;
	std::vector<Animator> animators;
    std::string modelType;
    int animatorIdx = -1;
    bool cleanTextures = true;
    bool defaultShader = false;
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

public:
    // model data 
    Camera* cameraDetails = NULL;
    Node AABBsize;
    bool ignoreAABB = false;
    bool walkeable = true;

    vector<Texture*> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Material*> material_loaded;	// stores all the materials.
    vector<Mesh*>    meshes;
    string directory;
    bool gammaCorrection;
    Shader* gpuDemo = NULL;
    string name;
    glm::vec3 lightColor = glm::vec3(3.0f,3.0f,3.0f);
    glm::vec3 lightPos = glm::vec3(100.2f, 100.0f, 100.0f);

    // constructor, expects a filepath to a 3D model.
    Model();
    Model(string const& path, Camera* camera, bool rotationX = false, bool rotationY = true, bool gamma = false);
    Model(vector<Vertex>& vertices, unsigned int numVertices, vector<unsigned int>& indices, unsigned int numIndices, Camera* camera);
    Model(string const& path, glm::vec3& actualPosition, Camera* cam, bool rotationX = false, bool rotationY = true, bool gamma = false);
    virtual ~Model();

    void setModelType(const std::string& type) { modelType = type; }
    const std::string& getModelType() const { return modelType; }

    // draws the model, and thus all its meshes
    virtual void prepShader(Shader& gpuDemo, ModelAttributes &attributes);
    virtual void Draw();
    virtual void Draw(Shader& shader, int attribute);
    glm::mat4 makeTransScale(const glm::mat4& prevTransformations, int idx = 0) const;
    glm::mat4 makeTrans(int idx = 0) const;
    glm::mat4 makeTransNextPosition(int idx = 0);
    glm::mat4 makeTransScaleNextPosition(const glm::mat4& prevTransformations, int idx = 0);
    bool getDefaultShader();
    void setDefaultShader(bool defaultShader);

    void setVelocity(glm::vec3* velocity);
    glm::vec3* getVelocity();
    virtual void setNextTranslate(glm::vec3* translate, int idx = 0);
    virtual void setTranslate(glm::vec3* translate, int idx = 0);
    void setScale(glm::vec3* scale, int idx = 0);
    void setRotX(float rotationAngle, int idx = 0);
    void setRotY(float rotationAngle, int idx = 0);
    void setRotZ(float rotationAngle, int idx = 0);
    glm::vec3* getTranslate(int idx = 0);
    glm::vec3* getNextTranslate(int idx = 0);
    glm::vec3* getScale(int idx = 0);
    float getRotX(int idx = 0);
    float getRotY(int idx = 0);
    float getRotZ(int idx = 0);
    glm::vec3* getRotationVector(int idx = 0);
    float getNextRotX(int idx = 0);
    float getNextRotY(int idx = 0);
    float getNextRotZ(int idx = 0);
    glm::vec3* getNextRotationVector(int idx = 0);
    bool getActive(int idx = 0);
    void setNextRotX(float rotationAngle, int idx = 0);
    void setNextRotY(float rotationAngle, int idx = 0);
    void setNextRotZ(float rotationAngle, int idx = 0);
    void setActive(bool active, int idx = 0);

    void buildKDtree();
    void buildCollider(float x, float y, float z, float halfWidth, float halfHeight, float halfDepth);
    bool colisionaCon(ModelAttributes& objeto, glm::vec3 &yPos, bool collitionMove = false, int idx = 0);
    bool nodoColisionCon(Model& objeto, std::pair<Node*, Node*>& nodeCollitions, bool collitionMove = false, int idx = 0);
    static bool colisionaCon(ModelAttributes& objeto0, ModelAttributes& objeto, glm::vec3 &yPos, bool collitionMove = false);

    std::unordered_map<string, int>* GetBoneInfoMap();
    std::vector<BoneInfo>* getBonesInfo();
    int& GetBoneCount();
    void setAnimator(Animator animator);
    void setAnimator(std::vector<Animator>& animator);
    std::vector<Animator>& getAnimator();
    void setAnimation(unsigned int id);
    void setCleanTextures(bool flag);
    std::vector<ModelAttributes>* getModelAttributes();
    virtual ModelCollider update(float terrainY, std::vector<Model*>& models, glm::vec3 &ejeColision, bool gravityEnable = false, int idx = 0);
private:
    void loadMaterial(vector<Material> &m, aiMaterial* mat);
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path, bool rotationX = false, bool rotationY = true);
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene, bool rotationX = false, bool rotationY = true);
    void processMesh(aiMesh* mesh, const aiScene* scene, bool rotationX = false, bool rotationY = true);
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    void loadMaterialTextures(vector<Texture> &textures, aiMaterial* mat, aiTextureType type, string typeName, const aiScene *scene, bool rotationX = false, bool rotationY = true);
    void SetVertexBoneDataToDefault(Vertex& vertex);
    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
    void ExtractBoneWeightForVertices(vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

};

#endif