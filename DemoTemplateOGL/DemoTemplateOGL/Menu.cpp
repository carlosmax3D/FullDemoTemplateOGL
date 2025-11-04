#include "Menu.h"

void Menu::initMenu() {
	glm::vec3 scale;

	//corregir la escala para la flecha
	scale = glm::vec3(100.0f, 100.0f, 0.0f);
	billBoard2D.emplace_back(new Billboard2D((WCHAR*)L"billboards/flecha.png", 6, 6, 0, 150, 0, camDetails));
	billBoard2D.back()->setScale(&scale);
	arrow = billBoard2D.back();

	scale = glm::vec3(400.0f, 400.0f, 0.0f);	// it's a bit too big for our scene, so scale it down
	billBoard2D.emplace_back(new Billboard2D((WCHAR*)L"billboards/MENU.png", 6, 6, 50, 100, 0, camDetails));
	billBoard2D.back()->setScale(&scale);

	/*
	//corregir la escala para la flecha
	scale = glm::vec3(100.0f, 100.0f, 0.0f);
	billBoard2D.emplace_back(new Billboard2D((WCHAR*)L"billboards/flecha.png", 6, 6, 0, 150, 0, camDetails));
	billBoard2D.back()->setScale(&scale);*/
}

Menu::Menu(Camera *cam) {
	camDetails = cam;
	initMenu();
}

void Menu::Update(int opc) {
	//creamos un vector de posicion para la flecha
	glm::vec3 trans;
	switch (opc) {
		case 1:
			trans = glm::vec3(0, 150, 0);
			break;
		case 2:
			trans = glm::vec3(0, 250, 0);
			break;
		case 3:
			trans = glm::vec3(0, 350, 0);
			break;
	}
	arrow->setTranslate(&trans);
}

void Menu::Draw() {
	for (auto *b : billBoard2D) 
		b->Draw();
}
