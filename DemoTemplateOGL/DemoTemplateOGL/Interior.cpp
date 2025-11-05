#include "Interior.h"
#ifdef __linux__ 
#define ZeroMemory(x,y) memset(x,0,y)
#define wcscpy_s(x,y,z) wcscpy(x,z)
#define wcscat_s(x,y,z) wcscat(x,z)
#endif


Interior::Interior(Camera* cam) {
	glm::vec3 translate;
	glm::vec3 scale;
	Model* model = new Model("models/Cube/Cube.obj", cam);
	translate = glm::vec3(0.0f, 0.0f, 3.0f);
	scale = glm::vec3(0.25f, 0.25f, 0.25f);	// it's a bit too big for our scene, so scale it down
	model->setScale(&scale);
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	InitGraph(model);
}
Interior::Interior(Model* model) {
	InitGraph(model);
}
void Interior::InitGraph(Model* main) {
	float matAmbient[] = { 1,1,1,1 };
	float matDiff[] = { 1,1,1,1 };
	angulo = 0;
	camara = main;
	//creamos el objeto skydome
	sky = new SkyDome(32, 32, 20, (WCHAR*)L"skydome/night3.jpg", main->cameraDetails);
	//creamos el terreno
	terreno = new Terreno((WCHAR*)L"skydome/black.png", (WCHAR*)L"skydome/texnieve.jpg", 400, 400, main->cameraDetails);
	water = new Water((WCHAR*)L"textures/terreno.bmp", (WCHAR*)L"textures/aguacon.jpg", 1, 1, camara->cameraDetails);
	glm::vec3 translate;
	glm::vec3 scale;
	glm::vec3 rotation;
	translate = glm::vec3(30.0f, 0.0f, 30.0f);
	water->setRotX(180);
	water->setTranslate(&translate);
	// load models
	// -----------
	ourModel.emplace_back(main);

	Model* arana; // Modelo #4 - Animación #2 - Idle, caminar y atacar
	arana = new Model("models/Arana/Arana.glb", main->cameraDetails);
	translate = glm::vec3(-60.0f, terreno->Superficie(0.0f, 0.0f) + 1, 10.0f);
	arana->setTranslate(&translate);
	arana->setNextTranslate(&translate);
	ourModel.emplace_back(arana);
	try {
		std::vector<Animation> animations = Animation::loadAllAnimations("models/Arana/Idle.glb", arana->GetBoneInfoMap(), arana->getBonesInfo(), arana->GetBoneCount());
		std::vector<Animation> animation2 = Animation::loadAllAnimations("models/Arana/Caminar.glb", arana->GetBoneInfoMap(), arana->getBonesInfo(), arana->GetBoneCount());
		std::vector<Animation> animation3 = Animation::loadAllAnimations("models/Arana/Atacar.glb", arana->GetBoneInfoMap(), arana->getBonesInfo(), arana->GetBoneCount());
		std::move(animation2.begin(), animation2.end(), std::back_inserter(animations));
		std::move(animation3.begin(), animation3.end(), std::back_inserter(animations));
		for (Animation animation : animations)
			arana->setAnimator(Animator(animation));
		arana->setAnimation(0);
	}
	catch (...) {
		ERRORL("Could not load animation!", "ANIMACION");
	}

	// Modelo #6 - Los dos modelos con posicion aleatoria 
	Model* piston1;
	piston1 = new Model("models/Piston/Piston.glb", main->cameraDetails);
	translate = glm::vec3(-55.0f, terreno->Superficie(0.0f, 0.0f) + 0.1, 0.0f);
	scale = glm::vec3(4.0f, 4.0f, 4.0f);
	piston1->setTranslate(&translate);
	piston1->setNextTranslate(&translate);
	piston1->setScale(&scale);
	ourModel.emplace_back(piston1);
	Model* piston2;
	piston2 = new Model("models/Piston/Piston.glb", main->cameraDetails);
	translate = glm::vec3(-48.0f, terreno->Superficie(0.0f, 0.0f) + 0.1, 0.0f);
	scale = glm::vec3(4.0f, 4.0f, 4.0f);
	piston2->setTranslate(&translate);
	piston2->setNextTranslate(&translate);
	piston2->setScale(&scale);
	ourModel.emplace_back(piston2);

	// Interior #2
	Model* interior;
	interior = new Model("models/Casa/Interior2.fbx", main->cameraDetails);
	translate = glm::vec3(5.0f, terreno->Superficie(10.0f, 16.0f) + 4.6, 5.0f);
	scale = glm::vec3(2.5f, 2.5f, 2.5f);
	interior->setTranslate(&translate);
	interior->setNextTranslate(&translate);
	interior->setNextRotX(270);
	interior->setNextRotZ(90);
	interior->setScale(&scale);
	interior->getModelAttributes()->at(0).hitbox = NULL;
	ourModel.emplace_back(interior);

	// Modelo #11 - Animación #5 (inorgánica)
	Model* ventilador;
	ventilador = new Model("models/ModelosInterior/Ventilador.fbx", main->cameraDetails);
	translate = glm::vec3(-3.0f, terreno->Superficie(10.0f, 16.0f) + 6.5, 15.0f);
	scale = glm::vec3(2.5f, 2.5f, 2.5f);
	ventilador->setTranslate(&translate);
	ventilador->setNextTranslate(&translate);
	ventilador->setScale(&scale);
	ventilador->getModelAttributes()->at(0).hitbox = NULL;
	ourModel.emplace_back(ventilador);
	//INFO("Bone count: " + std::to_string(ventilador->GetBoneCount()), "DEBUG");
	try {
		std::vector<Animation> animations = Animation::loadAllAnimations("models/ModelosInterior/Girar.fbx", ventilador->GetBoneInfoMap(), ventilador->getBonesInfo(), ventilador->GetBoneCount());
		for (Animation animation : animations)
			ventilador->setAnimator(Animator(animation));
		ventilador->setAnimation(0);
	}
	catch (...) {
		ERRORL("Could not load animation!", "ANIMACION");
	}
	ventilador->setNextRotX(0);
	ventilador->setNextRotY(0);
	ventilador->setNextRotZ(0);
	glm::vec3 fixScale = glm::vec3(0.02f, 0.02f, 0.02f);
	ventilador->setScale(&fixScale);

	// Modelo #12
	Model* cocoon;
	cocoon = new Model("models/ModelosInterior/Cocoon.fbx", main->cameraDetails);
	translate = glm::vec3(-14.0f, terreno->Superficie(10.0f, 16.0f) + 7, 20.0f);
	scale = glm::vec3(2.5f, 2.5f, 2.5f);
	cocoon->setTranslate(&translate);
	cocoon->setNextTranslate(&translate);
	cocoon->setNextRotX(90);
	cocoon->setNextRotZ(45);
	cocoon->setScale(&scale);
	ourModel.emplace_back(cocoon);

	// Modelo #13 
	Model* huevos;
	huevos = new Model("models/ModelosInterior/Huevos.fbx", main->cameraDetails);
	translate = glm::vec3(-2.0f, terreno->Superficie(10.0f, 16.0f), -2.0f);
	scale = glm::vec3(2.5f, 2.5f, 2.5f);
	huevos->setTranslate(&translate);
	huevos->setNextTranslate(&translate);
	huevos->setNextRotX(270);
	huevos->setNextRotZ(45);
	huevos->setScale(&scale);
	ourModel.emplace_back(huevos);

	// Modelo #14 
	Model* cama;
	cama = new Model("models/ModelosInterior/Cama.fbx", main->cameraDetails);
	translate = glm::vec3(16.0f, terreno->Superficie(10.0f, 16.0f), 5.0f);
	scale = glm::vec3(2.5f, 2.5f, 2.5f);
	cama->setTranslate(&translate);
	cama->setNextTranslate(&translate);
	cama->setNextRotX(270);
	cama->setNextRotZ(270);
	cama->setScale(&scale);
	ourModel.emplace_back(cama);

	// Modelo #15 
	Model* mueble;
	mueble = new Model("models/ModelosInterior/Mueble.fbx", main->cameraDetails);
	translate = glm::vec3(-10.0f, terreno->Superficie(10.0f, 16.0f) + 3, 26.5f);
	scale = glm::vec3(2.5f, 2.5f, 2.5f);
	mueble->setTranslate(&translate);
	mueble->setNextTranslate(&translate);
	mueble->setNextRotX(270);
	mueble->setNextRotZ(180);
	mueble->setScale(&scale);
	ourModel.emplace_back(mueble);

	inicializaBillboards();
	std::wstring prueba(L"Objetivo: Encuentra los pistones de tu moto y huye");
	ourText.emplace_back(new Texto(prueba, 20, 0, 0, SCR_HEIGHT, 0, camara));
}

