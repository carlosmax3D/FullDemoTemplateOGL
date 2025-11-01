#version 330 core
layout(location = 0) in vec3 inPos;

out VS_OUT {
    vec3 pos;
} vs_out;

void main() {
    vs_out.pos = inPos;
}
