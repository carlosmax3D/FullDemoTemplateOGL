#include "Animator.h"

Animator::Animator(){}

Animator::Animator(Animation& Animation){
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
	
void Animator::PlayAnimation() {
    m_CurrentTime = 0.0;
}
void Animator::PlayAnimation(Animation& pAnimation) {
    m_CurrentAnimation = pAnimation;
    m_CurrentTime = 0.0;
}
	
void Animator::CalculateBoneTransform(const UTILITIES_OGL::AssimpNodeData* node, const glm::mat4& parentTransform) {
    // Precompute references for faster access
    glm::mat4& transInv = m_CurrentAnimation.GetRootNode().invTransformation;
    std::vector<UTILITIES_OGL::AssimpNodeData>& nodes = m_CurrentAnimation.GetNodes();
    BoneInfo* bonesInfo = m_CurrentAnimation.getBonesInfo() ? m_CurrentAnimation.getBonesInfo()->data() : nullptr;
    
    size_t nodeCount = nodes.size();
    if (nodeCount == 0) return; // Early exit if no nodes exist

    // Pointer to the first node in the vector (avoids indexing overhead)
    UTILITIES_OGL::AssimpNodeData* nodePtr = nodes.data();
    for (size_t i = 0; i < nodeCount; ++i, ++nodePtr) {
        // Fetch node data directly using pointer arithmetic
        Bone* bone = m_CurrentAnimation.getBone(nodePtr->idxBone);

        // Compute transformation
        const glm::mat4& nodeTransformation = bone ? bone->Update(m_CurrentTime) : nodePtr->transformation;
        nodes[i].tmpTransformation = ((i == 0) ? parentTransform : nodes[nodePtr->parent].tmpTransformation) * nodeTransformation;

        // Skip if boneInfo is not valid
        if (nodePtr->boneInfo == -1) continue;

        // Fetch bone info using pointer arithmetic
        BoneInfo& boneInfo = bonesInfo[nodePtr->boneInfo];
        m_FinalBoneMatrices[boneInfo.id] = transInv * nodes[i].tmpTransformation * boneInfo.offset;
    }
}
	
const glm::mat4* Animator::GetFinalBoneMatrices() { 
    return &m_FinalBoneMatrices[0];
}

void Animator::setAnimation(Animation& Animation){
    m_CurrentTime = 0.0;
    m_CurrentAnimation = Animation;

    for (int i = 0; i < MAX_MODEL_BONES; i++)
        m_FinalBoneMatrices[i] = glm::mat4(0.0f);
}

Animation &Animator::getAnimation(){
    return m_CurrentAnimation;
}