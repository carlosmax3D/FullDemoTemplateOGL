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

class Model {
private:
    std::list<Node::vecType> point_list;
    bool hasTranslate = false;
    glm::vec3 translate = glm::vec3(0.0f, 0.0f, 0.0f);
    bool hasScale = false;
    glm::vec3 scale = glm::vec3(0.0f, 0.0f, 0.0f);
    float rotX = 0;
    float rotY = 0;
    float rotZ = 0;
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    bool defaultShader = false;;
    glm::vec3 nextTranslate = glm::vec3(0.0f, 0.0f, 0.0f);
    float nextRotX = 0;
    float nextRotY = 0;
    float nextRotZ = 0;
    glm::vec3 nextRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    std::unordered_map<string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;
	Animator* animator = NULL;
    bool cleanTextures = true;

    void SetVertexBoneDataToDefault(Vertex& vertex);
    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
    void ExtractBoneWeightForVertices(vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

public:
    // model data 
    Camera* cameraDetails = NULL;
    Model* AABB = NULL; // Modelo que alberga un cubo para colisiones

    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Material> material_loaded;	// stores all the materials.
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
    ~Model();
    // draws the model, and thus all its meshes
    void prepShader(Shader& gpuDemo);
    void Draw();
    void Draw(Shader& shader);
    glm::mat4 makeTransScale(const glm::mat4& prevTransformations) const;
    glm::mat4 makeTrans() const;
    glm::mat4 makeTransNextPosition();
    glm::mat4 makeTransScaleNextPosition(const glm::mat4& prevTransformations);
    bool getDefaultShader();
    void setDefaultShader(bool defaultShader);

    void setNextTranslate(glm::vec3* translate);
    void setTranslate(glm::vec3* translate);
    void setScale(glm::vec3* scale);
    void setRotX(float rotationAngle);
    void setRotY(float rotationAngle);
    void setRotZ(float rotationAngle);
    glm::vec3* getTranslate();
    glm::vec3* getNextTranslate();
    glm::vec3* getScale();
    float getRotX();
    float getRotY();
    float getRotZ();
    glm::vec3* getRotationVector();
    float getNextRotX();
    float getNextRotY();
    float getNextRotZ();
    glm::vec3* getNextRotationVector();
    void setNextRotX(float rotationAngle);
    void setNextRotY(float rotationAngle);
    void setNextRotZ(float rotationAngle);

    void buildKDtree();
    bool colisionaCon(Model& objeto, bool collitionMove = false);
    bool nodoColisionCon(Model& objeto, std::pair<Node*, Node*>& nodeCollitions, bool collitionMove = false);

    std::unordered_map<string, BoneInfo>& GetBoneInfoMap();
    int& GetBoneCount();
    void setAnimator(Animator *animator);
    void setCleanTextures(bool flag);
private:
    vector<Vertex> init_cube(float x, float y, float z, float width, float height, float depth);
    vector<unsigned int> getCubeIndex();
    vector<Material> loadMaterial(aiMaterial* mat);
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path, bool rotationX = false, bool rotationY = true);
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene, bool rotationX = false, bool rotationY = true);
    void processMesh(aiMesh* mesh, const aiScene* scene, bool rotationX = false, bool rotationY = true);
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, bool rotationX = false, bool rotationY = true);
};

#endif