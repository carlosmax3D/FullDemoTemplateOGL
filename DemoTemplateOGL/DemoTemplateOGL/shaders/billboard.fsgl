#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture_diffuse1;
uniform vec4 color;

void main()
{    
    vec4 texColor = texture(texture_diffuse1, TexCoord);
    if(texColor.a < 0.1)
        discard;
	FragColor = vec4(texColor.xyz,texColor.a);
}