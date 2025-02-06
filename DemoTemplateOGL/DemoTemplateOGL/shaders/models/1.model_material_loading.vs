#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIds; 
layout (location = 6) in vec4 weights;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform int nModelBones;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 200;
uniform mat4 finalBonesMatrices[MAX_BONES];

bool isZeroMatrix(mat4 m) {
    return m[0].x == 0.0 && m[0].y == 0.0 && m[0].z == 0.0 && m[0].w == 0.0 &&
           m[1].x == 0.0 && m[1].y == 0.0 && m[1].z == 0.0 && m[1].w == 0.0 &&
           m[2].x == 0.0 && m[2].y == 0.0 && m[2].z == 0.0 && m[2].w == 0.0 &&
           m[3].x == 0.0 && m[3].y == 0.0 && m[3].z == 0.0 && m[3].w == 0.0;
}
void main() {
    mat4 BoneTransform = mat4(0);// = finalBonesMatrices[boneIds[0]] * weights[0];
//    BoneTransform += finalBonesMatrices[boneIds[1]] * weights[1];
//    BoneTransform += finalBonesMatrices[boneIds[2]] * weights[2];
//    BoneTransform += finalBonesMatrices[boneIds[3]] * weights[3];
    int flag = 1;
    for(int i = 0 ; i < 4 ; i++) {
        if(boneIds[i] == -1)
            continue;
        if (isZeroMatrix(finalBonesMatrices[boneIds[i]]) || weights[i] == 0.0f)
            continue;
        if (flag == 1){
            BoneTransform = finalBonesMatrices[boneIds[i]] * weights[i];
            flag = 0;
        } else BoneTransform += finalBonesMatrices[boneIds[i]] * weights[i];
    }
    if (isZeroMatrix(BoneTransform))
        BoneTransform = mat4(1);
    vec4 totalPosition = BoneTransform * vec4(aPos, 1.0f);
    mat3 normalMatrix = transpose(inverse(mat3(BoneTransform)));
    FragPos = vec3(model * totalPosition);
    Normal = normalize(normalMatrix * aNormal);

    TexCoords = aTexCoords;
    gl_Position = projection * view * model * totalPosition;
}
