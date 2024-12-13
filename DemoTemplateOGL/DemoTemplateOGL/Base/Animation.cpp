#include "Animation.h"

Animation_Exception::Animation_Exception(){ }
const char* Animation_Exception::what(){
    return "Failed at load the animation sequence";
}

std::vector<Animation> Animation::loadAllAnimations(std::string const& animationPath, std::unordered_map<std::string, int>* boneInfoMap, std::vector<BoneInfo>* bonesInfo, int& boneCount){
    Assimp::Importer importer;
    std::vector<Animation> animations;
//    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    const aiScene* scene = importer.ReadFile(animationPath, ASSIMP_READFILE);
    if (scene && scene->mRootNode && scene->mNumAnimations > 0) {
        animations.reserve(scene->mNumAnimations);
        for (int i = 0; i < scene->mNumAnimations; i++){
            animations.emplace_back(scene, boneInfoMap, bonesInfo, boneCount, i);
        }
    } else throw Animation_Exception();
    return animations;
}

Animation::Animation() {};

Animation::Animation(const aiScene* scene, std::unordered_map<std::string, int>* boneInfoMap, std::vector<BoneInfo>* bonesInfo, int& boneCount, int idx) {
    loadFromScene(scene, boneInfoMap, bonesInfo, boneCount, idx);
}

Animation::Animation(std::string const& animationPath, std::unordered_map<std::string, int>* boneInfoMap, std::vector<BoneInfo>* bonesInfo, int& boneCount, int idx) {
    Assimp::Importer importer;
//    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    const aiScene* scene = importer.ReadFile(animationPath, ASSIMP_READFILE);
    loadFromScene(scene, boneInfoMap, bonesInfo, boneCount, idx);
}

Animation::~Animation() { }

void Animation::loadFromScene(const aiScene* scene, std::unordered_map<std::string, int>* boneInfoMap, std::vector<BoneInfo>* bonesInfo, int& boneCount, int idx){
    if (scene && scene->mRootNode && scene->mNumAnimations > 0) {
        this->m_BoneInfoMap = boneInfoMap;
        this->bonesInfo = bonesInfo;
        aiAnimation* animation = scene->mAnimations[idx];
        m_Duration = animation->mDuration;
        m_TicksPerSecond = animation->mTicksPerSecond;
        ReadMissingBones(animation, boneCount, scene);
        ReadHeirarchyData(m_RootNode, scene->mRootNode, scene);
        this->name = animation->mName.C_Str();
    } else throw Animation_Exception();
}

Bone* Animation::FindBone(std::string const& name) {
    auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
            [&](Bone& bone){
                return bone.GetBoneName().compare(name) == 0;
            }
        );
        if (iter == m_Bones.end()) return nullptr;
        else return &(*iter);
}

Bone* Animation::getBone(int idx){
    if (idx == -1 || idx >= m_Bones.size())
        return NULL;
    return &m_Bones[idx];
}


int Animation::getIdxBone(std::string const& name){
    for (int i = 0; i < m_Bones.size(); i++){
        if (m_Bones[i].GetBoneName().compare(name) == 0)
            return i;
    }
    return -1;
}

double Animation::GetTicksPerSecond() { return m_TicksPerSecond; }

double Animation::GetDuration() { return m_Duration;}

UTILITIES_OGL::AssimpNodeData& Animation::GetRootNode() { return m_RootNode; }

std::unordered_map<std::string,int>* Animation::GetBoneIDMap() { 
    return m_BoneInfoMap;
}

std::vector<BoneInfo>* Animation::getBonesInfo(){
    return bonesInfo;
}

void Animation::ReadMissingBones(const aiAnimation* animation, int& boneCount, const aiScene *scene) {
    int size = animation->mNumChannels;

//    std::map<std::string, BoneInfo>& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
//    int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

    //reading channels(bones engaged in an animation and their keyframes)
    m_Bones.reserve(size);
    for (int i = 0; i < size; i++) {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (m_BoneInfoMap->find(boneName) == m_BoneInfoMap->end()) {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            bonesInfo->emplace_back(newBoneInfo);
            (*m_BoneInfoMap)[boneName] = bonesInfo->size() - 1;
            boneCount++;
        }
        m_Bones.emplace_back(channel->mNodeName.data,
            (*m_BoneInfoMap)[channel->mNodeName.data], channel);
    }
}

void Animation::ReadHeirarchyData(UTILITIES_OGL::AssimpNodeData& dest, const aiNode* src, const aiScene *scene) {
    assert(src);

    dest.name = src->mName.data;
    aiMatrix4x4 mat = src->mTransformation;
    dest.transformation = UTILITIES_OGL::aiMatrix4x4ToGlm(mat);
    dest.invTransformation = UTILITIES_OGL::aiMatrix4x4ToGlm(mat.Inverse());
    dest.childrenCount = src->mNumChildren;
    dest.children.reserve(src->mNumChildren);
    dest.idxBone = getIdxBone(dest.name);
    auto boneInfo = this->m_BoneInfoMap->find(dest.name);
    dest.boneInfo = boneInfo != this->m_BoneInfoMap->end() ? boneInfo->second : -1;
    for (unsigned int i = 0; i < src->mNumChildren; i++) {
        UTILITIES_OGL::AssimpNodeData newData;
        ReadHeirarchyData(newData, src->mChildren[i], scene);
        dest.children.emplace_back(newData);
    }
}
