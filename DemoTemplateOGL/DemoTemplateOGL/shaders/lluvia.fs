#version 330 core
out vec4 FragColor;
uniform vec3 rainColor;

void main() {
    FragColor = vec4(rainColor, 0.6); // translucent blue
}
