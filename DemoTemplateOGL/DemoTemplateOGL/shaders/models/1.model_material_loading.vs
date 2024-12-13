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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 200;
const int MAX_BONES_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

bool isZeroMatrix(mat4 m) {
    float epsilon = 0.0001; // Small threshold for floating-point precision
    return abs(m[0][0]) < epsilon && abs(m[1][1]) < epsilon && abs(m[2][2]) < epsilon && abs(m[3][3]) < epsilon;
}
bool isZeroVec4(vec4 v){
    return v.x == 0 && v.y == 0 && v.z == 0 && v.w == 0;
}
bool isZeroVec3(vec3 v){
    return v.x == 0 && v.y == 0 && v.z == 0;
}
void main() {
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal = vec3(0.0);
    for(int i = 0; i < MAX_BONES_INFLUENCE; i++) {
        if (boneIds[i] >= 0 && boneIds[i] < MAX_BONES && weights[i] > 0.0) {
            mat4 boneMatrix = finalBonesMatrices[boneIds[i]];
            totalPosition += (boneMatrix * vec4(aPos, 1.0)) * weights[i];
            totalNormal += (mat3(boneMatrix) * aNormal) * weights[i];
        }
    }
    totalPosition = isZeroVec4(totalPosition) ? vec4(aPos,1.0) : totalPosition;
    totalNormal = isZeroVec3(totalNormal) ? aNormal : totalNormal;

    // Final transformations
    FragPos = vec3(model * totalPosition);
    Normal = normalize(mat3(model) * totalNormal);
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * totalPosition;
}
