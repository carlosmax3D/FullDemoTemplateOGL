#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in VS_OUT {
    vec3 pos;
} gs_in[];

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;

out vec2 TexCoord;

void main() {
    vec3 pos = gs_in[0].pos;

    // small quad around pos
    float size = 0.1;

    // Direccion desde la camara hacia la particula
    vec3 forward = normalize(pos - cameraPos);

    // Vector up global
    vec3 worldUp = vec3(0.0, 1.0, 0.0);

    // Ejes del billboard
    vec3 right = normalize(cross(worldUp, forward));
    vec3 up    = normalize(cross(forward, right));

    // Construir el quad orientado a la camara
    vec3 p0 = pos - right * size - up * size;
    vec3 p1 = pos + right * size - up * size;
    vec3 p2 = pos - right * size + up * size;
    vec3 p3 = pos + right * size + up * size;

    gl_Position = projection * view * vec4(p0, 1.0); TexCoord = vec2(0,0); EmitVertex();
    gl_Position = projection * view * vec4(p1, 1.0); TexCoord = vec2(1,0); EmitVertex();
    gl_Position = projection * view * vec4(p2, 1.0); TexCoord = vec2(0,1); EmitVertex();
    gl_Position = projection * view * vec4(p3, 1.0); TexCoord = vec2(1,1); EmitVertex();
    EndPrimitive();
}