void Interior::inicializaBillboards() {
	float ye = terreno->Superficie(0, 0);                                         //      x        y   z
	billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Bosque/Arbol1.png", 15, 30, -20, ye - 5, 50, camara->cameraDetails));

	ye = terreno->Superficie(-9, -15);
	billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Bosque/Arbol1.png", 7, 15, -9, ye - 5, -15, camara->cameraDetails));

	ye = terreno->Superficie(0, 0);
	billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Bosque/Arbol1.png", 40, 60, 30, ye - 8, 70, camara->cameraDetails));

	ye = terreno->Superficie(0, 0);
	billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Bosque/Arbol1.png", 40, 60, -30, ye - 8, 70, camara->cameraDetails));

	ye = terreno->Superficie(0, 0);
	billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Bosque/Arbol1.png", 50, 80, -5, ye - 10, 95, camara->cameraDetails));

	ye = terreno->Superficie(0, 0);
	billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Bosque/Arbol2.png", 30, 40, -22, ye - 3, 70, camara->cameraDetails));

	ye = terreno->Superficie(0, 0);
	billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Bosque/Arbol2.png", 35, 50, 15, ye - 3, 80, camara->cameraDetails));

	//Animación Billboard
	/*
	BillboardAnimation *billBoardAnimated = new BillboardAnimation();
	ye = terreno->Superficie(5, -5);
	for (int frameArbol = 1; frameArbol < 4; frameArbol++){
		wchar_t textura[50] = {L"billboards/Arbol"};
		if (frameArbol != 1){
			wchar_t convert[25];
			swprintf(convert, 25, L"%d", frameArbol);
			wcscat_s(textura, 50, convert);
		}
		wcscat_s(textura, 50, L".png");
		billBoardAnimated->pushFrame(new Billboard((WCHAR*)textura, 6, 6, 5, ye - 1, -5, camara->cameraDetails));
	}
	billBoardAnim.emplace_back(billBoardAnimated); */
}

