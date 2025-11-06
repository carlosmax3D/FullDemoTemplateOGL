#ifndef _terreno
#define _terreno
#include "Base/model.h"

class Terreno : public Model {

private:
	struct Plano {
		float A, B, C, D;
	};

	std::vector<Plano> planos1; // para triángulos donde difx > difz
	std::vector<Plano> planos2; // para triángulos donde difx <= difz
	float anchof;
	float proff;
	float deltax, deltaz;
	int mapAlturaX, mapAlturaY;

	void PrecalcularPlanos() {
		std::vector<Vertex>& vertices = meshes[0]->vertices;
		planos1.resize((verx - 1) * (verz - 1));
		planos2.resize((verx - 1) * (verz - 1));

		for (int iz = 0; iz < verz - 1; ++iz) {
			for (int ix = 0; ix < verx - 1; ++ix) {
				int base = ix + iz * (verx - 1);

				glm::vec3 p0 = vertices[ix + iz * verx].Position;
				glm::vec3 p1 = vertices[(ix + 1) + iz * verx].Position;
				glm::vec3 p2 = vertices[(ix + 1) + (iz + 1) * verx].Position;
				glm::vec3 n1 = glm::cross(p2 - p0, p1 - p0);
				float D1 = -glm::dot(n1, p0);
				planos1[base] = { n1.x, n1.y, n1.z, D1 };

				glm::vec3 p3 = vertices[ix + (iz + 1) * verx].Position;
				glm::vec3 n2 = glm::cross(p3 - p0, p2 - p0);
				float D2 = -glm::dot(n2, p0);
				planos2[base] = { n2.x, n2.y, n2.z, D2 };
			}
		}
	}

public:
	int verx, verz;
	//el nombre numerico de la textura en cuestion, por lo pronto una

	Terreno(WCHAR alturas[], WCHAR textura[], float ancho, float prof, Camera* camera, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW) {
		cameraDetails = camera;
		vector<unsigned int> indices;
		vector<Texture>      textures;
		vector<Material> materials;
		vector<Vertex>       vertices;
		if (this->getModelAttributes()->size() == 0){
			ModelAttributes attr{0};
			this->getModelAttributes()->push_back(attr);
		}
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
		UTILITIES_OGL::Maya terreno = UTILITIES_OGL::PlanoTerreno(mapAlturaX, mapAlturaY, ancho, prof, mapaAlturas, mapAlturaComp);
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
		PrecalcularPlanos();
	}

