#ifndef _Sky
#define _Sky
#include "Base/Utilities.h"
#include "Base/model.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class SkyDome : public Model {
	//El constructor llama al metodo Esfera de la clase geometrias que generara los vertices
	//normales y uvs de la misma, nos regresa la estructura Maya.
public:
	Camera* cameraDetails = NULL;
	SkyDome(int stacks, int slices, float radio, WCHAR *nombre, Camera* camera) {
		cameraDetails = camera;
		vector<unsigned int> indices;
		vector<Texture> textures;
		vector<Vertex>	vertices;
		vector<Material> materials;
		unsigned int esferaTextura;
		ModelAttributes attr{0};
		this->getModelAttributes()->push_back(attr);
		UTILITIES_OGL::Maya cuadro = UTILITIES_OGL::Esfera(stacks, slices, radio, 0.5, 1);
		UTILITIES_OGL::vectoresEsfera(cuadro, vertices, indices, stacks * slices * 3, (stacks - 1) * (slices - 1) * 6);
		delete[] cuadro.maya;
		delete[] cuadro.indices;
		// cargamos la textura de la figura
		char stext[1024];
		Texture t;
#ifdef _WIN32
		wcstombs_s(NULL, stext, 1024, (wchar_t*)nombre, 1024);
		strcpy_s(t.type, 255, "texture_diffuse");
		strcpy_s(t.path, 1024, stext);
#else
		wcstombs(stext, (wchar_t*)nombre, 1024);
		strcpy(t.type, "texture_diffuse");
		strcpy(t.path, stext);
#endif
		esferaTextura = TextureFromFile(stext, this->directory);
		t.id = esferaTextura;
		textures.emplace_back(t);
		gpuDemo = NULL;
		meshes.emplace_back(new Mesh(vertices, indices, textures, materials));
		textures_loaded.emplace_back(&this->meshes[0]->textures.data()[0]);
		setDefaultShader(false);
	}

	~SkyDome() {
		//nos aseguramos de disponer de los recursos previamente reservados
	}

	// Usa el shader default para poder imprimir el skydome
	void Draw() {
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

	void Draw(Shader& shader) {
		glDisable(GL_DEPTH_TEST);
		Model::Draw(shader,0);
		glEnable(GL_DEPTH_TEST);
	}

	void prepShader(Shader& shader) {
		glm::vec3 lightColor;
		lightColor.x = 1;// sin(7200 * 2.0f);
		lightColor.y = 1;// sin(7200 * 0.7f);
		lightColor.z = 1;// sin(7200 * 1.3f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		shader.setVec3("light.ambient", ambientColor);
		shader.setVec3("light.diffuse", diffuseColor);
		shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		glm::vec3 lightPos(1.2f, 100.0f, 2.0f);
		shader.setVec3("light.position", lightPos);
		shader.setVec3("viewPos", this->cameraDetails->getPosition());

		// view/projection transformations
		shader.setMat4("projection", cameraDetails->getProjection());
		shader.setMat4("view", cameraDetails->getView());

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		//		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::translate(model, glm::vec3(cameraDetails->getPosition().x, cameraDetails->getPosition().y - 5, cameraDetails->getPosition().z)); // translate it down so it's at the center of the scene
		if (getRotationVector()->x != 0)
			model = glm::rotate(model, glm::radians(getRotX()), glm::vec3(1, 0, 0));
		if (getRotationVector()->y != 0)
			model = glm::rotate(model, glm::radians(getRotY()), glm::vec3(0, 1, 0));
		if (getRotationVector()->z != 0)
			model = glm::rotate(model, glm::radians(getRotZ()), glm::vec3(0, 0, 1));
		//model = glm::scale(model, glm::vec3(0.0025f, 0.0025f, 0.0025f));	// it's a bit too big for our scene, so scale it down
//			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
	}
};
#endif