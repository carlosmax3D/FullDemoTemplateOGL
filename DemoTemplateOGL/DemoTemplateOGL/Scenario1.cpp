#include "Scenario1.h"
//#include "Scene.h"
/*#include "Base/model.h"
#include "SkyDome.h"
#include "Terreno.h"
#include "Billboard.h"
#include "BillboardAnimation.h"
#include "Billboard2D.h"
#include "Texto.h"
#include "Water.h"
#include "Animator.h"
#include "Animation.h"*/

#ifdef __linux__ 
#define ZeroMemory(x,y) memset(x,0,y)
#define wcscpy_s(x,y,z) wcscpy(x,z)
#define wcscat_s(x,y,z) wcscat(x,z)
#endif

Scenario1::Scenario1(Camera* cam) {
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
Scenario1::Scenario1(Model* camIni) {
	InitGraph(camIni);
}
/*
virtual int update() {
	float angulo = getAngulo() + 1.5 * gameTime.deltaTime / 100;
	angulo = angulo >= 360 ? angulo - 360.0 : angulo;
	setAngulo(angulo);
	//getSky()->setRotY(angulo);
	Model* camara = getMainModel();

	//creamos una variable para hacer el tp
	int tp = -1;

	for (int i = 0; i < getLoadedModels()->size(); i++) {
		auto it = getLoadedModels()->begin() + i;
		Model* collider = NULL, * model = *it;
		int idxCollider = -1;
		bool objInMovement = (*model->getNextTranslate()) != (*model->getTranslate());
		glm::vec3& posM = objInMovement ? *model->getNextTranslate() : *model->getTranslate();
		glm::vec3 ejeColision = glm::vec3(0);
		bool isPrincipal = model == camara; // Si es personaje principal, activa gravedad
		ModelCollider mcollider = model->update(getTerreno()->Superficie(posM.x, posM.z), *getLoadedModels(), ejeColision, isPrincipal);
		if (mcollider.model != NULL) {
			collider = (Model*)mcollider.model;
			idxCollider = mcollider.attrIdx;
		}

		//preguntar por el nombre del modelo para sacar la colision y hacer el tp al otro escenario
		if (collider != NULL && model == camara) {
			if (collider->name.compare("casa") == 0) {
				tp = 1;
			}
			if (ejeColision.y == 1) {
				INFO("APLASTADO!!!!", "JUMP HITBOX");
				if (removeCollideModel(collider, idxCollider))
					i--;
			}

		}
		if (i < 0) i = 0;
	}
	// Actualizamos la camara
	//camara->cameraDetails->CamaraUpdate(camara->getRotZ(), camara->getTranslate());
	camara->cameraDetails->CamaraUpdate(camara->getRotY(), camara->getTranslate());
	return tp;
}
*/
void Scenario1::InitGraph(Model* main) {
	float matAmbient[] = { 1,1,1,1 };
	float matDiff[] = { 1,1,1,1 };
	angulo = 0;
	camara = main;
	//creamos el objeto skydome
	sky = new SkyDome(32, 32, 20, (WCHAR*)L"skydome/orangesky.png", main->cameraDetails);
	//creamos el terreno
	terreno = new Terreno((WCHAR*)L"skydome/terr_int5.png", (WCHAR*)L"skydome/interior.png", 400, 400, main->cameraDetails);
	

	water = new Water((WCHAR*)L"textures/terreno.bmp", (WCHAR*)L"textures/water.bmp", 20, 20, camara->cameraDetails);
	glm::vec3 translate;
	glm::vec3 scale;
	glm::vec3 rotation;
	translate = glm::vec3(0.0f, 20.0f, 30.0f);
	water->setTranslate(&translate);
	// load models
	// -----------
	ourModel.emplace_back(main);

	Model* model;
	

	/*exportar objetos*/
	/*NOTAS MIAS
	LA Y REPRESENTA LA ALTURA, LA X ES DE ADELANTE ATRAS*/


	/*INTERACTUABLES
	----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*libro*/
	Model* libro;
	libro = new Model("models/Objetos/libro.fbx", main->cameraDetails);
	translate = glm::vec3(32.5f, terreno->Superficie(20.0f, 0.0f) + 2.2, 43.0f);
	libro->setTranslate(&translate);
	libro->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	libro->setNextRotX(90); // 45� rotation
	scale = glm::vec3(1.50f, 1.50f, 1.50f);
	libro->setScale(&scale);
	ourModel.emplace_back(libro);
	//anular la colision
	//libro->getModelAttributes()->at(0).hitbox = NULL;

	//cochecito

	model = new Model("models/Objetos/carrito.fbx", main->cameraDetails);
	translate = glm::vec3(65.0f, terreno->Superficie(20.0f, 0.0f) + 0.5, 28.0f);
	scale = glm::vec3(0.35f, 0.35f, 0.35f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(90);
	ourModel.emplace_back(model);

	/*foto*/
	
	model = new Model("models/Objetos/picture/source/foto.fbx", main->cameraDetails);
	translate = glm::vec3(70.0f, terreno->Superficie(20.0f, 0.0f) + 3.4, -2.0f);
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	//model->setNextRotX(90); // 45� rotation
	scale = glm::vec3(.050f, .050f, .050f);
	model->setScale(&scale);
	model->setNextRotX(90);
	model->setNextRotZ(90);
	ourModel.emplace_back(model);
	//anular la colision
	//model->getModelAttributes()->at(0).hitbox = NULL;
	/*---------------------------------------------------------------------------------------------------------------------------------------------------- * /
	/*CASA
	----------------------------------------------------------------------------------------------------------------------------------------------------*/

	//silla
	model = new Model("models/casa/silla.fbx", main->cameraDetails);
	translate = glm::vec3(60.0f, terreno->Superficie(20.0f, 0.0f) + 0.6, 0.0f);
	scale = glm::vec3(.50f, .50f, .50f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(120);
	model->setNextRotY(15);
	ourModel.emplace_back(model);


	//jarron
	model = new Model("models/casa/jarron.fbx", main->cameraDetails);
	translate = glm::vec3(70.0f, terreno->Superficie(20.0f, 0.0f)+2.3, -5.0f);
	scale = glm::vec3(0.15f, 0.15f, 0.15f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(90);
	ourModel.emplace_back(model);
	//model->getModelAttributes()->at(0).hitbox = NULL;

	//mesa del jarron
	model = new Model("models/casa/mesajar.fbx", main->cameraDetails);
	translate = glm::vec3(70.0f, terreno->Superficie(20.0f, 0.0f) + 0.2, -5.0f);
	scale = glm::vec3(2.0f, 2.0f, 2.0f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(90);
	ourModel.emplace_back(model);
	//model->getModelAttributes()->at(0).hitbox = NULL;

	//alfombra
	model = new Model("models/casa/alfombra.fbx", main->cameraDetails);
	translate = glm::vec3(50.0f, terreno->Superficie(10.0f, 0.0f) +0.16, 0.0f);
	scale = glm::vec3(2.0f, 2.0f, 2.0f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(90);
	ourModel.emplace_back(model);
	model->getModelAttributes()->at(0).hitbox = NULL;


	//silla giratoria
	model = new Model("models/silla/office-desk-chair/source/DC/silla2.fbx", main->cameraDetails);
	translate = glm::vec3(32.0f, terreno->Superficie(20.0f, 0.0f) + 1.5, 51.0f);
	scale = glm::vec3(2.0f, 2.5f, 3.0f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(90);
	ourModel.emplace_back(model);
	//model->getModelAttributes()->at(0).hitbox = NULL;


	//escritorio
	model = new Model("models/escritorio/desk/source/escritorio.fbx", main->cameraDetails);
	translate = glm::vec3(35.0f, terreno->Superficie(10.0f, 0.0f)+0.3 , 40.5f);
	scale = glm::vec3(3.0f, 2.5f, 3.0f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(270);
	model->setNextRotY(270);
	ourModel.emplace_back(model);
	//model->getModelAttributes()->at(0).hitbox = NULL;


	model = new Model("models/casa/source/sink2.fbx", main->cameraDetails);
	translate = glm::vec3(78.0f, terreno->Superficie(10.0f, 0.0f) + 1.3, 21.0f);
	scale = glm::vec3(3.0f, 2.0f, 3.0f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotY(90);

	ourModel.emplace_back(model);
	//model->getModelAttributes()->at(0).hitbox = NULL;

	//baño
	model = new Model("models/casa/vater.fbx", main->cameraDetails);
	translate = glm::vec3(74.0f, terreno->Superficie(10.0f, 0.0f) -0.3, 20.0f);
	scale = glm::vec3(0.03f, 0.03f, 0.03f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotY(180);
	ourModel.emplace_back(model);
	//model->getModelAttributes()->at(0).hitbox = NULL;

	//lavabo
	model = new Model("models/casa/sink.fbx", main->cameraDetails);
	translate = glm::vec3(35.0f, terreno->Superficie(10.0f, 0.0f) , 60.0f);
	scale = glm::vec3(1.2f, 01.3f, 1.90f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->getModelAttributes()->at(0).active = NULL; //con esto anulamos la carga del modelo de modo que no me joda el vater
	ourModel.emplace_back(model);
	model->getModelAttributes()->at(0).hitbox = NULL;

	
	//ducha
	model = new Model("models/casa/shower.fbx", main->cameraDetails);
	translate = glm::vec3(74.0f, terreno->Superficie(10.0f, 0.0f) + 3.5, 2.0f);
	scale = glm::vec3(3.0f, 3.0f, 3.0f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(90);
	ourModel.emplace_back(model);
	//model->getModelAttributes()->at(0).hitbox = NULL;
	/*-------------------------------------------------------------------------------------------------------------------------------------------------------
	
	/*---------------------------------------------------------------------------------------------------------------------------------------------------- * /
	/*modelos del template*/

	//probar colocar las paredes
	model = new Model("models/casa/paredes/pared1.fbx", main->cameraDetails);
	translate = glm::vec3(24.0f, terreno->Superficie(20.0f, 0.0f) + 5.5, 0.0f);
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	scale = glm::vec3(0.25f, 07.0f, 05.5f); // x es a los lados , y es de atras hacia adelante,  z es la altura
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(90);
	model->setRotX(270);
	model->setRotZ(90);
	model->ignoreAABB = true;
	model->walkeable = false;
	ourModel.emplace_back(model);

	model = new Model("models/casa/paredes/pared2.fbx", main->cameraDetails);
	translate = glm::vec3(17.0f, terreno->Superficie(20.0f, 0.0f) + 7, 28.0f); //z con 16 queda perfectamente alineado con respecto a la pared 1
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	scale = glm::vec3(0.65f, 0.05f, 0.48f); // z es el largo de la pared
	model->setScale(&scale);
	model->setNextRotZ(90);
	model->setRotZ(90);
	ourModel.emplace_back(model);
	model->ignoreAABB = true;
	model->walkeable = false;

	
	model = new Model("models/casa/paredes/pared3.fbx", main->cameraDetails);
	translate = glm::vec3(26.5f, terreno->Superficie(20.0f, 0.0f) + 5, 57.0f);//x dejar en 27, z en 35
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
	scale = glm::vec3(0.5f, 10.05f, 5.28f); // x es el grosor aqui, y son los lados y z la altura
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(90);
	model->setRotX(270);
	model->setRotZ(90);
	ourModel.emplace_back(model);
	model->ignoreAABB = true;
	model->walkeable = false;

	//duplicamos la pared pero sin colision para crear la pared interna

	model = new Model("models/casa/paredes/pared31.fbx", main->cameraDetails);
	translate = glm::vec3(26.5f, terreno->Superficie(20.0f, 0.0f) + 5, 38.0f);//x dejar en 27, z en 35
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
	scale = glm::vec3(0.5f, 10.05f, 5.28f); // x es el grosor aqui, y son los lados y z la altura
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(90);
	model->setRotX(270);
	model->setRotZ(90);
	ourModel.emplace_back(model);
//	model->getModelAttributes()->at(0).hitbox = NULL; 
	model->ignoreAABB = true;
	model->walkeable = false;

	model = new Model("models/casa/paredes/pared4.fbx", main->cameraDetails);
	translate = glm::vec3(36.5f, terreno->Superficie(20.0f, 0.0f) + 5, 48.0f);//x dejar en 27, z en 35
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	scale = glm::vec3(0.5f, 10.05f, 5.28f); // x es el grosor aqui, y son los lados y z la altura
	model->setScale(&scale);
	model->setNextRotX(90);
	model->setNextRotZ(180);
	model->setRotX(90);
	model->setRotZ(180);
	ourModel.emplace_back(model);
	model->ignoreAABB = true;
	model->walkeable = false;

	/*
	model = new Model("models/casa/paredes/pared6.fbx", main->cameraDetails);
	translate = glm::vec3(70.0f, terreno->Superficie(20.0f, 0.0f) + 5, 30.0f);//desde el z = 26 da problemas con la colision, debo colocar que x = 73 para que no falle
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
	model->setNextRotX(270);
	model->setNextRotZ(90);
	scale = glm::vec3(05.2f, 0.41f, 05.4f); // x es el largo, z la altura
	model->setScale(&scale);
	ourModel.emplace_back(model);
	model->getModelAttributes()->at(0).hitbox = NULL; //revisar pq se acomoda mal
	model->ignoreAABB = true;
	model->walkeable = false;
	*/

	model = new Model("models/casa/paredes/pared61.fbx", main->cameraDetails);
	translate = glm::vec3(70.0f, terreno->Superficie(20.0f, 0.0f) + 5, 29.0f);
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
	model->setNextRotX(270);
	model->setNextRotZ(90);
	model->setRotX(270);
	model->setRotZ(90);
	//scale = glm::vec3(08.4f, 0.41f, 05.4f); // x es el largo, z la altura
	scale = glm::vec3(5.0f, 1.0f, 3.2f);
	model->setScale(&scale);
	ourModel.emplace_back(model);
	//model->getModelAttributes()->at(0).hitbox = NULL;
	model->ignoreAABB = true;
	model->walkeable = false;

	model = new Model("models/casa/paredes/pared5.fbx", main->cameraDetails);
	translate = glm::vec3(64.0f, terreno->Superficie(20.0f, 0.0f) + 6, 37.5f);//x dejar en 27, z en 35
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
	scale = glm::vec3(0.2f, 0.21f, 01.75f); // y es el largo , z la altura
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(90);
	model->setRotX(270);
	model->setRotZ(90);
	ourModel.emplace_back(model);
	model->ignoreAABB = true;
	model->walkeable = false;

	model = new Model("models/casa/paredes/pared7.fbx", main->cameraDetails);
	translate = glm::vec3(76.0f, terreno->Superficie(20.0f, 0.0f) + 5, 22.5f);//
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
	model->setNextRotX(270);
	model->setNextRotZ(90);
	model->setRotX(270);
	model->setRotZ(90);
	scale = glm::vec3(0.52f, 06.5f, 05.4f); // y es el largo, z la altura
	model->setScale(&scale);
	ourModel.emplace_back(model);
	//model->getModelAttributes()->at(0).hitbox = NULL; //revisar pq se acomoda mal
	model->ignoreAABB = true;
	model->walkeable = false;

	model = new Model("models/casa/paredes/pared8.fbx", main->cameraDetails);
	translate = glm::vec3(50.0f, terreno->Superficie(20.0f, 0.0f) + 4.5, 0.0f);
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	scale = glm::vec3(0.25f, 06.2f, 05.3f); // z es la altura, y es el largo
	model->setScale(&scale);
	model->setNextRotX(270);
	model->setNextRotZ(90);
	model->setRotX(270);
	model->setRotZ(90);
	ourModel.emplace_back(model);
	model->ignoreAABB = true;
	model->walkeable = false;

	//duplicamos la pared 6 para el fondo de una de las habitaciones
	model = new Model("models/casa/paredes/pared61.fbx", main->cameraDetails);
	translate = glm::vec3(82.0f, terreno->Superficie(20.0f, 0.0f) + 5, 11.0f);//desde el z = 26 da problemas con la colision, debo colocar que x = 85 para que no falle
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
	model->setNextRotX(270);
	model->setNextRotZ(90);
	model->setRotX(270);
	model->setRotZ(90);
	//scale = glm::vec3(08.4f, 0.41f, 05.4f); // x es el largo, z la altura
	scale = glm::vec3(7.8f, 1.0f, 3.2f);
	model->setScale(&scale);
	ourModel.emplace_back(model);
	//model->getModelAttributes()->at(0).hitbox = NULL; //revisar pq se acomoda mal
	model->ignoreAABB = true;
	model->walkeable = false;


	
	model = new Model("models/casa/paredes/pared9.fbx", main->cameraDetails);
	translate = glm::vec3(70.0f, terreno->Superficie(20.0f, 0.0f) + 5, 9.0f);//
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
	model->setNextRotX(270);
	model->setNextRotZ(90);
	model->setRotX(270);
	model->setRotZ(90);
	scale = glm::vec3(015.2f, 0.41f, 05.4f); // x es el largo, z la altura
	model->setScale(&scale);
	ourModel.emplace_back(model);
//	model->getModelAttributes()->at(0).hitbox = NULL; //revisar pq se acomoda mal*/
	model->ignoreAABB = true;
	model->walkeable = false;

	/* --------------------------------------------------------------------------------------------------------------------------------------------------- */
	//monstruo
	model = new Model("models/monstruo/catwalk-loop-378982.fbx", main->cameraDetails);
	//model->name = "monstruo";
	translate = glm::vec3(0.0f, terreno->Superficie(0.0f, 60.0f), 60.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);

	model->setRotX(180);
	model->setRotY(180);
	model->setNextRotX(270);
	model->setNextRotY(0);
	model->setNextRotZ(180);
	ourModel.emplace_back(model);
	try {
		std::vector<Animation> animations = Animation::loadAllAnimations("models/monstruo/catwalk-loop-378982.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::vector<Animation> animation = Animation::loadAllAnimations("models/monstruo/catwalk-loop-378982.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::move(animation.begin(), animation.end(), std::back_inserter(animations));
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(1);
	}
	catch (...) {
		ERRORL("Could not load animation!", "ANIMACION");
	}

	


	inicializaBillboards();
	std::wstring prueba(L"Esta es una prueba");
	ourText.emplace_back(new Texto(prueba, 20, 0, 0, SCR_HEIGHT, 0, camara));

	/*
	billBoard2D.emplace_back(new Billboard2D((WCHAR*)L"billboards/awesomeface.png", 6, 6, 100, 200, 0, camara->cameraDetails));
	scale = glm::vec3(100.0f, 100.0f, 0.0f);	// it's a bit too big for our scene, so scale it down
	billBoard2D.back()->setScale(&scale);*/
}

void Scenario1::inicializaBillboards() {
	float ye = terreno->Superficie(0, 0);
	//billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol.png", 6, 6, 0, ye - 1, 0, camara->cameraDetails));

	ye = terreno->Superficie(-9, -15);
	//billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol3.png", 8, 8, -9, ye - 1, -15, camara->cameraDetails));

	/*
	BillboardAnimation* billBoardAnimated = new BillboardAnimation();
	ye = terreno->Superficie(5, -5);
	for (int frameArbol = 1; frameArbol < 4; frameArbol++) {
		wchar_t textura[50] = { L"billboards/Arbol" };
		if (frameArbol != 1) {
			wchar_t convert[25];
			swprintf(convert, 25, L"%d", frameArbol);
			wcscat_s(textura, 50, convert);
		}
		wcscat_s(textura, 50, L".png");
		billBoardAnimated->pushFrame(new Billboard((WCHAR*)textura, 6, 6, 5, ye - 1, -5, camara->cameraDetails));
	}
	billBoardAnim.emplace_back(billBoardAnimated);*/
}

//el metodo render toma el dispositivo sobre el cual va a dibujar
//y hace su tarea ya conocida
Scene* Scenario1::Render() {
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

std::vector<Model*>* Scenario1::getLoadedModels() {
	return &ourModel;
}
std::vector<Billboard*>* Scenario1::getLoadedBillboards() {
	return &billBoard;
}
std::vector<Billboard2D*>* Scenario1::getLoadedBillboards2D() {
	return &billBoard2D;
}
std::vector<Texto*>* Scenario1::getLoadedText() {
	return &ourText;
}
std::vector<BillboardAnimation*>* Scenario1::getLoadedBillboardsAnimation() {
	return &billBoardAnim;
}

Model* Scenario1::getMainModel() {
	return this->camara;
}
void Scenario1::setMainModel(Model* mainModel) {
	this->camara = mainModel;
}
float Scenario1::getAngulo() {
	return this->angulo;
}
void Scenario1::setAngulo(float angulo) {
	this->angulo = angulo;
}
SkyDome* Scenario1::getSky() {
	return sky;
}
Terreno* Scenario1::getTerreno() {
	return terreno;
}

Scenario1::~Scenario1() {
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
