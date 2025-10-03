#version 330 core
layout(location = 0) in vec3 inPos;
layout(location = 7) in vec3 instancePos;

out VS_OUT {
    vec3 pos;
} vs_out;

void main() {
    vs_out.pos = inPos + instancePos;
}
