#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;

uniform sampler2D texture1;      // Textura actual
uniform sampler2D texture2;      // Textura siguiente
uniform float blendFactor;       // Factor de mezcla (0.0 a 1.0)
uniform bool useBlending;        // Si usar mezcla o no
uniform float timeOfDay;
uniform vec3 cameraPos;

void main()
{
    vec4 color1 = texture(texture1, TexCoords);
    vec4 finalColor;
    
    if (useBlending) {
        // Mezclar dos texturas para transición suave
        vec4 color2 = texture(texture2, TexCoords);
        finalColor = mix(color1, color2, blendFactor);
    } else {
        finalColor = color1;
    }
    
    // Aplicar tintes de color según la hora del día
    vec3 tintColor = vec3(1.0);
    
    // NOCHE (0-8): Tinte azul oscuro
    if (timeOfDay >= 0.0 && timeOfDay < 8.0) {
        float nightIntensity = 1.0 - (timeOfDay / 8.0);
        tintColor = mix(vec3(1.0), vec3(0.5, 0.5, 0.8), nightIntensity * 0.4);
    }
    // DÍA (8-16): Color natural, ligeramente más brillante
    else if (timeOfDay >= 8.0 && timeOfDay < 16.0) {
        tintColor = vec3(1.0, 1.0, 1.05);
    }
    // TARDE (16-24): Tinte naranja/rojizo
    else {
        float eveningIntensity = (timeOfDay - 16.0) / 8.0;
        tintColor = mix(vec3(1.0, 0.85, 0.7), vec3(0.7, 0.5, 0.6), eveningIntensity * 0.5);
    }
    
    // Aplicar tinte
    finalColor.rgb *= tintColor;
    
    // Ajustar brillo general según hora
    float brightness = 1.0;
    if (timeOfDay >= 0.0 && timeOfDay < 8.0) {
        // Noche más oscura
        brightness = 0.4 + (timeOfDay / 8.0) * 0.6;
    } else if (timeOfDay >= 8.0 && timeOfDay < 16.0) {
        // Día brillante
        brightness = 1.0;
    } else {
        // Tarde atenuada
        brightness = 1.0 - ((timeOfDay - 16.0) / 8.0) * 0.6;
    }
    
    finalColor.rgb *= brightness;
    
    FragColor = vec4(finalColor.rgb, finalColor.a);
}