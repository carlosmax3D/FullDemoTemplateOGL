#include "Animation.h"

Animation_Exception::Animation_Exception(){ }
const char* Animation_Exception::what(){
    return "Failed at load the animation sequence";
}

Animation::Animation() {};

Animation::Animation(std::string const& animationPath, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCount) {
    Assimp::Importer importer;
//    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    const aiScene* scene = importer.ReadFile(animationPath, ASSIMP_READFILE);
    if (scene && scene->mRootNode && scene->mNumAnimations > 0) {
        aiAnimation* animation = scene->mAnimations[0];
        m_Duration = animation->mDuration;
        m_TicksPerSecond = animation->mTicksPerSecond;
        ReadHeirarchyData(m_RootNode, scene->mRootNode, scene);
        ReadMissingBones(animation, boneInfoMap, boneCount, scene);
    } else throw Animation_Exception();
}

Animation::~Animation() { }

Bone* Animation::FindBone(std::string const& name) {
    for (int i = 0; i < m_Bones.size(); i++){
        if (m_Bones[i].GetBoneName().compare(name) == 0)
            return &m_Bones[i];
    }
    return NULL;
}

	
double Animation::GetTicksPerSecond() { return m_TicksPerSecond; }

double Animation::GetDuration() { return m_Duration;}

UTILITIES_OGL::AssimpNodeData& Animation::GetRootNode() { return m_RootNode; }

std::unordered_map<std::string,BoneInfo>& Animation::GetBoneIDMap() { 
    return m_BoneInfoMap;
}

void Animation::ReadMissingBones(const aiAnimation* animation, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCount, const aiScene *scene) {
    int size = animation->mNumChannels;

//    std::map<std::string, BoneInfo>& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
//    int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

    //reading channels(bones engaged in an animation and their keyframes)
    m_Bones.reserve(size);
    for (int i = 0; i < size; i++) {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        m_Bones.emplace_back(channel->mNodeName.data,
            boneInfoMap[channel->mNodeName.data].id, channel);
    }

    m_BoneInfoMap = boneInfoMap;
}

void Animation::ReadHeirarchyData(UTILITIES_OGL::AssimpNodeData& dest, const aiNode* src, const aiScene *scene) {
    assert(src);

    dest.name = src->mName.data;
    aiMatrix4x4 mat = src->mTransformation;
    dest.transformation = UTILITIES_OGL::aiMatrix4x4ToGlm(mat);
    if (scene->mRootNode == src)
        dest.invTransformation = UTILITIES_OGL::aiMatrix4x4ToGlm(mat.Inverse());
    dest.childrenCount = src->mNumChildren;
    dest.children.reserve(src->mNumChildren);
    for (unsigned int i = 0; i < src->mNumChildren; i++) {
        UTILITIES_OGL::AssimpNodeData newData;
        newData.invTransformation = dest.invTransformation;
        ReadHeirarchyData(newData, src->mChildren[i], scene);
        dest.children.emplace_back(newData);
    }
}
