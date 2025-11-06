#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
};

struct DirLight {
    vec3 direction;
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
uniform DirLight dirLight;

uniform int textureSample;
uniform sampler2D texture_diffuse1;
uniform vec4 color;

uniform float timeOfDay;
uniform bool useDynamicLighting;

// MISMAS funciones de sombra que el terreno
float calculateTerrainShadow(vec3 normal, vec3 lightDir) {
    float NdotL = dot(normal, lightDir);
    float shadow = smoothstep(-0.1, 0.2, NdotL);
    
    // Ajuste según hora del día - IDÉNTICO al terreno
    if (timeOfDay >= 0.0 && timeOfDay < 6.0) {
        shadow *= 0.55; // Noche 
    }
    else if (timeOfDay >= 6.0 && timeOfDay < 8.0) {
        float t = (timeOfDay - 6.0) / 2.0;
        shadow *= mix(0.25, 1.0, t); // Amanecer
    }
    else if (timeOfDay >= 16.0 && timeOfDay < 20.0) {
        float t = (timeOfDay - 16.0) / 4.0;
        shadow *= mix(1.0, 0.4, t); // Atardecer
    }
    else if (timeOfDay >= 20.0) {
        shadow *= 0.4; // Noche
    }
    
    return clamp(shadow, 0.15, 1.0);
}

void main()
{    
    vec4 texColor;
    if (textureSample == 1) {
        texColor = texture(texture_diffuse1, TexCoords);
        if (texColor.a < 0.1)
            discard;
    } else {
        texColor = vec4(1.0);
    }
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-dirLight.direction);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Ambient - MÁS SIMILAR al terreno
    vec3 ambient = dirLight.ambient * texColor.rgb * material.ambient;
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * texColor.rgb * material.diffuse;
    
    // Specular - AJUSTADO para ser más sutil como el terreno
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = dirLight.specular * spec * material.specular * 0.1; // Reducido como el terreno
    
    // Calcular sombras - SIEMPRE usar dynamic lighting para consistencia
    float shadowFactor = calculateTerrainShadow(norm, lightDir);
    
    // Combinar - MISMOS cálculos que el terreno
    vec3 result = ambient + (diffuse + specular) * shadowFactor;
    
    // Ajuste de brillo general - IDÉNTICO al terreno
    float brightness = 1.0;
    if (timeOfDay >= 0.0 && timeOfDay < 8.0) {
        brightness = 0.35 + (timeOfDay / 8.0) * 0.5;
    }
    else if (timeOfDay >= 16.0 && timeOfDay < 24.0) {
        brightness = 1.0 - ((timeOfDay - 16.0) / 8.0) * 0.5;
    }
    
    result *= brightness;
    result *= color.rgb; // Preservar color uniform si se usa
    
    FragColor = vec4(result, texColor.a);
}