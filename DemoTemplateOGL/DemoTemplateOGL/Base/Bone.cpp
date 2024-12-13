#include "Bone.h"

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
        : m_Name(name), m_ID(ID), m_LocalTransform(1.0f) {
    m_NumPositions = channel->mNumPositionKeys;

    m_Positions.reserve(m_NumPositions);
    for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex) {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        double timeStamp = channel->mPositionKeys[positionIndex].mTime;
        UTILITIES_OGL::KeyPosition data;
        data.position.x = aiPosition.x;
        data.position.y = aiPosition.y;
        data.position.z = aiPosition.z;
        data.timeStamp = timeStamp;
        m_Positions.emplace_back(data);
    }

    m_NumRotations = channel->mNumRotationKeys;
    m_Rotations.reserve(m_NumRotations);
    for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex) {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        double timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        UTILITIES_OGL::KeyRotation data;
        data.orientation.x = aiOrientation.x;
        data.orientation.y = aiOrientation.y;
        data.orientation.z = aiOrientation.z;
        data.orientation.w = aiOrientation.w;
        data.timeStamp = timeStamp;
        m_Rotations.emplace_back(data);
    }

    m_NumScalings = channel->mNumScalingKeys;
    m_Scales.reserve(m_NumScalings);
    for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex) {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        double timeStamp = channel->mScalingKeys[keyIndex].mTime;
        UTILITIES_OGL::KeyScale data;
        data.scale.x = scale.x;
        data.scale.y = scale.y;
        data.scale.z = scale.z;
        data.timeStamp = timeStamp;
        m_Scales.emplace_back(data);
    }
}
	
/*interpolates  b/w positions,rotations & scaling keys based on the curren time of 
the animation and prepares the local transformation matrix by combining all keys 
tranformations*/
void Bone::Update(double animationTime) {
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    m_LocalTransform = translation * rotation * scale;
}

glm::mat4 Bone::GetLocalTransform() { return m_LocalTransform; }
const std::string& Bone::GetBoneName() { return m_Name; }
int Bone::GetBoneID() { return m_ID; }
	

/* Gets the current index on mKeyPositions to interpolate to based on 
the current animation time*/
int Bone::GetPositionIndex(double animationTime) {
    for (int index = 0; index < m_NumPositions - 1; ++index) {
        if (animationTime < m_Positions[index + 1].timeStamp)
            return index;
    }
    return 0;
}

/* Gets the current index on mKeyRotations to interpolate to based on the 
current animation time*/
int Bone::GetRotationIndex(double animationTime) {
    for (int index = 0; index < m_NumRotations - 1; ++index) {
        if (animationTime < m_Rotations[index + 1].timeStamp)
            return index;
    }
    return 0;
}

/* Gets the current index on mKeyScalings to interpolate to based on the 
current animation time */
int Bone::GetScaleIndex(double animationTime) {
    for (int index = 0; index < m_NumScalings - 1; ++index) {
        if (animationTime < m_Scales[index + 1].timeStamp)
            return index;
    }
    return 0;
}

/* Gets normalized value for Lerp & Slerp*/
double Bone::GetScaleFactor(double lastTimeStamp, double nextTimeStamp, double animationTime) {
    double scaleFactor = 0.0f;
    double midWayLength = animationTime - lastTimeStamp;
    double framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

/*figures out which position keys to interpolate b/w and performs the interpolation 
and returns the translation matrix*/
glm::mat4 Bone::InterpolatePosition(double animationTime) {
    if (1 == m_NumPositions)
        return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    double scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

/*figures out which rotations keys to interpolate b/w and performs the interpolation 
and returns the rotation matrix*/
glm::mat4 Bone::InterpolateRotation(double animationTime) {
    if (1 == m_NumRotations){
        glm::quat rotation = glm::normalize(m_Rotations[0].orientation);
        return glm::toMat4(rotation);
    }
    int p0Index = GetRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);
}

/*figures out which scaling keys to interpolate b/w and performs the interpolation 
and returns the scale matrix*/
glm::mat4 Bone::InterpolateScaling(double animationTime) {
    if (1 == m_NumScalings)
        return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

    int p0Index = GetScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    double scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}