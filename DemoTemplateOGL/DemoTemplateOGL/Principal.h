#pragma once
#include "Base/model.h"
#include "Billboard2D.h"

class Principal : public Model {
public:
	vector<Billboard2D*> vidas;
	float tiempoInvulnerable = 0.0f;												//Tal vez
	bool enInvulnerable = false;													//Tal vez
	Principal(string const& path, Camera* camera, bool rotationX = false, bool rotationY = true, bool gamma = false) 
		: Model(path, camera, rotationX, rotationY, gamma) {
		asignardVidas();
		}
	Principal(vector<Vertex>& vertices, unsigned int numVertices, vector<unsigned int>& indices, unsigned int numIndices, Camera* camera)
		: Model(vertices, numVertices, indices, numIndices, camera) {
		asignardVidas();
	}
	Principal(string const& path, glm::vec3& actualPosition, Camera* cam, bool rotationX = false, bool rotationY = true, bool gamma = false)
		: Model(path, actualPosition, cam, rotationX, rotationY, gamma) {
		asignardVidas();
	}
	Principal() : Model () {
		asignardVidas();
	}
	void asignardVidas() {
		vidas.push_back(new Billboard2D((WCHAR*)L"textures/heartFull.png", 6, 6, 10, 50, 0, this->cameraDetails));
		glm::vec3 scale = glm::vec3(70.0f, 70.0f, 0.0f);	
		vidas.back()->setScale(&scale);
		vidas.push_back(new Billboard2D((WCHAR*)L"textures/heartFull.png", 6, 6, 80, 50, 0, this->cameraDetails));
		scale = glm::vec3(70.0f, 70.0f, 0.0f);
		vidas.back()->setScale(&scale);
		vidas.push_back(new Billboard2D((WCHAR*)L"textures/heartFull.png", 6, 6, 150, 50, 0, this->cameraDetails));
		scale = glm::vec3(70.0f, 70.0f, 0.0f);
		vidas.back()->setScale(&scale);
		vidas.push_back(new Billboard2D((WCHAR*)L"textures/heartFull.png", 6, 6, 220, 50, 0, this->cameraDetails));
		scale = glm::vec3(70.0f, 70.0f, 0.0f);
		vidas.back()->setScale(&scale);
	}

	void Draw() {
		Model::Draw();
		for (Billboard2D* vida : vidas)
			vida->Draw();
	}
};