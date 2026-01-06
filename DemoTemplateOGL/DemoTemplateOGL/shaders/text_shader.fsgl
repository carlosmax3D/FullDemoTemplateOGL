#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture_diffuse1;
uniform vec3 color;

void main(){
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(texture_diffuse1, TexCoord).r);
    if(sampled.a < 0.1)
        discard;
    FragColor = vec4(color, 1.0) * sampled;
}  