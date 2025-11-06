#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

// TEXTURAS
uniform sampler2D texture_diffuse1;  // Textura original (pasto)
uniform sampler2D texture_diffuse2;  // Textura secundaria (tierra)
uniform sampler2D texture_splatmap;  // Mapa de mezcla (escala de grises)

// Iluminación dinámica
uniform vec3 dirLightDirection;
uniform vec3 dirLightAmbient;
uniform vec3 dirLightDiffuse;
uniform vec3 dirLightSpecular;
uniform vec3 viewPos;
uniform float timeOfDay;

// Calcular sombras suaves para el terreno
float calculateTerrainShadow(vec3 normal, vec3 lightDir) {
    float NdotL = dot(normal, lightDir);
    float shadow = smoothstep(-0.1, 0.2, NdotL);
    
    // Ajuste según hora del día (MEJORADO)
    if (timeOfDay >= 0.0 && timeOfDay < 6.0) {
        shadow *= 0.55; // Noche más clara
    }
    else if (timeOfDay >= 6.0 && timeOfDay < 8.0) {
        float t = (timeOfDay - 6.0) / 2.0;
        shadow *= mix(0.55, 1.0, t); // Amanecer
    }
    else if (timeOfDay >= 16.0 && timeOfDay < 20.0) {
        float t = (timeOfDay - 16.0) / 4.0;
        shadow *= mix(1.0, 0.4, t); // Atardecer
    }
    else if (timeOfDay >= 20.0) {
        shadow *= 0.55; // Noche
    }
    
    return clamp(shadow, 0.15, 1.0);
}

void main()
{    
    // ============================================================
    // SISTEMA DE MULTITEXTURA CON SPLATMAP ÚNICO
    // ============================================================
    
    // SPLATMAP: Se aplica UNA SOLA VEZ sobre todo el terreno
    // Usamos TexCoords directamente SIN fract() para que no se repita
    vec2 splatmapCoords = TexCoords;
    
    // TEXTURAS: También aplicamos UNA SOLA VEZ para que coincidan con el splatmap
    vec2 textureCoords = TexCoords;
    
    // Leer splatmap (1x sobre TODO el terreno)
    float blend = texture(texture_splatmap, splatmapCoords).r;
    
    // Leer texturas (también 1x sobre TODO el terreno)
    vec4 texColor1 = texture(texture_diffuse1, textureCoords); // Pasto
    vec4 texColor2 = texture(texture_diffuse2, textureCoords); // Tierra
    
    // Mezclar según el splatmap:
    // blend = 1.0 (blanco) → 100% textura1 (pasto)
    // blend = 0.0 (negro)  → 100% textura2 (tierra)
    // blend = 0.5 (gris)   → 50% de cada una
    vec4 texColor = mix(texColor2, texColor1, blend);
    
    // ============================================================
    // ILUMINACIÓN
    // ============================================================
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-dirLightDirection);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Ambient
    vec3 ambient = dirLightAmbient * texColor.rgb;
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dirLightDiffuse * diff * texColor.rgb;
    
    // Specular (sutil)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 16.0);
    vec3 specular = dirLightSpecular * spec * 0.1;
    
    // Sombras
    float shadowFactor = calculateTerrainShadow(norm, lightDir);
    
    // Combinar
    vec3 result = ambient + (diffuse + specular) * shadowFactor;
    
    // Ajuste de brillo (MEJORADO)
    float brightness = 1.0;
    if (timeOfDay >= 0.0 && timeOfDay < 8.0) {
        brightness = 0.60 + (timeOfDay / 8.0) * 0.3; // Mínimo 0.60
    }
    else if (timeOfDay >= 16.0 && timeOfDay < 24.0) {
        brightness = 1.0 - ((timeOfDay - 16.0) / 8.0) * 0.3;
    }
    
    result *= brightness;
    
    FragColor = vec4(result, texColor.a);
}