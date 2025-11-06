#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
//uniform float waveAmplitude = 0.5;
//uniform float waveFrequency = 0.5;
//uniform float waveSpeed = 0.1;

void main() {
    vec3 modifiedPos = aPos;
    // Apply wave function (sine-based)
    float waveFactorX = sin(0.5 * modifiedPos.x + 0.1 * time);
    float waveFactorZ = sin(0.5 * modifiedPos.z + 0.1 * time);
    
    // Calculate final Y position
    modifiedPos.y += 0.5 * (waveFactorX + waveFactorZ);

    // Output to fragment shader
    FragPos = modifiedPos;
    TexCoords = aTexCoords;
    Normal = aNormal;

    gl_Position = projection * view * model * vec4(modifiedPos, 1.0);
}