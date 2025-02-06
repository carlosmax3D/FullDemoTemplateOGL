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

    Animation(std::string const& animationPath, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCount);
    ~Animation();
    Bone* FindBone(std::string const& name);
	
    double GetTicksPerSecond();
    double GetDuration();
    UTILITIES_OGL::AssimpNodeData& GetRootNode();
    std::unordered_map<std::string,BoneInfo>& GetBoneIDMap();

private:
    void ReadMissingBones(const aiAnimation* animation, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCount, const aiScene *scene);
    void ReadHeirarchyData(UTILITIES_OGL::AssimpNodeData& dest, const aiNode* src, const aiScene *scene);

    double m_Duration = 0;
    double m_TicksPerSecond = 0;
    std::vector<Bone> m_Bones;
    UTILITIES_OGL::AssimpNodeData m_RootNode;
    std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
};

#endif