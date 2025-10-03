#version 330 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inVel;

out VS_OUT {
    vec3 pos;
    vec3 vel;
} vs_out;

void main() {
    vs_out.pos = inPos;
    vs_out.vel = inVel;
}
