#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 7) in vec3 mPos;
layout (location = 12) in float enabled;

uniform mat4 projection;
uniform mat4 view;
out vec2 TexCoord;
uniform vec3 billboardPos;
uniform int multipleInstances = 0;
uniform float width;
uniform float height;
uniform vec3 cameraPos;
uniform vec3 camRight;  // should be camera right, not front!

void main()
{
    if (multipleInstances == 1 && enabled == 0.0f){
        gl_Position.w = 0;
    }else {
        // Center the quad around its middle
        vec2 centeredPos = aPos.xy; // - vec2(0.5, 0.5);
        
        vec3 trans = multipleInstances == 1 ? mPos : billboardPos;

        // Compute camera-facing basis
        vec3 forward = normalize(cameraPos - trans);
        forward.y = 0.0;
        forward = normalize(forward);
        vec3 upVec = vec3(0.0, 1.0, 0.0);
        vec3 right   = normalize(cross(upVec,camRight));
        upVec   = normalize(cross(right, forward));

        // Compute final vertex position
        vec3 worldPos = trans 
                    + centeredPos.x * width * right
                    + centeredPos.y * height * upVec;

        gl_Position = projection * view * vec4(worldPos, 1.0);
        TexCoord = aTexCoord;
    }
}