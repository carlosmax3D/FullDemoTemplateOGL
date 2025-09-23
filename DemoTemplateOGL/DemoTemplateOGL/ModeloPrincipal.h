#pragma once
#include "Base/model.h"
#include "Billboard2D.h"

class ModeloPrincipal : public Model {
public:
	std::vector<Billboard2D*> vidas;
	ModeloPrincipal() {
		iniciarVidas();
	}
	ModeloPrincipal(vector<Vertex>& vertices, unsigned int numVertices, vector<unsigned int>& indices, unsigned int numIndices, Camera* camera)
		: Model(vertices, numVertices, indices, numIndices, camera) {
		iniciarVidas();
	}
	ModeloPrincipal(string const& path, glm::vec3& actualPosition, Camera* cam, bool rotationX = false, bool rotationY = true, bool gamma = false)
		: Model(path, actualPosition, cam, rotationX, rotationY, gamma) {
		iniciarVidas();
	}

	ModeloPrincipal(string const& path, Camera* camera, bool rotationX = false, bool rotationY = true, bool gamma = false)
		: Model(path, camera, rotationX, rotationY, gamma) {
		iniciarVidas();
	}
	void iniciarVidas() {
		glm::vec3 scale = glm::vec3(100.0f, 100.0f, 0.0f);
		vidas.emplace_back(new Billboard2D((WCHAR*)L"textures/corazon1.png", 6, 6, 0, 25, 0, this->cameraDetails));
		vidas.back()->setScale(&scale);
		vidas.emplace_back(new Billboard2D((WCHAR*)L"textures/corazon1.png", 6, 6, 100, 25, 0, this->cameraDetails));
		vidas.back()->setScale(&scale);
		vidas.emplace_back(new Billboard2D((WCHAR*)L"textures/corazon1.png", 6, 6, 200, 25, 0, this->cameraDetails));
		vidas.back()->setScale(&scale);
		vidas.emplace_back(new Billboard2D((WCHAR*)L"textures/corazon1.png", 6, 6, 300, 25, 0, this->cameraDetails));
		vidas.back()->setScale(&scale);
		vidas.emplace_back(new Billboard2D((WCHAR*)L"textures/corazon1.png", 6, 6, 400, 25, 0, this->cameraDetails));
		vidas.back()->setScale(&scale);
	}
	~ModeloPrincipal() {
		for (int i = 0; i < vidas.size(); i++)
			delete vidas[i];
	}
	void Draw() {
		for (auto vida : vidas)
			vida->Draw();
		Model::Draw();
	}
};