	Terreno(WCHAR alturas[], WCHAR textura[], WCHAR textura2[], WCHAR splatmap[],
		float ancho, float prof, Camera* camera,
		int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW) {

		cameraDetails = camera;
		vector<unsigned int> indices;
		vector<Texture> textures;
		vector<Material> materials;
		vector<Vertex> vertices;
		if (this->getModelAttributes()->size() == 0){
			ModelAttributes attr{0};
			this->getModelAttributes()->push_back(attr);
		}

		unsigned int planoTextura, planoTextura2, splatTextura;
		int mapAlturaComp;
		anchof = ancho;
		proff = prof;

		// Cargar mapa de alturas
		char stext[1024];
#ifdef _WIN32
		wcstombs_s(NULL, stext, 1024, (wchar_t*)alturas, 1024);
#else
		wcstombs(stext, (wchar_t*)alturas, 1024);
#endif
		unsigned char* mapaAlturas = loadFile(stext, &mapAlturaX, &mapAlturaY, &mapAlturaComp, 0);

		// Crear geometr�a del terreno
		UTILITIES_OGL::Maya terreno = UTILITIES_OGL::Plano(mapAlturaX, mapAlturaY, ancho, prof, mapaAlturas, mapAlturaComp, 30);
		UTILITIES_OGL::vectoresEsfera(terreno, vertices, indices, mapAlturaX * mapAlturaY * 3, (mapAlturaX - 1) * (mapAlturaY - 1) * 6);
		delete[] terreno.maya;
		delete[] terreno.indices;
		delete[] mapaAlturas;

		verx = mapAlturaX;
		verz = mapAlturaY;
		deltax = anchof / verx;
		deltaz = proff / verz;

		// ============================================================
		// CARGAR LAS 3 TEXTURAS
		// ============================================================

		// 1. TEXTURA ORIGINAL (pasto)
		Texture t1;
#ifdef _WIN32
		wcstombs_s(NULL, stext, 1024, (wchar_t*)textura, 1024);
		strcpy_s(t1.type, 255, "texture_diffuse");
		strcpy_s(t1.path, 1024, stext);
#else
		wcstombs(stext, (wchar_t*)textura, 1024);
		strcpy(t1.type, "texture_diffuse");
		strcpy(t1.path, stext);
#endif
		planoTextura = TextureFromFile(stext, this->directory);
		t1.id = planoTextura;
		textures.emplace_back(t1);

		// 2. TEXTURA SECUNDARIA (tierra)
		Texture t2;
#ifdef _WIN32
		wcstombs_s(NULL, stext, 1024, (wchar_t*)textura2, 1024);
		strcpy_s(t2.type, 255, "texture_diffuse2");
		strcpy_s(t2.path, 1024, stext);
#else
		wcstombs(stext, (wchar_t*)textura2, 1024);
		strcpy(t2.type, "texture_diffuse2");
		strcpy(t2.path, stext);
#endif
		planoTextura2 = TextureFromFile(stext, this->directory);
		t2.id = planoTextura2;
		textures.emplace_back(t2);

		// 3. SPLATMAP (multitextura.jpg)
		Texture t3;
#ifdef _WIN32
		wcstombs_s(NULL, stext, 1024, (wchar_t*)splatmap, 1024);
		strcpy_s(t3.type, 255, "texture_splatmap");
		strcpy_s(t3.path, 1024, stext);
#else
		wcstombs(stext, (wchar_t*)splatmap, 1024);
		strcpy(t3.type, "texture_splatmap");
		strcpy(t3.path, stext);
#endif
		splatTextura = TextureFromFile(stext, this->directory);
		t3.id = splatTextura;
		textures.emplace_back(t3);

		// Crear mesh
		meshes.emplace_back(new Mesh(vertices, indices, textures, materials, VBOGLDrawType, EBOGLDrawType));
		setDefaultShader(false);

		// Guardar referencias
		textures_loaded.emplace_back(&this->meshes[0]->textures.data()[0]);
		textures_loaded.emplace_back(&this->meshes[0]->textures.data()[1]);
		textures_loaded.emplace_back(&this->meshes[0]->textures.data()[2]);
		PrecalcularPlanos();
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
		float fx = (x + anchof / 2) / deltax;
		float fz = (z + proff / 2) / deltaz;
		int ix = (int)fx;
		int iz = (int)fz;

		if (ix < 0 || iz < 0 || ix >= verx - 1 || iz >= verz - 1)
			return 0.0f;

		float dx = fx - ix;
		float dz = fz - iz;

		int base = ix + iz * (verx - 1);

		const Plano& P = (dx > dz) ? planos1[base] : planos2[base];
		return (-P.A * x - P.C * z - P.D) / P.B;
	}


	virtual void prepShader(Shader& shader) {
		// Obtener iluminaci�n din�mica del SkyDome
		// (Esto ya lo est�s pasando desde Scenario::Render())

		shader.setVec3("viewPos", cameraDetails->getPosition());

		// view/projection transformations
		shader.setMat4("projection", cameraDetails->getProjection());
		shader.setMat4("view", cameraDetails->getView());

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, *getTranslate());

		if (this->getRotX() != 0)
			model = glm::rotate(model, glm::radians(this->getRotX()), glm::vec3(1, 0, 0));
		if (this->getRotY() != 0)
			model = glm::rotate(model, glm::radians(this->getRotY()), glm::vec3(0, 1, 0));
		if (this->getRotZ() != 0)
			model = glm::rotate(model, glm::radians(this->getRotZ()), glm::vec3(0, 0, 1));

		shader.setMat4("model", model);

		// Calcular matriz normal para iluminaci�n correcta
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
		shader.setMat3("normalMatrix", normalMatrix);
	}

	float getAncho() { return anchof; }
	void  setAncho(float ancho) { anchof = ancho; }
	int   getMapAlturaX() { return mapAlturaX; }
	int   getMapAlturaY() { return mapAlturaY; }
};

#endif 