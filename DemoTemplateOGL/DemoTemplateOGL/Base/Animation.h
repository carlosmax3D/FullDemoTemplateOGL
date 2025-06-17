#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H
#include "Utilities.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
#include <string.h>

#include "Bone.h"

class Animation_Exception : std::exception {
    public:
        Animation_Exception();
        const char* what();
};

class Animation {
public:
    Animation();

    Animation(const aiScene* scene, std::unordered_map<std::string, int>* boneInfoMap, std::vector<BoneInfo>* bonesInfo, int& boneCount, int idx = 0);
    Animation(std::string const& animationPath, std::unordered_map<std::string, int>* boneInfoMap, std::vector<BoneInfo>* bonesInfo, int& boneCount, int idx = 0);
    ~Animation();
    void loadFromScene(const aiScene* scene, std::unordered_map<std::string, int>* boneInfoMap, std::vector<BoneInfo>* bonesInfo, int& boneCount, int idx = 0);
    Bone* FindBone(std::string const& name);
    Bone* getBone(int idx);
    int getIdxBone(std::string const& name);

    double GetTicksPerSecond();
    double GetDuration();
    UTILITIES_OGL::AssimpNodeData& GetRootNode();
    std::vector<UTILITIES_OGL::AssimpNodeData>& GetNodes();
    std::unordered_map<std::string,int>* GetBoneIDMap();
    std::vector<BoneInfo>* getBonesInfo();
    static std::vector<Animation> loadAllAnimations(std::string const& animationPath, std::unordered_map<std::string, int>* boneInfoMap, std::vector<BoneInfo>* bonesInfo, int& boneCount);
    void setBoneInfoMap(std::unordered_map<std::string, int> *m_BoneInfoMap);
    void setBonesInfo(std::vector<BoneInfo> *bonesInfo);

private:
    void ReadMissingBones(const aiAnimation* animation, int& boneCount, const aiScene *scene);
    void ReadHeirarchyData(int parent, const aiNode* src, const aiScene *scene);

    double m_Duration = 0;
    double m_TicksPerSecond = 0;
    std::vector<Bone> m_Bones;
    std::vector<UTILITIES_OGL::AssimpNodeData> m_RootNode;
    std::unordered_map<std::string, int> *m_BoneInfoMap = NULL;
    std::vector<BoneInfo> *bonesInfo = NULL;
    std::string name;
};

#endif