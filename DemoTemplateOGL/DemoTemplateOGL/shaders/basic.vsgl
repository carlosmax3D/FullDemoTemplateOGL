#version 440

out float intensidad;

void main()
{
	//aqui definimos la textura activa con la que trabajaremos
	//por lo pronto es la 0, si hay mas pues entonces con su numero
    gl_TexCoord[0] = gl_MultiTexCoord0;

	//obtenemos al vertice y lo transformamos para que obtenga su nueva posicion
	//esta es con la que trabajaremos en lo siguiente
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;

	//La normal matrix es la transpuesta de la ModelView Matrix
	//con vectores es necesario manejarlo asi y tambien es necesario
	//mover la normal ya que se movio el punto o vertice
	vec3 Normal = normalize(gl_NormalMatrix *gl_Normal);

	//esta es la luz que definimos en el render, tenemos esta fuente de posicion
	//aunque tambien podemos definirla fuera de esto.
	vec3 vertex_light_position = normalize(gl_LightSource[0].position.xyz);		

	//obtenemos la intensidad, si utilizamos la posicion de la luz solamente
	//sera direccional, si la restamos del vertice sera puntual
	//intensidad = max(dot(Normal, -vertex_light_position),0); //quiten el comentario si quieren esta

	//en caso de luz puntual asi seria
	intensidad = max(dot(Normal, -normalize(vec3(gl_LightSource[0].position.xyz-gl_ModelViewMatrix*gl_Vertex))),0); //pongan comentario si usan la otra
	
}