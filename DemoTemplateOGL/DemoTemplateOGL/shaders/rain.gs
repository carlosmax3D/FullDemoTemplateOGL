#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in VS_OUT {
    vec3 pos;
    vec3 vel;
} gs_in[];

uniform mat4 view;
uniform mat4 proj;

out vec2 TexCoord;

void main() {
    vec3 pos = gs_in[0].pos;

    // small quad around pos
    float size = 0.1;
    vec3 right = vec3(size, 0, 0);
    vec3 up    = vec3(0, size, 0);

    vec3 p0 = pos - right - up;
    vec3 p1 = pos + right - up;
    vec3 p2 = pos - right + up;
    vec3 p3 = pos + right + up;

    gl_Position = proj * view * vec4(p0, 1.0); TexCoord = vec2(0,0); EmitVertex();
    gl_Position = proj * view * vec4(p1, 1.0); TexCoord = vec2(1,0); EmitVertex();
    gl_Position = proj * view * vec4(p2, 1.0); TexCoord = vec2(0,1); EmitVertex();
    gl_Position = proj * view * vec4(p3, 1.0); TexCoord = vec2(1,1); EmitVertex();
    EndPrimitive();
}
