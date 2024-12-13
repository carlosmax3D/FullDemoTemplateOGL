#include "Animator.h"

Animator::Animator(){}

Animator::Animator(Animation Animation){
    setAnimation(Animation);
}

Animator::~Animator(){
}
	
void Animator::UpdateAnimation(float dt, glm::mat4 parentTransform) {
    m_DeltaTime = dt;
    float ticksPerSecond = m_CurrentAnimation.GetTicksPerSecond() != 0 ?
                            m_CurrentAnimation.GetTicksPerSecond() : 25.0f;
    m_CurrentTime += ticksPerSecond * dt;
    m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation.GetDuration());
    CalculateBoneTransform(&m_CurrentAnimation.GetRootNode(), parentTransform);
}
	
void Animator::PlayAnimation(Animation pAnimation) {
    m_CurrentAnimation = pAnimation;
    m_CurrentTime = 0.0;
}
	
void Animator::CalculateBoneTransform(const UTILITIES_OGL::AssimpNodeData* node, const glm::mat4& parentTransform) {
//    std::string nodeName = node->name;
    glm::mat4 &transInv = m_CurrentAnimation.GetRootNode().invTransformation;
    glm::mat4 &transfor = m_CurrentAnimation.GetRootNode().transformation;
    Bone* bone = m_CurrentAnimation.getBone(node->idxBone);
    if (bone)
        bone->Update(m_CurrentTime);
    const glm::mat4 &nodeTransformation = (const glm::mat4)(bone != NULL ? bone->GetLocalTransform() : node->transformation);
    glm::mat4 globalTransformation = parentTransform * nodeTransformation;

/*    auto boneInfoMap = m_CurrentAnimation.GetBoneIDMap();
    auto boneInfo = boneInfoMap.find(nodeName);
    if (boneInfo != boneInfoMap.end()) {*/
    if (node->boneInfo != -1){
        BoneInfo &boneInfo = (*m_CurrentAnimation.getBonesInfo())[node->boneInfo];
        int &index = boneInfo.id ;//&(*boneInfo).second.id;
        glm::mat4 &offset = boneInfo.offset;//(*boneInfo).second.offset;
        m_FinalBoneMatrices[index] = transInv * globalTransformation * offset;
    }
	
    for (int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}
	
std::vector<glm::mat4>* Animator::GetFinalBoneMatrices() { 
    return &m_FinalBoneMatrices;  
}

void Animator::setAnimation(Animation Animation){
    m_CurrentTime = 0.0;
    m_CurrentAnimation = Animation;

    m_FinalBoneMatrices.reserve(200);

    for (int i = 0; i < 200; i++)
        m_FinalBoneMatrices.push_back(glm::mat4(0.0f));
}