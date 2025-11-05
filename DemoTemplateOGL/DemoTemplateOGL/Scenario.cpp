#include "Scenario.h"
#ifdef __linux__ 
#define ZeroMemory(x,y) memset(x,0,y)
#define wcscpy_s(x,y,z) wcscpy(x,z)
#define wcscat_s(x,y,z) wcscat(x,z)
#endif

Scenario::Scenario(Camera *cam) {
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
Scenario::Scenario(Model *camIni) {
    InitGraph(camIni);
}
void Scenario::InitGraph(Model *main) {
    float matAmbient[] = { 1,1,1,1 };
	float matDiff[] = { 1,1,1,1 };
	angulo = 0;
	camara = main;
	//creamos el objeto skydome
	sky = new SkyDome(32, 32, 20, (WCHAR*)L"skydome/night3.jpg", main->cameraDetails);
	//creamos el terreno
	terreno = new Terreno((WCHAR*)L"skydome/algo3.png", (WCHAR*)L"skydome/texnieve.jpg", 400, 400, main->cameraDetails);
	water = new Water((WCHAR*)L"textures/terreno.bmp", (WCHAR*)L"textures/aguacon.jpg", 200, 200, camara->cameraDetails);
	glm::vec3 translate;
	glm::vec3 scale;
	glm::vec3 rotation;
	translate = glm::vec3(30.0f, 7.0f, 30.0f);
	water->setRotX(180);
	water->setTranslate(&translate);
	// load models
	// -----------
	ourModel.emplace_back(main);

	Model* cabana; // Interior #1
	cabana = new Model("models/Casa/Cabana2.fbx", main->cameraDetails);
	translate = glm::vec3(0.0f, terreno->Superficie(10.0f, 16.0f)+3, 80.0f);
	scale = glm::vec3(2.0f, 2.0f, 2.0f);
	cabana->setTranslate(&translate);
	cabana->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 1.0f); 
	cabana->setNextRotX(270); 
	cabana->setNextRotZ(180);
	cabana->setScale(&scale);
	ourModel.emplace_back(cabana);

	// Desde este punto, hitbox manuales :0
	cabana->getModelAttributes()->at(0).hitbox = NULL;
	glm::vec3 posPuerta = glm::vec3(-2.0f, terreno->Superficie(10.0f, 16.0f)+1, 76.5f);
	Model* puertaHitbox = new CollitionBox(posPuerta, cabana->AABBsize, main->cameraDetails);
	scale = glm::vec3(0.3f, 0.7f, 0.2f);
	puertaHitbox->name = "PuertaCabana"; 
	puertaHitbox->setNextTranslate(puertaHitbox->getTranslate());
	puertaHitbox->setScale(&scale);
	ourModel.emplace_back(puertaHitbox);
	
	Model* moto; // Modelo #2
	moto = new Model("models/Moto/moto.fbx", main->cameraDetails);
	translate = glm::vec3(-5.0f, terreno->Superficie(0.0f, 60.0f) + 0.7, 60.0f);
	moto->setTranslate(&translate);
	moto->setNextTranslate(&translate);
	scale = glm::vec3(0.7f, 0.7f, 0.7f);
	moto->setNextRotY(135); // 180 rotation
	moto->setScale(&scale);
	ourModel.emplace_back(moto);

	Model* roca1; // Modelo #3 - 3 rocas
	roca1 = new Model("models/Bosque/Roca1.fbx", main->cameraDetails);
	translate = glm::vec3(30.0f, terreno->Superficie(0.0f, 40.0f) + 4, 50.0f);
	scale = glm::vec3(4.0f, 4.0f, 4.0f);	
	roca1->setTranslate(&translate);
	roca1->setNextTranslate(&translate);
	roca1->setNextRotZ(90);
	roca1->setNextRotY(270);
	roca1->setScale(&scale);
	ourModel.emplace_back(roca1);

	Model* roca2; // #3
	roca2 = new Model("models/Bosque/Roca2.fbx", main->cameraDetails);
	translate = glm::vec3(30.0f, terreno->Superficie(0.0f, 40.0f) + 1, 70.0f);
	scale = glm::vec3(5.0f, 2.0f, 5.0f);
	roca2->setTranslate(&translate);
	roca2->setNextTranslate(&translate);
	roca2->setNextRotY(90);
	roca2->setScale(&scale);
	ourModel.emplace_back(roca2);

	Model* roca3; // #3
	roca3 = new Model("models/Bosque/Roca3.fbx", main->cameraDetails);
	translate = glm::vec3(-20.0f, terreno->Superficie(0.0f, 40.0f) + 2, 30.0f);
	scale = glm::vec3(2.0f, 4.0f, 4.0f);
	roca3->setTranslate(&translate);
	roca3->setNextTranslate(&translate);
	roca3->setNextRotX(270);
	rotation = glm::vec3(0.0f, 1.0f, 0.0f);
	roca3->setNextRotY(270);
	roca3->setScale(&scale);
	ourModel.emplace_back(roca3);

	Model* perro; // Modelo #5 - Animaci�n #3 - Idle
	perro = new Model("models/Perro/Perro.glb", main->cameraDetails);
	translate = glm::vec3(10.0f, terreno->Superficie(10.0f, 60.0f), 60.0f);
	scale = glm::vec3(2.0f, 2.0f, 2.0f);
	perro->setTranslate(&translate);
	perro->setNextTranslate(&translate);
	perro->setNextRotY(225);
	perro->setScale(&scale);
	ourModel.emplace_back(perro);
	try {
		std::vector<Animation> animations = Animation::loadAllAnimations("models/Perro/Idle.glb", perro->GetBoneInfoMap(), perro->getBonesInfo(), perro->GetBoneCount());
		for (Animation animation : animations)
			perro->setAnimator(Animator(animation));
		perro->setAnimation(0);
	}
	catch (...) {
		ERRORL("Could not load animation!", "ANIMACION");
	}

	// Modelo #7
	Model* lampara; 
	lampara = new Model("models/Lampara/Lampara.fbx", main->cameraDetails);
	translate = glm::vec3(14.0f, terreno->Superficie(0.0f, 40.0f) + 9, 10.0f);
	scale = glm::vec3(2.0f, 2.0f, 2.0f);
	lampara->setTranslate(&translate);
	lampara->setNextTranslate(&translate);
	lampara->setNextRotX(270);
	lampara->setScale(&scale);
	ourModel.emplace_back(lampara);

	// Modelo #8
	Model* valla;
	valla = new Model("models/Lampara/Valla.fbx", main->cameraDetails);
	translate = glm::vec3(10.0f, terreno->Superficie(0.0f, 0.0f) + 1.3, 20.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.3f);
	valla->setTranslate(&translate);
	valla->setNextTranslate(&translate);
	valla->setNextRotX(0);   
	valla->setNextRotY(0);
	valla->setNextRotZ(0);
	valla->setNextRotX(90);
	valla->setNextRotZ(90);
	valla->setScale(&scale);
	ourModel.emplace_back(valla);
	Model* valla2;
	valla2 = new Model("models/Lampara/Valla.fbx", main->cameraDetails);
	translate = glm::vec3(-10.0f, terreno->Superficie(0.0f, 0.0f) + 1.3, 20.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.3f);
	valla2->setTranslate(&translate);
	valla2->setNextTranslate(&translate);
	valla2->setNextRotX(0);
	valla2->setNextRotY(0);
	valla2->setNextRotZ(0);
	valla2->setNextRotX(90);
	valla2->setNextRotZ(90);
	valla2->setScale(&scale);
	ourModel.emplace_back(valla2);

	// Modelo #9
	Model* pizza; 
	pizza = new Model("models/Pizza/Pizza.fbx", main->cameraDetails);
	translate = glm::vec3(-8.0f, terreno->Superficie(0.0f, 60.0f) + 1.6, 57.0f);
	pizza->setTranslate(&translate);
	pizza->setNextTranslate(&translate);
	scale = glm::vec3(2.0f, 2.0f, 2.0f);
	pizza->setNextRotZ(135);
	pizza->setNextRotX(270);
	pizza->setScale(&scale);
	pizza->getModelAttributes()->at(0).hitbox = NULL;
	ourModel.emplace_back(pizza);

	// Modelo #10 - Animaci�n #4 (inorg�nica)
	Model* antena;
	antena = new Model("models/Bosque/Antena.fbx", main->cameraDetails);
	translate = glm::vec3(4.0f, terreno->Superficie(10.0f, 16.0f) + 10, 80.0f);
	antena->setTranslate(&translate);
	antena->setNextTranslate(&translate);
	scale = glm::vec3(2.0f, 2.0f, 2.0f);
	antena->setScale(&scale);
	antena->getModelAttributes()->at(0).hitbox = NULL;
	ourModel.emplace_back(antena);
	try {
		std::vector<Animation> animations = Animation::loadAllAnimations("models/Bosque/AntenaGirar.fbx", antena->GetBoneInfoMap(), antena->getBonesInfo(), antena->GetBoneCount());
		for (Animation animation : animations)
			antena->setAnimator(Animator(animation));
		antena->setAnimation(0);
	}
	catch (...) {
		ERRORL("Could not load animation!", "ANIMACION");
	}
	antena->setNextRotX(0);
	antena->setNextRotY(0);
	antena->setNextRotZ(0);
	glm::vec3 fixScale = glm::vec3(0.02f, 0.02f, 0.02f);
	antena->setScale(&fixScale);
	

	// Cosas para m�s tarde
	/* 
	ModelAttributes m;
	Model *pez = new Model("models/pez/pez.obj", main->cameraDetails);
	translate = glm::vec3(0.0f, terreno->Superficie(0.0f, 50.0f), 50.0f);
	pez->setNextTranslate(&translate);
	pez->setTranslate(&translate);
	ourModel.emplace_back(pez);
	model = CollitionBox::GenerateAABB(m.translate, pez->AABBsize, main->cameraDetails);
	m.hitbox = model;
	pez->getModelAttributes()->push_back(m);
	translate.x = 5;
	pez->setTranslate(&translate, pez->getModelAttributes()->size()-1);
	pez->setNextTranslate(&translate, pez->getModelAttributes()->size()-1);
	model = CollitionBox::GenerateAABB(m.translate, pez->AABBsize, main->cameraDetails);
	m.hitbox = model; // Le decimos al ultimo ModelAttribute que tiene un hitbox asignado
	pez->getModelAttributes()->push_back(m);
	translate.x = 10;
	pez->setTranslate(&translate, pez->getModelAttributes()->size()-1);
	pez->setNextTranslate(&translate, pez->getModelAttributes()->size()-1);

	Model* silly = new Model("models/Silly_Dancing/Silly_Dancing.fbx", main->cameraDetails);
	translate = glm::vec3(10.0f, terreno->Superficie(10.0f, 60.0f) , 60.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
	silly->setTranslate(&translate);
	silly->setNextTranslate(&translate);
	silly->setScale(&scale);
	silly->setNextRotY(180);
	ourModel.emplace_back(silly);
	try{
		std::vector<Animation> animations = Animation::loadAllAnimations("models/Silly_Dancing/Silly_Dancing.fbx", silly->GetBoneInfoMap(), silly->getBonesInfo(), silly->GetBoneCount());
		for (Animation animation : animations)
			silly->setAnimator(Animator(animation));
		silly->setAnimation(0);
	}catch(...){
		ERRORL("Could not load animation!", "ANIMACION");
	}
	model = CollitionBox::GenerateAABB(translate, silly->AABBsize, main->cameraDetails);
	m.hitbox = model; // Le decimos al ultimo ModelAttribute que tiene un hitbox asignado
	silly->getModelAttributes()->push_back(m);
	translate.x += 10;
	silly->setTranslate(&translate, silly->getModelAttributes()->size()-1);
	silly->setNextTranslate(&translate, silly->getModelAttributes()->size()-1);
	silly->setScale(&scale, silly->getModelAttributes()->size()-1);
	silly->setNextRotY(180, silly->getModelAttributes()->size()-1);
	silly->setRotY(180, silly->getModelAttributes()->size()-1);
	// Import model and clone with bones and animations
	model = new Model("models/Silly_Dancing/Silly_Dancing.fbx", main->cameraDetails);
	translate = glm::vec3(30.0f, terreno->Superficie(30.0f, 60.0f) , 60.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
	model->name = "Silly_Dancing1";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotY(180);
	ourModel.emplace_back(model);
	// Para clonar la animacion se eliminan los huesos del modelo actual y se copian los modelos y animators
	model->GetBoneInfoMap()->clear();
	model->getBonesInfo()->clear();
	*model->GetBoneInfoMap() = *silly->GetBoneInfoMap();
	*model->getBonesInfo() = *silly->getBonesInfo();
	model->setAnimator(silly->getAnimator());

	//	model = new Model("models/IronMan.obj", main->cameraDetails);
//	translate = glm::vec3(0.0f, 20.0f, 30.0f);
//	scale = glm::vec3(0.025f, 0.025f, 0.025f);	// it's a bit too big for our scene, so scale it down
//	model->setScale(&scale);
//	model->setTranslate(&translate);
//	ourModel.emplace_back(model);
	model = new Model("models/backpack/backpack.obj", main->cameraDetails, false, false);
	translate = glm::vec3(20.0f, terreno->Superficie(20.0f, 0.0f) + 2, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	ourModel.emplace_back(model);
	model->lightColor = glm::vec3(10,0,0);
	
	model = new CollitionBox(60.0f, 15.0f, 10.0f, 10, 10, 10, main->cameraDetails);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
	model->setNextTranslate(model->getTranslate());
	model->setScale(&scale);
	ourModel.emplace_back(model); */
	

	inicializaBillboards();
	std::wstring prueba(L"Objetivo: Refugiarse de la tormenta");
	ourText.emplace_back(new Texto(prueba, 20, 0, 0, SCR_HEIGHT, 0, camara));
	//billBoard2D.emplace_back(new Billboard2D((WCHAR*)L"billboards/awesomeface.png", 6, 6, 50, 50, 0, camara->cameraDetails));
	//scale = glm::vec3(100.0f, 100.0f, 0.0f);	// it's a bit too big for our scene, so scale it down
	//billBoard2D.back()->setScale(&scale);
	}

