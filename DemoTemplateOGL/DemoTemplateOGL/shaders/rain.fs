#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D dropTex;

void main() {
    vec4 c = texture(dropTex, TexCoord);
    if (c.a < 0.1) discard;
    FragColor = c;
}
