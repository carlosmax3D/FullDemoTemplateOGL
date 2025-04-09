#ifndef _terreno
#define _terreno
#include "Base/model.h"

class Terreno : public Model {

private:
	float anchof;
	float proff;
	float deltax, deltaz;
	int mapAlturaX, mapAlturaY;

public:
	int verx, verz;
	Camera* cameraDetails = NULL;
	//el nombre numerico de la textura en cuestion, por lo pronto una

	Terreno(WCHAR alturas[], WCHAR textura[], float ancho, float prof, Camera* camera, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW) {
		cameraDetails = camera;
		vector<unsigned int> indices;
		vector<Texture>      textures;
		vector<Material> materials;
		vector<Vertex>       vertices;
		ModelAttributes attr{0};
		this->getModelAttributes()->push_back(attr);
		unsigned int planoTextura;
		int mapAlturaComp;
		anchof = ancho;
		proff = prof;
		//cargamos la textura de la figura
		char stext[1024];
#ifdef _WIN32
		wcstombs_s(NULL, stext, 1024, (wchar_t*)alturas, 1024);
#else
		wcstombs(stext, (wchar_t*)alturas, 1024);
#endif
		unsigned char* mapaAlturas = loadFile(stext, &mapAlturaX, &mapAlturaY, &mapAlturaComp, 0);
		//en caso del puntero de la imagen sea nulo se brica esta opcion
		UTILITIES_OGL::Maya terreno = UTILITIES_OGL::Plano(mapAlturaX, mapAlturaY, ancho, prof, mapaAlturas, mapAlturaComp, 30);
		UTILITIES_OGL::vectoresEsfera(terreno, vertices, indices, mapAlturaX * mapAlturaY * 3, (mapAlturaX - 1) * (mapAlturaY - 1) * 6);
		delete[] terreno.maya;
		delete[] terreno.indices;
		verx = mapAlturaX;
		verz = mapAlturaY;
		deltax = anchof / verx;
		deltaz = proff / verz;
		//disponemos la textura del gdi.
		delete[]mapaAlturas;

		// cargamos la textura de la figura
		Texture t;
#ifdef _WIN32
		wcstombs_s(NULL, stext, 1024, (wchar_t*)textura, 1024);
		strcpy_s(t.type, 255, "texture_height");
		strcpy_s(t.path, 1024, stext);
#else
		wcstombs(stext, (wchar_t*)textura, 1024);
		strcpy(t.type, "texture_height");
		strcpy(t.path, stext);
#endif
		planoTextura = TextureFromFile(stext, this->directory);
		t.id = planoTextura;
		textures.emplace_back(t);
		meshes.emplace_back(new Mesh(vertices, indices, textures, materials, VBOGLDrawType, EBOGLDrawType));
		setDefaultShader(false);
		textures_loaded.emplace_back(&this->meshes[0]->textures.data()[0]);
	}

	~Terreno() {
		//nos aseguramos de disponer de los recursos previamente reservados
	}

	virtual void Draw() {
		if (gpuDemo == NULL) {
			gpuDemo = new Shader("shaders/models/1.model_loading.vs", "shaders/models/1.model_loading.fs");
			setDefaultShader(true);
		}
		if (getDefaultShader()) {
			gpuDemo->use();
			prepShader(*gpuDemo);
			Draw(*gpuDemo);
			gpuDemo->desuse();
		} else Draw(*gpuDemo);
	}

	virtual void Draw(Shader& shader) {
		Model::Draw(shader,0);
	}

