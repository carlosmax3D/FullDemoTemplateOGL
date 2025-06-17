#pragma once
#ifndef ANIMATOR_H
#define ANIMATOR_H
#include "Utilities.h"
#include "Animation.h"

class Animator {	
public:
    Animator();
    ~Animator();

    Animator(Animation& Animation);
	
    void UpdateAnimation(float dt, glm::mat4 parentTransform = glm::mat4(1.0f));
	
    void PlayAnimation(Animation& pAnimation);
	void PlayAnimation();

    void CalculateBoneTransform(const UTILITIES_OGL::AssimpNodeData* node, const glm::mat4& parentTransform);
	
    const glm::mat4* GetFinalBoneMatrices() ;

    void setAnimation(Animation& Animation);
    Animation &getAnimation();

private:
    glm::mat4 m_FinalBoneMatrices[MAX_MODEL_BONES];
    Animation m_CurrentAnimation;
    double m_CurrentTime = 0.0;
    double m_DeltaTime = 0.0;
};
#endif