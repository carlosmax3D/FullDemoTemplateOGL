#version 330 core
out vec4 FragColor;
uniform sampler2D splashTex;

void main() {
    vec2 uv = gl_PointCoord;
    vec4 c = texture(splashTex, uv);
    if (c.a < 0.05) discard;
    FragColor = c;
}