//el metodo render toma el dispositivo sobre el cual va a dibujar
//y hace su tarea ya conocida
Scene* Interior::Render() {
	//borramos el biffer de color y el z para el control de profundidad a la 
	//hora del render a nivel pixel.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	//	glClearColor(255.0f, 255.0f, 255.0f, 255.0f);

	if (this->animacion > 10) { // Timer se ejecuta cada 1000/30 = 33.333 ms
		for (BillboardAnimation* b : billBoardAnim) {
			b->nextAnimation();
		}
		this->animacion = 0;
	}
	else {
		animacion = animacion + (1 * gameTime.deltaTime / 100);
	}
	// Decimos que dibuje la media esfera
	sky->Draw();
	// Ahora el terreno
	terreno->Draw();
	water->Draw();
	// Dibujamos cada billboard que este cargado en el arreglo de billboards.
	for (int i = 0; i < billBoard.size(); i++)
		billBoard[i]->Draw();
	for (int i = 0; i < billBoardAnim.size(); i++)
		billBoardAnim[i]->Draw();
	for (int i = 0; i < billBoard2D.size(); i++)
		billBoard2D[i]->Draw();
	// Dibujamos cada modelo que este cargado en nuestro arreglo de modelos
	for (int i = 0; i < ourModel.size(); i++) {
		ourModel[i]->Draw();
	}
	for (int i = 0; i < ourText.size(); i++) {
		ourText[i]->Draw();
	}
	// Le decimos a winapi que haga el update en la ventana
	return this;
}

std::vector<Model*>* Interior::getLoadedModels() {
	return &ourModel;
}
std::vector<Billboard*>* Interior::getLoadedBillboards() {
	return &billBoard;
}
std::vector<Billboard2D*>* Interior::getLoadedBillboards2D() {
	return &billBoard2D;
}
std::vector<Texto*>* Interior::getLoadedText() {
	return &ourText;
}
std::vector<BillboardAnimation*>* Interior::getLoadedBillboardsAnimation() {
	return &billBoardAnim;
}

Model* Interior::getMainModel() {
	return this->camara;
}
void Interior::setMainModel(Model* mainModel) {
	this->camara = mainModel;
}
float Interior::getAngulo() {
	return this->angulo;
}
void Interior::setAngulo(float angulo) {
	this->angulo = angulo;
}
SkyDome* Interior::getSky() {
	return sky;
}
Terreno* Interior::getTerreno() {
	return terreno;
}

Interior::~Interior() {
	if (this->sky != NULL) {
		delete this->sky;
		this->sky = NULL;
	}
	if (this->terreno != NULL) {
		delete this->terreno;
		this->terreno = NULL;
	}
	if (billBoard.size() > 0)
		for (int i = 0; i < billBoard.size(); i++)
			delete billBoard[i];
	if (billBoardAnim.size() > 0)
		for (int i = 0; i < billBoardAnim.size(); i++)
			delete billBoardAnim[i];
	if (billBoard2D.size() > 0)
		for (int i = 0; i < billBoard2D.size(); i++)
			delete billBoard2D[i];
	this->billBoard.clear();
	if (ourText.size() > 0)
		for (int i = 0; i < ourText.size(); i++)
			if (!(ourText[i]->name.compare("FPSCounter") || ourText[i]->name.compare("Coordenadas")))
				delete ourText[i];
	this->ourText.clear();
	if (ourModel.size() > 0)
		for (int i = 0; i < ourModel.size(); i++)
			if (ourModel[i] != camara)
				delete ourModel[i];
	this->ourModel.clear();
}
