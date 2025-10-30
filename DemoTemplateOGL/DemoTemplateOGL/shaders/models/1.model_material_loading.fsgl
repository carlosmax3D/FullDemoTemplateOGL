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
    vec3 direction;
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
    vec3 norm = normalize(Normal);
    vec3 lightToFrag = normalize(FragPos - light.position);

    // Difusa
    float diff = max(dot(norm, lightToFrag), 0.0);

    // Especular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightToFrag, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Atenuación por distancia
    float distance = length(FragPos - light.position);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

    // Intensidad del foco
    float theta = dot(lightToFrag, normalize(light.direction));  // sin el signo menos
    float epsilon = cos(radians(12.5)) - cos(radians(5.5));
    float intensity = clamp((theta - cos(radians(5.5))) / epsilon, 0.0, 1.0);

    // Combinar
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;

    ambient  *= attenuation;
    diffuse  *= intensity * attenuation;
    specular *= intensity * attenuation;

    vec3 result = ambient + diffuse + specular;
    if (textureSample == 1) {
        vec4 texColor = texture(texture_diffuse1, TexCoords);
        if (texColor.a < 0.1)
            discard;
        FragColor = vec4(result, 1.0) * texColor;
    } else {
        FragColor = vec4(result, 1.0); // use color alpha if you want
    }
}
