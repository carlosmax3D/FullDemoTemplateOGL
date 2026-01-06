#version 440

//con esta variable accedemos al sampler que es quien agarrara pixel por pixel
uniform sampler2D colorMap;

//la intensidad viene del vertex shader como out
in float intensidad;

void main() {
	
	vec3 ambiente = vec3(0.1,0.1,0.1);
	
	gl_FragColor = vec4(texture2D( colorMap, gl_TexCoord[0].st).rgb * (intensidad + ambiente),
		1);		
		gl_FragColor.g = 0;
		
}
