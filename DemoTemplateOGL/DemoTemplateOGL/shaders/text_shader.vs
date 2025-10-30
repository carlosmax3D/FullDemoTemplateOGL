#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location =  7) in vec3 trans;
layout (location =  8) in vec3 scale;
layout (location =  9) in float rotX;
layout (location = 10) in float rotY;
layout (location = 11) in float rotZ;
layout (location = 12) in float enabled;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform int multipleInstances; // Pasar desde CPU
out vec2 TexCoord;

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
mat4 translateMat(mat4 m, vec3 v) {
    // Multiplica cada columna por el componente del vector y suma la columna de traslación
    mat4 result;
    result[0] = m[0]; // columna X
    result[1] = m[1]; // columna Y
    result[2] = m[2]; // columna Z
    result[3] = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3]; // columna W = m * v + m[3]
    return result;
}
mat4 scaleMat(mat4 m, vec3 v) {
    mat4 result;
    result[0] = m[0] * v.x; // Escalar columna X
    result[1] = m[1] * v.y; // Escalar columna Y
    result[2] = m[2] * v.z; // Escalar columna Z
    result[3] = m[3];       // Columna de traslación sin cambios
    return result;
}
mat4 rotateMat(mat4 m, float angle, vec3 axis) {
    float c = cos(angle);
    float s = sin(angle);
    vec3 a = normalize(axis);
    vec3 temp = (1.0 - c) * a;
    // Matriz de rotación 3x3
    mat3 R;
    R[0][0] = c + temp.x * a.x;
    R[0][1] = temp.x * a.y + s * a.z;
    R[0][2] = temp.x * a.z - s * a.y;
    R[1][0] = temp.y * a.x - s * a.z;
    R[1][1] = c + temp.y * a.y;
    R[1][2] = temp.y * a.z + s * a.x;
    R[2][0] = temp.z * a.x + s * a.y;
    R[2][1] = temp.z * a.y - s * a.x;
    R[2][2] = c + temp.z * a.z;
    // Aplicar la rotación a la matriz 4x4
    mat4 result;
    result[0] = m[0] * R[0][0] + m[1] * R[0][1] + m[2] * R[0][2];
    result[1] = m[0] * R[1][0] + m[1] * R[1][1] + m[2] * R[1][2];
    result[2] = m[0] * R[2][0] + m[1] * R[2][1] + m[2] * R[2][2];
    result[3] = m[3]; // columna de traslación sin cambios
    return result;
}
mat4 createInstanceMatrix() {
    if (multipleInstances == 0) return model;
    mat4 T = mat4(1.0);
    T = translateMat(T, trans);
    if (rotX != 0)
        T = rotateMat(T, radians(rotX), vec3(1,0,0));
    if (rotY != 0)
        T = rotateMat(T, radians(rotY), vec3(0,1,0));
    if (rotZ != 0)
        T = rotateMat(T, radians(rotZ), vec3(0,0,1));
    if (!isZeroVec3(scale))
        T = scaleMat(T, scale);
    return T;
}
void main()
{
    if (multipleInstances == 1 && enabled == 0.0f){
        gl_Position.w = 0;
    }else {
		gl_Position =  projection * createInstanceMatrix() * vec4(aPos, 1.0);
		TexCoord =  vec2(aTexCoord.x, aTexCoord.y);
	}
}