#version 330 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in float inSize;
layout(location = 2) in int active;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 proj;

void main() {
    if (active != 0){
      gl_Position = proj * view * vec4(inPos, 1.0);
      gl_PointSize = inSize * 20.0; // scale in screen space
      TexCoord = vec2(0.5, 0.5); // placeholder
    }
}
