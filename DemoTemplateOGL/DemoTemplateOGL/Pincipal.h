#pragma once
#include "Base/model.h"
#include "Billboard2D.h"

class Principal : public Model {
public:
	vector<Billboard2D*> vidas;
	// Cooldown en milisegundos para volver a recibir daño
	double damageCooldownMs = 0.0; // <= 0 listo para recibir daño
	Principal(string const& path, Camera* camera, bool rotationX = false, bool rotationY = true, bool gamma = false)
		: Model(path, camera, rotationX, rotationY, gamma) {
		asignarVidas();
	}
	Principal(vector<Vertex>& vertices, unsigned int numVertices, vector<unsigned int>& indices, unsigned int numIndices, Camera* camera)
		: Model(vertices, numVertices, indices, numIndices, camera) {
		asignarVidas();
	}
	Principal(string const& path, glm::vec3& actualPosition, Camera* cam, bool rotationX = false, bool rotationY = true, bool gamma = false)
		: Model(path, actualPosition, cam, rotationX, rotationY, gamma) {
		asignarVidas();
	}
	Principal() : Model() {
		asignarVidas();
	}
	bool puedeRecibirDano() const { return damageCooldownMs <= 0.0; }
	void iniciarCooldownDano(double ms) { damageCooldownMs = ms; }
	void actualizarCooldown(double deltaMs) { if (damageCooldownMs > 0.0) { damageCooldownMs -= deltaMs; if (damageCooldownMs < 0.0) damageCooldownMs = 0.0; } }
	void asignarVidas() {
		vidas.push_back(new Billboard2D((WCHAR*)L"billboards/vida1.png", 6, 6, 50, 50, 0, this->cameraDetails));
		glm::vec3 scale = glm::vec3(500.0f, 45.0f, 100.0f);
		vidas.back()->setScale(&scale);

		vidas.push_back(new Billboard2D((WCHAR*)L"billboards/vida2.png", 6, 6, 50, 50, 0, this->cameraDetails));
		scale = glm::vec3(500.0f, 45.0f, 100.0f);
		vidas.back()->setScale(&scale);

		vidas.push_back(new Billboard2D((WCHAR*)L"billboards/vida3.png", 6, 6, 50, 50, 0, this->cameraDetails));
		scale = glm::vec3(500.0f, 45.0f, 100.0f);
		vidas.back()->setScale(&scale);

		vidas.push_back(new Billboard2D((WCHAR*)L"billboards/vida4.png", 6, 6, 50, 50, 0, this->cameraDetails));
		scale = glm::vec3(500.0f, 45.0f, 100.0f);
		vidas.back()->setScale(&scale);

		vidas.push_back(new Billboard2D((WCHAR*)L"billboards/vida5.png", 6, 6, 50, 50, 0, this->cameraDetails));
		scale = glm::vec3(500.0f, 45.0f, 100.0f);
		vidas.back()->setScale(&scale);
	}

	void Draw() {
		Model::Draw();
		for(Billboard2D * vida : vidas)
			vida->Draw();
	}
};
