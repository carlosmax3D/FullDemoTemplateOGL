#pragma once
#ifndef BONE_H
#define BONE_H
#include "Utilities.h"
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <string>
#include <assimp/anim.h>
#include <assimp/vector3.h>

class Bone {
private:
    std::vector<UTILITIES_OGL::KeyFrame> m_Positions;
    std::vector<UTILITIES_OGL::KeyFrame> m_Rotations;
    std::vector<UTILITIES_OGL::KeyFrame> m_Scales;
    int m_NumPositions;
    int m_NumRotations;
    int m_NumScalings;
	
    glm::mat4 m_LocalTransform;
    std::string m_Name;
    int m_ID;

public:

/*reads keyframes from aiNodeAnim*/
    Bone(const std::string& name, int ID, const aiNodeAnim* channel);
	
    /*interpolates  b/w positions,rotations & scaling keys based on the curren time of 
    the animation and prepares the local transformation matrix by combining all keys 
    tranformations*/
    glm::mat4& Update(double animationTime);

    glm::mat4& GetLocalTransform();
    const std::string& GetBoneName();
    int GetBoneID();
	

    /* Gets the current index on mKeyPositions to interpolate to based on 
    the current animation time*/
    int GetPositionIndex(double animationTime);

    /* Gets the current index on mKeyRotations to interpolate to based on the 
    current animation time*/
    int GetRotationIndex(double animationTime);

    /* Gets the current index on mKeyScalings to interpolate to based on the 
    current animation time */
    int GetScaleIndex(double animationTime);

private:
    /* Gets normalized value for Lerp & Slerp*/
    double GetScaleFactor(double lastTimeStamp, double nextTimeStamp, double animationTime);

    /*figures out which position keys to interpolate b/w and performs the interpolation 
    and returns the translation matrix*/
    glm::mat4 InterpolatePosition(double animationTime);

    /*figures out which rotations keys to interpolate b/w and performs the interpolation 
    and returns the rotation matrix*/
    glm::mat4 InterpolateRotation(double animationTime);

    /*figures out which scaling keys to interpolate b/w and performs the interpolation 
    and returns the scale matrix*/
    glm::mat4 InterpolateScaling(double animationTime);
};

#endif