void Scenario::inicializaBillboards() {
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

	//Animaci�n Billboard
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
Scene* Scenario::Render() {
	//borramos el biffer de color y el z para el control de profundidad a la 
	//hora del render a nivel pixel.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
//	glClearColor(255.0f, 255.0f, 255.0f, 255.0f);

	if (this->animacion > 10) { // Timer se ejecuta cada 1000/30 = 33.333 ms
		for (BillboardAnimation *b : billBoardAnim){
			b->nextAnimation();
		}
		this->animacion = 0;
	} else {
		animacion = animacion + (1 * gameTime.deltaTime/100);
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
	
std::vector<Model*> *Scenario::getLoadedModels() {
	return &ourModel;
}
std::vector<Billboard*> *Scenario::getLoadedBillboards() {
	return &billBoard;
}
std::vector<Billboard2D*> *Scenario::getLoadedBillboards2D(){
	return &billBoard2D;
}
std::vector<Texto*> *Scenario::getLoadedText(){
	return &ourText;
}
std::vector<BillboardAnimation*> *Scenario::getLoadedBillboardsAnimation(){
	return &billBoardAnim;
}

Model* Scenario::getMainModel() {
	return this->camara;
}
void Scenario::setMainModel(Model* mainModel){
	this->camara = mainModel;
}
float Scenario::getAngulo() {
	return this->angulo;
}
void Scenario::setAngulo(float angulo) {
	this->angulo = angulo;
}
SkyDome* Scenario::getSky() {
	return sky;
}
Terreno* Scenario::getTerreno() {
	return terreno;
}

Scenario::~Scenario() {
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
