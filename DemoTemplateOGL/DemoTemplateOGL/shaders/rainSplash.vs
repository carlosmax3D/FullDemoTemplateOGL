#version 330 core
layout(location = 0) in vec3 inPos;
layout(location = 7) in vec3 translate;
layout(location = 8) in vec3 inSize;
layout(location = 12) in int actived;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main() {
    if (actived != 0){
      gl_Position = projection * view * vec4(inPos + translate, 1.0);
      gl_PointSize = inSize.x * 20.0; // scale in screen space
      TexCoord = vec2(0.5, 0.5); // placeholder
    }else gl_Position.w = 0;
}