	float Superficie(float x, float z) {
		vector<Vertex> &vertices = meshes[0]->vertices;
		//obtenemos el indice pero podria incluir una fraccion
		float indicefx = (x + anchof / 2) / deltax;
		float indicefz = (z + proff / 2) / deltaz;
		//nos quedamos con solo la parte entera del indice
		int indiceix = (int)indicefx;
		int indiceiz = (int)indicefz;
		//nos quedamos con solo la fraccion del indice
		float difx = indicefx - indiceix;
		float difz = indicefz - indiceiz;

		float altura;
		float D;
		if (indiceix + 1 + (indiceiz + 1) * verx > vertices.size() || indiceix + 1 + indiceiz * verx > vertices.size())
			return 0.0f;
		//el cuadro del terreno esta formado por dos triangulos, si difx es mayor que dify 
		//entonces estamos en el triangulo de abajo en caso contrario arriba
		if (difx > difz)
		{
			//obtenemos el vector 1 de dos que se necesitan
			glm::vec3 v1(vertices[indiceix + 1 + (indiceiz + 1) * verx].Position.x - vertices[indiceix + indiceiz * verx].Position.x,
				vertices[indiceix + 1 + (indiceiz + 1) * verx].Position.y - vertices[indiceix + indiceiz * verx].Position.y,
				vertices[indiceix + 1 + (indiceiz + 1) * verx].Position.z - vertices[indiceix + indiceiz * verx].Position.z);
			//obtenemos el vector 2 de dos
			glm::vec3 v2(vertices[indiceix + 1 + indiceiz * verx].Position.x - vertices[indiceix + indiceiz * verx].Position.x,
				vertices[indiceix + 1 + indiceiz * verx].Position.y - vertices[indiceix + indiceiz * verx].Position.y,
				vertices[indiceix + 1 + indiceiz * verx].Position.z - vertices[indiceix + indiceiz * verx].Position.z);

			//con el producto punto obtenemos la normal y podremos obtener la ecuacion del plano
			//la parte x de la normal nos da A, la parte y nos da B y la parte z nos da C
			glm::vec3 normalPlano = glm::cross(v1, v2);
			//entonces solo falta calcular D
			D = -1 * (normalPlano.x * vertices[indiceix + indiceiz * verx].Position.x +
				normalPlano.y * vertices[indiceix + indiceiz * verx].Position.y +
				normalPlano.z * vertices[indiceix + indiceiz * verx].Position.z);
			//sustituyendo obtenemos la altura de contacto en el terreno
			altura = ((-normalPlano.x * x - normalPlano.z * z - D) / normalPlano.y);
		}
		else
		{
			glm::vec3 v1(vertices[indiceix + (indiceiz + 1) * verx].Position.x - vertices[indiceix + indiceiz * verx].Position.x,
				vertices[indiceix + (indiceiz + 1) * verx].Position.y - vertices[indiceix + indiceiz * verx].Position.y,
				vertices[indiceix + (indiceiz + 1) * verx].Position.z - vertices[indiceix + indiceiz * verx].Position.z);

			glm::vec3 v2(vertices[indiceix + 1 + (indiceiz + 1) * verx].Position.x - vertices[indiceix + indiceiz * verx].Position.x,
				vertices[indiceix + 1 + (indiceiz + 1) * verx].Position.y - vertices[indiceix + indiceiz * verx].Position.y,
				vertices[indiceix + 1 + (indiceiz + 1) * verx].Position.z - vertices[indiceix + indiceiz * verx].Position.z);


			glm::vec3 normalPlano = glm::cross(v1, v2);

			D = -1 * (normalPlano.x * vertices[indiceix + indiceiz * verx].Position.x +
				normalPlano.y * vertices[indiceix + indiceiz * verx].Position.y +
				normalPlano.z * vertices[indiceix + indiceiz * verx].Position.z);

			altura = ((-normalPlano.x * x - normalPlano.z * z - D) / normalPlano.y);
		}
		return altura;
	}

	virtual void prepShader(Shader& shader) {
		glm::vec3 lightColor;
		lightColor.x = sin(7200 * 2.0f);
		lightColor.y = sin(7200 * 0.7f);
		lightColor.z = sin(7200 * 1.3f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		shader.setVec3("light.ambient", ambientColor);
		shader.setVec3("light.diffuse", diffuseColor);
		shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
		shader.setVec3("light.position", lightPos);
		shader.setVec3("viewPos", cameraDetails->getPosition());

		// view/projection transformations
		shader.setMat4("projection", cameraDetails->getProjection());
		shader.setMat4("view", cameraDetails->getView());

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		// translate it down so it's at the center of the scene
//		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
		model = glm::translate(model, *getTranslate()); // translate it down so it's at the center of the scene
//			model = glm::translate(model, glm::vec3(cameraDetails.Position->x, cameraDetails.Position->y - 5, cameraDetails.Position->z)); // translate it down so it's at the center of the scene
//			model = glm::scale(model, glm::vec3(0.0025f, 0.0025f, 0.0025f));	// it's a bit too big for our scene, so scale it down
		if (this->getRotX() != 0)
			model = glm::rotate(model, glm::radians(this->getRotX()), glm::vec3(1, 0, 0));
		if (this->getRotY() != 0)
			model = glm::rotate(model, glm::radians(this->getRotY()), glm::vec3(0, 1, 0));
		if (this->getRotZ() != 0)
			model = glm::rotate(model, glm::radians(this->getRotZ()), glm::vec3(0, 0, 1));
		shader.setMat4("model", model);
	}

	float getAncho() { return anchof; }
	void  setAncho(float ancho) { anchof = ancho; }
	int   getMapAlturaX() { return mapAlturaX; }
	int   getMapAlturaY() { return mapAlturaY; }
};

#endif 