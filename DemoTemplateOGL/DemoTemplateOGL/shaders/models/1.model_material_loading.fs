#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
};
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec2 TexCoords;
in vec3 Normal;  
  
out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform int textureSample = 1;
uniform sampler2D texture_diffuse1;
uniform vec4 color;

void main()
{    
    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;  
        
    vec3 result = ambient + diffuse + specular;
//    result *= color.rgb; // <--- Apply color tint here

    if (textureSample == 1) {
        vec4 texColor = texture(texture_diffuse1, TexCoords);
        if (texColor.a < 0.1)
            discard;
        FragColor = vec4(result, 1.0) * texColor;
    } else {
        FragColor = vec4(result, 1.0); // use color alpha if you want
    }
}
