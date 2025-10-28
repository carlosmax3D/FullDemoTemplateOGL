#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 12) in float enabled;

uniform mat4 model;
uniform int multipleInstances = 0;
out vec2 TexCoord;

void main()
{
    if (multipleInstances == 1 && enabled == 0.0f){
        gl_Position.w = 0;
    }else {
		gl_Position = model * vec4(aPos, 1.0);
		TexCoord =  vec2(aTexCoord.x, aTexCoord.y);
	}
}