#include "Scenario.h"
#include "Ammo.h"
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
	scale = glm::vec3(0.25f, 0.25f, 0.25f); 	// it's a bit too big for our scene, so scale it down
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
	/*sky = new SkyDome(32, 32, 20, (WCHAR*)L"skydome/earth.jpg", main->cameraDetails);*/
	//creamos el terreno
	terreno = new Terreno((WCHAR*)L"skydome/terreno.jpg", (WCHAR*)L"skydome/texterr2.jpg", 400, 400, main->cameraDetails);
	water = new Water((WCHAR*)L"textures/terreno.bmp", (WCHAR*)L"textures/water.bmp", 450, 450, camara->cameraDetails);
	glm::vec3 translate;
	glm::vec3 scale;
	glm::vec3 rotation;
	translate = glm::vec3(0.0f, 13.0f, 115.0f);
	water->setRotX(180);
	water->setNextRotX(180); // 45� rotation
	water->setTranslate(&translate);
	//// load models
	//// -----------
	ourModel.emplace_back(main);
	Model* model;
	Model* fog;
	int idClone = 0;
	fog = new Model("models/fogata/fogata.obj", main->cameraDetails);
	translate = glm::vec3(-145.0f, terreno->Superficie(10.0f, 10.0f), 20.0f);
	scale = glm::vec3(0.5f, 0.5f, 0.5f);
	fog->setScale(&scale);
	fog->setTranslate(&translate);
	fog->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	fog->setNextRotX(0); // 45� rotation
	ourModel.emplace_back(fog);

	ModelAttributes mAttr = fog->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, fog->AABBsize, main->cameraDetails);
	fog->getModelAttributes()->push_back(mAttr);
	idClone = fog->getModelAttributes()->size() - 1;

	translate = glm::vec3(151.0f, terreno->Superficie(10.0f, 10.0f), -24.0f);
	scale = glm::vec3(0.5f, 0.5f, 0.5f);
	fog->setScale(&scale, idClone);
	fog->setTranslate(&translate, idClone);
	fog->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	fog->setNextRotX(0, idClone); // 45� rotation
	fog->setRotX(0, idClone); // 45� rotation

	//Modelos en medio de la nada

	mAttr = fog->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, fog->AABBsize, main->cameraDetails);
	fog->getModelAttributes()->push_back(mAttr);
	idClone = fog->getModelAttributes()->size() - 1;

	translate = glm::vec3(75.0f, terreno->Superficie(10.0f, 10.0f), 55.0f);
	scale = glm::vec3(0.5f, 0.5f, 0.5f);
	fog->setScale(&scale, idClone);
	fog->setTranslate(&translate, idClone);
	fog->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	fog->setNextRotX(0, idClone); // 45� rotation
	fog->setRotX(0, idClone); // 45� rotation

	Model* tCamp1;
	tCamp1 = new Model("models/Tienda_camp/tienda_camp.fbx", main->cameraDetails);
	translate = glm::vec3(85.0f, terreno->Superficie(10.0f, 10.0f), 45.0f);
	scale = glm::vec3(0.12f, 0.12f, 0.12f);
	tCamp1->setScale(&scale);
	tCamp1->setTranslate(&translate);
	tCamp1->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	tCamp1->setNextRotX(-90); // 45� rotation
	tCamp1->setNextRotZ(-90); // 45� rotation
	tCamp1->setNextRotY(90); // 45� rotation
	tCamp1->setRotX(-90); // 45� rotation
	tCamp1->setRotZ(-90); // 45� rotation
	tCamp1->setRotY(90); // 45� rotation
	ourModel.emplace_back(tCamp1);

	mAttr = tCamp1->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, tCamp1->AABBsize, main->cameraDetails);
	tCamp1->getModelAttributes()->push_back(mAttr);
	idClone = tCamp1->getModelAttributes()->size() - 1;

	translate = glm::vec3(85.0f, terreno->Superficie(10.0f, 10.0f), 65.0f);
	scale = glm::vec3(0.12f, 0.12f, 0.12f);
	tCamp1->setScale(&scale, idClone);
	tCamp1->setTranslate(&translate, idClone);
	tCamp1->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	tCamp1->setNextRotX(-90, idClone); // 45� rotation
	tCamp1->setNextRotZ(-90, idClone); // 45� rotation
	tCamp1->setNextRotY(90, idClone); // 45� rotation
	tCamp1->setRotX(-90, idClone); // 45� rotation
	tCamp1->setRotZ(-90, idClone); // 45� rotation
	tCamp1->setRotY(90, idClone); // 45� rotation

	mAttr = tCamp1->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, tCamp1->AABBsize, main->cameraDetails);
	tCamp1->getModelAttributes()->push_back(mAttr);
	idClone = tCamp1->getModelAttributes()->size() - 1;

	translate = glm::vec3(65.0f, terreno->Superficie(10.0f, 10.0f), 45.0f);
	scale = glm::vec3(0.12f, 0.12f, 0.12f);
	tCamp1->setScale(&scale, idClone);
	tCamp1->setTranslate(&translate, idClone);
	tCamp1->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	tCamp1->setNextRotX(90, idClone); // 45� rotation
	tCamp1->setNextRotZ(-90, idClone); // 45� rotation
	tCamp1->setNextRotY(-90, idClone); // 45� rotation
	tCamp1->setRotX(90, idClone); // 45� rotation
	tCamp1->setRotZ(-90, idClone); // 45� rotation
	tCamp1->setRotY(-90, idClone); // 45� rotation

	mAttr = tCamp1->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, tCamp1->AABBsize, main->cameraDetails);
	tCamp1->getModelAttributes()->push_back(mAttr);
	idClone = tCamp1->getModelAttributes()->size() - 1;

	translate = glm::vec3(65.0f, terreno->Superficie(10.0f, 10.0f), 65.0f);
	scale = glm::vec3(0.12f, 0.12f, 0.12f);
	tCamp1->setScale(&scale, idClone);
	tCamp1->setTranslate(&translate, idClone);
	tCamp1->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	tCamp1->setNextRotX(90, idClone); // 45� rotation
	tCamp1->setNextRotZ(-90, idClone); // 45� rotation
	tCamp1->setNextRotY(-90, idClone); // 45� rotation
	tCamp1->setRotX(90, idClone); // 45� rotation
	tCamp1->setRotZ(-90, idClone); // 45� rotation
	tCamp1->setRotY(-90, idClone); // 45� rotation

	//Modelos Oliver

	Model* gas_station;
	gas_station = new Model("models/Gas_Station/Gas_Station.fbx", main->cameraDetails);
	translate = glm::vec3(155.0f, terreno->Superficie(10.0f, 10.0f), 180.0f);
	scale = glm::vec3(0.5f, 0.5f, 0.5f);
	gas_station->setScale(&scale);
	gas_station->setTranslate(&translate);
	gas_station->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	gas_station->setNextRotX(-90); // 45� rotation
	gas_station->setNextRotZ(90); // 45� rotation
	ourModel.emplace_back(gas_station);

	//autos destruidos de antes del puente
	Model* autos_d;
	autos_d = new Model("models/Autos_Destruidos/autos_destruidos1.fbx", main->cameraDetails);
	translate = glm::vec3(-60.0f, terreno->Superficie(0.0f, 10.0f), 100.0f);
	scale = glm::vec3(2.5f, 2.5f, 2.5f);
	autos_d->setScale(&scale);
	autos_d->setTranslate(&translate);
	autos_d->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	autos_d->setNextRotX(-90); // 45� rotation
	autos_d->setNextRotZ(90); // 45� rotation
	autos_d->setNextRotY(-90); // 45� rotation
	ourModel.emplace_back(autos_d);

	Model* autos_d2;
	autos_d2 = new Model("models/Autos_Destruidos/autos_destruidos2.fbx", main->cameraDetails);
	translate = glm::vec3(-65.0f, terreno->Superficie(0.0f, 10.0f), 90.0f);
	scale = glm::vec3(2.5f, 2.5f, 2.5f);
	autos_d2->setScale(&scale);
	autos_d2->setTranslate(&translate);
	autos_d2->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	autos_d2->setNextRotX(-90); // 45� rotation
	autos_d2->setNextRotZ(90); // 45� rotation
	autos_d2->setNextRotY(-90); // 45� rotation
	ourModel.emplace_back(autos_d2);

	Model* autos_d3;
	autos_d3 = new Model("models/Autos_Destruidos/autos_destruidos3.fbx", main->cameraDetails);
	translate = glm::vec3(-70.0f, terreno->Superficie(0.0f, 10.0f), 80.0f);
	scale = glm::vec3(2.5f, 2.5f, 2.5f);
	autos_d3->setScale(&scale);
	autos_d3->setTranslate(&translate);
	autos_d3->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	autos_d3->setNextRotX(-90); // 45� rotation
	autos_d3->setNextRotZ(90); // 45� rotation
	autos_d3->setNextRotY(-90); // 45� rotation
	ourModel.emplace_back(autos_d3);

	//-------------------

	//autos destruidos despues del puente

	mAttr = autos_d->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, autos_d->AABBsize, main->cameraDetails);
	autos_d->getModelAttributes()->push_back(mAttr);
	idClone = autos_d->getModelAttributes()->size() - 1;

	translate = glm::vec3(50.0f, terreno->Superficie(0.0f, 10.0f), -180.0f);
	scale = glm::vec3(2.5f, 2.5f, 2.5f);
	autos_d->setScale(&scale, idClone);
	autos_d->setTranslate(&translate, idClone);
	autos_d->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	autos_d->setNextRotX(-90, idClone); // 45� rotation
	autos_d->setNextRotZ(90, idClone); // 45� rotation
	autos_d->setNextRotY(-90, idClone); // 45� rotation
	autos_d->setRotX(-90, idClone); // 45� rotation
	autos_d->setRotZ(90, idClone); // 45� rotation
	autos_d->setRotY(-90, idClone); // 45� rotation

	mAttr = autos_d2->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, autos_d2->AABBsize, main->cameraDetails);
	autos_d2->getModelAttributes()->push_back(mAttr);
	idClone = autos_d2->getModelAttributes()->size() - 1;

	translate = glm::vec3(65.0f, terreno->Superficie(0.0f, 10.0f), -186.0f);
	scale = glm::vec3(2.5f, 2.5f, 2.5f);
	autos_d2->setScale(&scale, idClone);
	autos_d2->setTranslate(&translate, idClone);
	autos_d2->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	autos_d2->setNextRotX(-90, idClone); // 45� rotation
	autos_d2->setNextRotZ(90, idClone); // 45� rotation
	autos_d2->setNextRotY(-90, idClone); // 45� rotation
	autos_d2->setRotX(-90, idClone); // 45� rotation
	autos_d2->setRotZ(90, idClone); // 45� rotation
	autos_d2->setRotY(-90, idClone); // 45� rotation

	mAttr = autos_d3->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, autos_d3->AABBsize, main->cameraDetails);
	autos_d3->getModelAttributes()->push_back(mAttr);
	idClone = autos_d3->getModelAttributes()->size() - 1;

	translate = glm::vec3(75.0f, terreno->Superficie(0.0f, 10.0f), -182.0f);
	scale = glm::vec3(2.5f, 2.5f, 2.5f);
	autos_d3->setScale(&scale, idClone);
	autos_d3->setTranslate(&translate, idClone);
	autos_d3->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	autos_d3->setNextRotX(-90, idClone); // 45� rotation
	autos_d3->setNextRotZ(90, idClone); // 45� rotation
	autos_d3->setNextRotY(-90, idClone); // 45� rotation
	autos_d3->setRotX(-90, idClone); // 45� rotation
	autos_d3->setRotZ(90, idClone); // 45� rotation
	autos_d3->setRotY(-90, idClone); // 45� rotation

	//_---------------------

	Model* wood_1;
	wood_1 = new Model("models/Estructuras_madera/estructura_madera1.fbx", main->cameraDetails);
	translate = glm::vec3(-155.0f, terreno->Superficie(10.0f, 10.0f), 10.0f);
	scale = glm::vec3(3.5f, 3.5f, 3.5f);
	wood_1->setScale(&scale);
	wood_1->setTranslate(&translate);
	wood_1->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	wood_1->setNextRotX(-90); // 45� rotation
	wood_1->setNextRotZ(90); // 45� rotation
	ourModel.emplace_back(wood_1);

	Model* wood_2;
	wood_2 = new Model("models/Estructuras_madera/estructura_madera2.fbx", main->cameraDetails);
	translate = glm::vec3(-155.0f, terreno->Superficie(10.0f, 10.0f), 30.0f);
	scale = glm::vec3(3.5f, 3.5f, 3.5f);
	wood_2->setScale(&scale);
	wood_2->setTranslate(&translate);
	wood_2->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	wood_2->setNextRotX(-90); // 45� rotation
	wood_2->setNextRotZ(90); // 45� rotation
	ourModel.emplace_back(wood_2);

	Model* granero_chico;
	granero_chico = new Model("models/Graneros/granero_chico.fbx", main->cameraDetails);
	translate = glm::vec3(-100.0f, terreno->Superficie(10.0f, 10.0f), -165.0f);
	scale = glm::vec3(0.14f, 0.14f, 0.14f);
	granero_chico->setScale(&scale);
	granero_chico->setTranslate(&translate);
	granero_chico->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	granero_chico->setNextRotX(-90); // 45� rotation
	granero_chico->setNextRotZ(0); // 45� rotation
	ourModel.emplace_back(granero_chico);

	Model* granero_chico1 = granero_chico;
	mAttr = granero_chico1->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, granero_chico1->AABBsize, main->cameraDetails);
	granero_chico1->getModelAttributes()->push_back(mAttr);
	idClone = granero_chico1->getModelAttributes()->size() - 1;

	translate = glm::vec3(-180.0f, terreno->Superficie(10.0f, 10.0f), -125.0f);
	scale = glm::vec3(0.14f, 0.14f, 0.14f);
	granero_chico1->setScale(&scale, idClone);
	granero_chico1->setTranslate(&translate, idClone);
	granero_chico1->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	granero_chico1->setNextRotX(-90, idClone); // 45� rotation
	granero_chico1->setNextRotZ(90, idClone); // 45� rotation
	granero_chico1->setRotX(-90, idClone); // 45� rotation
	granero_chico1->setRotZ(90, idClone); // 45� rotation

	mAttr = granero_chico1->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, granero_chico1->AABBsize, main->cameraDetails);
	granero_chico1->getModelAttributes()->push_back(mAttr);
	idClone = granero_chico1->getModelAttributes()->size() - 1;

	translate = glm::vec3(-100.0f, terreno->Superficie(10.0f, 10.0f), 182.0f);
	scale = glm::vec3(0.14f, 0.14f, 0.14f);
	granero_chico1->setScale(&scale, idClone);
	granero_chico1->setTranslate(&translate, idClone);
	granero_chico1->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	granero_chico1->setNextRotX(-90); // 45� rotation
	granero_chico1->setNextRotZ(180); // 45� rotation
	granero_chico1->setRotX(-90); // 45� rotation
	granero_chico1->setRotZ(180); // 45� rotation

	//este va a ser el modelo de estructura q se pueda entrar en el
	mAttr = granero_chico1->getModelAttributes()->at(0);
	granero_chico1->getModelAttributes()->push_back(mAttr);
	idClone = granero_chico1->getModelAttributes()->size() - 1;

	translate = glm::vec3(-55.0f, terreno->Superficie(10.0f, 10.0f), 182.0f);
	scale = glm::vec3(0.14f, 0.14f, 0.14f);
	granero_chico1->setScale(&scale, idClone);
	granero_chico1->setTranslate(&translate, idClone);
	granero_chico1->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	granero_chico1->setNextRotX(-90, idClone); // 45º rotation
	granero_chico1->setNextRotZ(180, idClone); // 45º rotation
	granero_chico1->setRotX(-90, idClone); // 45º rotation
	granero_chico1->setRotZ(180, idClone); // 45º rotation
	// Quitar hitbox: ponemos puntero a null y anulamos dimensiones AABB
	if (!granero_chico1->getModelAttributes()->empty()) {
		granero_chico1->getModelAttributes()->at(idClone).hitbox = nullptr;
	}

	//paredes laterales
	{
		glm::vec3 hbPos(-39.5f, 34.0f, 186.8f);
		float /*ancho en x*/halfW = 0.2f, /*ancho en y*/halfH = 10.0f, /*ancho en z*/halfD = 12.0f; // dimensiones pequeñas
		CollitionBox* hitboxVacia = new CollitionBox(hbPos.x, hbPos.y, hbPos.z, halfW, halfH, halfD, main->cameraDetails);
		hitboxVacia->name = "HitboxVacia"; // nombre identificable
		ourModel.emplace_back(hitboxVacia);
	}
	{
		glm::vec3 hbPos(-70.3f, 34.0f, 186.8f);
		float /*ancho en x*/halfW = 0.2f, /*ancho en y*/halfH = 10.0f, /*ancho en z*/halfD = 12.0f; // dimensiones pequeñas
		CollitionBox* hitboxVacia2 = new CollitionBox(hbPos.x, hbPos.y, hbPos.z, halfW, halfH, halfD, main->cameraDetails);
		hitboxVacia2->name = "HitboxVacia"; // nombre identificable
		ourModel.emplace_back(hitboxVacia2);
	}
	//pared trasera
	{
		glm::vec3 hbPos(-54.8f, 33.0f, 198.2f);
		float /*ancho en x*/halfW = 15.5f, /*ancho en y*/halfH = 10.0f, /*ancho en z*/halfD = 0.2f; // dimensiones pequeñas
		CollitionBox* hitboxVacia3 = new CollitionBox(hbPos.x, hbPos.y, hbPos.z, halfW, halfH, halfD, main->cameraDetails);
		hitboxVacia3->name = "HitboxVacia"; // nombre identificable
		ourModel.emplace_back(hitboxVacia3);
	}
	//paredes d la puerta
	{
		glm::vec3 hbPos(-62.5f, 33.0f, 174.6f);
		float /*ancho en x*/halfW = 7.7f, /*ancho en y*/halfH = 10.0f, /*ancho en z*/halfD = 0.2f; // dimensiones pequeñas
		CollitionBox* hitboxVacia4 = new CollitionBox(hbPos.x, hbPos.y, hbPos.z, halfW, halfH, halfD, main->cameraDetails);
		hitboxVacia4->name = "HitboxVacia"; // nombre identificable
		ourModel.emplace_back(hitboxVacia4);
	}
	{
		glm::vec3 hbPos(-43.3f, 33.0f, 174.6f);
		float /*ancho en x*/halfW = 3.8f, /*ancho en y*/halfH = 10.0f, /*ancho en z*/halfD = 0.2f; // dimensiones pequeñas
		CollitionBox* hitboxVacia5 = new CollitionBox(hbPos.x, hbPos.y, hbPos.z, halfW, halfH, halfD, main->cameraDetails);
		hitboxVacia5->name = "HitboxVacia"; // nombre identificable
		ourModel.emplace_back(hitboxVacia5);
	}
	{
		glm::vec3 hbPos(-51.0f, 36.0f, 174.6f);
		float /*ancho en x*/halfW = 3.2f, /*ancho en y*/halfH = 6.3f, /*ancho en z*/halfD = 0.2f; // dimensiones pequeñas
		CollitionBox* hitboxVacia5 = new CollitionBox(hbPos.x, hbPos.y, hbPos.z, halfW, halfH, halfD, main->cameraDetails);
		hitboxVacia5->name = "HitboxVacia"; // nombre identificable
		ourModel.emplace_back(hitboxVacia5);
	}

	Model* granero_2;
	granero_2 = new Model("models/Graneros/granero_2.fbx", main->cameraDetails);
	translate = glm::vec3(-163.0f, terreno->Superficie(10.0f, 10.0f), 154.0f);
	scale = glm::vec3(0.015f, 0.015f, 0.015f);
	granero_2->setScale(&scale);
	granero_2->setTranslate(&translate);
	granero_2->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	granero_2->setNextRotX(-90); // 45� rotation
	granero_2->setNextRotZ(90); // 45� rotation
	ourModel.emplace_back(granero_2);

	mAttr = granero_2->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, granero_2->AABBsize, main->cameraDetails);
	granero_2->getModelAttributes()->push_back(mAttr);
	idClone = granero_2->getModelAttributes()->size() - 1;

	translate = glm::vec3(170.0f, terreno->Superficie(10.0f, 10.0f), -178.0f);
	scale = glm::vec3(0.015f, 0.015f, 0.015f);
	granero_2->setScale(&scale, idClone);
	granero_2->setTranslate(&translate, idClone);
	granero_2->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	granero_2->setNextRotX(-90, idClone); // 45� rotation
	granero_2->setNextRotZ(90, idClone); // 45� rotation
	granero_2->setRotX(-90, idClone); // 45� rotation
	granero_2->setRotZ(90, idClone); // 45� rotation

	mAttr = granero_2->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, granero_2->AABBsize, main->cameraDetails);
	granero_2->getModelAttributes()->push_back(mAttr);
	idClone = granero_2->getModelAttributes()->size() - 1;

	translate = glm::vec3(-163.0f, terreno->Superficie(10.0f, 10.0f), 130.0f);
	scale = glm::vec3(0.015f, 0.015f, 0.015f);
	granero_2->setScale(&scale, idClone);
	granero_2->setTranslate(&translate, idClone);
	granero_2->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	granero_2->setNextRotX(-90, idClone); // 45� rotation
	granero_2->setNextRotZ(90, idClone); // 45� rotation
	granero_2->setRotX(-90, idClone); // 45� rotation
	granero_2->setRotZ(90, idClone); // 45� rotation

	mAttr = granero_2->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, granero_2->AABBsize, main->cameraDetails);
	granero_2->getModelAttributes()->push_back(mAttr);
	idClone = granero_2->getModelAttributes()->size() - 1;

	translate = glm::vec3(-163.0f, terreno->Superficie(10.0f, 10.0f), 110.0f);
	scale = glm::vec3(0.015f, 0.015f, 0.015f);
	granero_2->setScale(&scale, idClone);
	granero_2->setTranslate(&translate, idClone);
	granero_2->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	granero_2->setNextRotX(-90, idClone); // 45� rotation
	granero_2->setNextRotZ(90, idClone); // 45� rotation
	granero_2->setRotX(-90, idClone); // 45� rotation
	granero_2->setRotZ(90, idClone); // 45� rotation

	Model* pluma;
	pluma = new Model("models/PlumaStop/plumastop.fbx", main->cameraDetails);
	translate = glm::vec3(197.6f, terreno->Superficie(10.0f, 10.0f), -129.0f);
	scale = glm::vec3(3.0f, 3.0f, 3.0f);
	pluma->setScale(&scale);
	pluma->setTranslate(&translate);
	pluma->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	pluma->setNextRotX(-90); // 45� rotation
	pluma->setNextRotZ(90); // 45� rotation
	pluma->setNextRotY(0); // 45� rotation
	pluma->name = "StopFinal"; // <--- Nombre para detectar
	ourModel.emplace_back(pluma);

	Model* valla;
	valla = new Model("models/Estructuras_madera/valla.obj", main->cameraDetails);
	translate = glm::vec3(199.0f, terreno->Superficie(10.0f, 10.0f), -132.0f);
	scale = glm::vec3(3.0f, 3.0f, 3.0f);
	valla->setScale(&scale);
	valla->setTranslate(&translate);
	valla->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	//valla->setNextRotX(-90); // 45� rotation
	//valla->setNextRotZ(90); // 45� rotation
	//valla->setNextRotY(0); // 45� rotation
	ourModel.emplace_back(valla);

	//Model* valla2;
	//valla2 = new Model("models/Estructuras_madera/valla.obj", main->cameraDetails);
	//translate = glm::vec3(199.0f, terreno->Superficie(10.0f, 10.0f), -110.0f);
	//scale = glm::vec3(1.0f, 1.0f, 1.0f);
	//valla2->setScale(&scale);
	//valla2->setTranslate(&translate);
	//valla2->setNextTranslate(&translate);
	//rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	////valla2->setNextRotX(-90); // 45� rotation
	////valla2->setNextRotZ(90); // 45� rotation
	////valla2->setNextRotY(0); // 45� rotation
	//ourModel.emplace_back(valla2);

	Model* puente;
	puente = new Model("models/PuenteViejo/puente.fbx", main->cameraDetails);
	translate = glm::vec3(-120.0f, 13.3f, -57.5f);
	scale = glm::vec3(0.03f, 0.03f, 0.03f);
	puente->setScale(&scale);
	puente->setTranslate(&translate);
	puente->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	puente->setNextRotX(-90); // 45� rotation
	puente->setNextRotZ(0); // 45� rotation
	puente->setNextRotY(0); // 45� rotation
	ourModel.emplace_back(puente);

	mAttr = tCamp1->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, tCamp1->AABBsize, main->cameraDetails);
	tCamp1->getModelAttributes()->push_back(mAttr);
	idClone = tCamp1->getModelAttributes()->size() - 1;

	translate = glm::vec3(167.0f, terreno->Superficie(10.0f, 10.0f), -24.0f);
	scale = glm::vec3(0.12f, 0.12f, 0.12f);
	tCamp1->setScale(&scale, idClone);
	tCamp1->setTranslate(&translate, idClone);
	tCamp1->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	tCamp1->setNextRotX(-90, idClone); // 45� rotation
	tCamp1->setNextRotZ(-90, idClone); // 45� rotation
	tCamp1->setNextRotY(90, idClone); // 45� rotation
	tCamp1->setRotX(-90, idClone); // 45� rotation
	tCamp1->setRotZ(-90, idClone); // 45� rotation
	tCamp1->setRotY(90, idClone); // 45� rotation

	Model* auto_chido;
	auto_chido = new Model("models/Auto_chido/auto.obj", main->cameraDetails);
	translate = glm::vec3(90.0f, terreno->Superficie(10.0f, 10.0f), 170.0f);
	scale = glm::vec3(2.0f, 2.0f, 2.0f);
	auto_chido->setScale(&scale);
	auto_chido->setTranslate(&translate);
	auto_chido->setNextTranslate(&translate);
	//rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	//auto_chido->setNextRotX(-90); // 45� rotation
	//auto_chido->setNextRotZ(27); // 45� rotation
	auto_chido->name = "AutoChido"; // <--- Nombre para detectar
	ourModel.emplace_back(auto_chido);

	model = new Model("models/Viejito/viejillo.fbx", main->cameraDetails);
	translate = glm::vec3(134.0f, terreno->Superficie(0.0f, 60.0f), 164.0f);
	scale = glm::vec3(0.0002f, 0.0002f, 0.0002f); 	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotY(180);
	ourModel.emplace_back(model);
	try {
		std::vector<Animation> animations = Animation::loadAllAnimations("models/Viejito/viejillo.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::vector<Animation> animation = Animation::loadAllAnimations("models/Viejito/viejo_idle.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::move(animation.begin(), animation.end(), std::back_inserter(animations));
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(0);
	}
	catch (...) {
		ERRORL("Could not load animation!", "ANIMACION");
	}

	Camera* camera = Camera::getInstance();

	model = new Model("models/Zombies/ZombieE.fbx", main->cameraDetails);
	translate = glm::vec3(-134.0f, terreno->Superficie(0.0f, 10.0f), -151.0f);
	scale = glm::vec3(0.00075f, 0.00075f, 0.00075f); 	// it's a bit too big for our scene, so scale it down
	float Height = model->AABBsize.m_halfHeight;
	float Depth = model->AABBsize.m_halfDepth;  
	float width = model->AABBsize.m_halfWidth;
	delete model->getModelAttributes()->at(0).hitbox;
	Node n = model->AABBsize;

	n.m_center.y = 200;
	n.m_halfWidth = width * 14;
	n.m_halfHeight = Height * 325;
	n.m_halfDepth = Depth * 14;

	model->getModelAttributes()->at(0).hitbox = CollitionBox::GenerateAABB(translate, n, camera);

	ModelAttributes m;
	model = new Ammo("models/bullet/bullet_9_mm.glb", main->cameraDetails);
	translate = glm::vec3(0.0f, terreno->Superficie(0.0f, 70.0f)+5, 70.0f);
	scale = glm::vec3(0.10f, 0.10f, 0.10f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotY(-90);
	model->setNextRotY(-90);
	delete model->getModelAttributes()->at(0).hitbox;
	model->getModelAttributes()->at(0).hitbox = NULL;
	ourModel.emplace_back(model);

	model = new Model("models/Zombies/ZombieE.fbx", main->cameraDetails);
	translate = glm::vec3(-60.0f, terreno->Superficie(0.0f, 10.0f), -178.0f);
	scale = glm::vec3(0.00075f, 0.00075f, 0.00075f); 	// it's a bit too big for our scene, so scale it down
	delete model->getModelAttributes()->at(0).hitbox;

	n.m_center.y = 200;
	n.m_halfWidth = width * 14;
	n.m_halfHeight = Height * 325;
	n.m_halfDepth = Depth * 14;

	model->getModelAttributes()->at(0).hitbox = CollitionBox::GenerateAABB(translate, n, camera);
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotX(90);
	model->setNextRotY(-90);
	model->setNextRotZ(90);
	ourModel.emplace_back(model);
	try {
		std::vector<Animation> animations = Animation::loadAllAnimations("models/Zombies/ZombieE.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::vector<Animation> animation = Animation::loadAllAnimations("models/Zombies/ZombieWalk.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::move(animation.begin(), animation.end(), std::back_inserter(animations));
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(0);
	}
	catch (...) {
		ERRORL("Could not load animation!", "ANIMACION");
	}

	model = new Model("models/Zombies/ZombieN.fbx", main->cameraDetails);
	translate = glm::vec3(32.0f, terreno->Superficie(0.0f, 10.0f), -193.0f);
	scale = glm::vec3(0.0003f, 0.0003f, 0.0003f); 	// it's a bit too big for our scene, so scale it down
	delete model->getModelAttributes()->at(0).hitbox;

	n.m_center.y = 200;
	n.m_halfWidth = width * 35;
	n.m_halfHeight = Height * 650;
	n.m_halfDepth = Depth * 35;

	model->getModelAttributes()->at(0).hitbox = CollitionBox::GenerateAABB(translate, n, camera);
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotX(90);
	model->setNextRotY(-90);
	model->setNextRotZ(90);
	ourModel.emplace_back(model);
	try {
		std::vector<Animation> animations = Animation::loadAllAnimations("models/Zombies/ZombieN.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::vector<Animation> animation = Animation::loadAllAnimations("models/Zombies/ZombieCaminar.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::move(animation.begin(), animation.end(), std::back_inserter(animations));
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(0);
	}
	catch (...) {
		ERRORL("Could not load animation!", "ANIMACION");
	}

	model = new Model("models/Zombies/ZombieN.fbx", main->cameraDetails);
	translate = glm::vec3(-173.0f, terreno->Superficie(0.0f, 10.0f), -190.0f);
	scale = glm::vec3(0.0003f, 0.0003f, 0.0003f); 	// it's a bit too big for our scene, so scale it down
	delete model->getModelAttributes()->at(0).hitbox;

	n.m_center.y = 200;
	n.m_halfWidth = width * 35;
	n.m_halfHeight = Height * 650;
	n.m_halfDepth = Depth * 35;

	model->getModelAttributes()->at(0).hitbox = CollitionBox::GenerateAABB(translate, n, camera);
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotX(90);
	model->setNextRotY(-90);
	model->setNextRotZ(90);
	ourModel.emplace_back(model);
	try {
		std::vector<Animation> animations = Animation::loadAllAnimations("models/Zombies/ZombieN.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::vector<Animation> animation = Animation::loadAllAnimations("models/Zombies/ZombieCaminar.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::move(animation.begin(), animation.end(), std::back_inserter(animations));
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(0);
	}
	catch (...) {
		ERRORL("Could not load animation!", "ANIMACION");
	}

	//GASOLINAS
	Model* gas1;
	gas1 = new Model("models/Gasolina/gasolina.fbx", main->cameraDetails);
	translate = glm::vec3(-67.2f, terreno->Superficie(10.0f, 10.0f), 178.8f);
	scale = glm::vec3(0.25f, 0.25f, 0.25f);
	gas1->setScale(&scale);
	gas1->setTranslate(&translate);
	gas1->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	gas1->setNextRotX(-90); // 45� rotation
	gas1->setNextRotZ(-90); // 45� rotation
	gas1->name = "Gasolina";
	ourModel.emplace_back(gas1);

	Model* gas2;
	gas2 = new Model("models/Gasolina/gasolina.fbx", main->cameraDetails);
	translate = glm::vec3(-180.0f, terreno->Superficie(10.0f, 10.0f), 110.0f);
	scale = glm::vec3(0.25f, 0.25f, 0.25f);
	gas2->setScale(&scale);
	gas2->setTranslate(&translate);
	gas2->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	gas2->setNextRotX(-90); // 45� rotation
	gas2->setNextRotZ(-90); // 45� rotation
	gas2->name = "Gasolina";
	ourModel.emplace_back(gas2);

	Model* gas3;
	gas3 = new Model("models/Gasolina/gasolina.fbx", main->cameraDetails);
	translate = glm::vec3(-145.0f, terreno->Superficie(10.0f, 10.0f), 8.0f);
	scale = glm::vec3(0.25f, 0.25f, 0.25f);
	gas3->setScale(&scale);
	gas3->setTranslate(&translate);
	gas3->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	gas3->setNextRotX(-90); // 45� rotation
	gas3->setNextRotZ(-90); // 45� rotation
	gas3->name = "Gasolina";
	ourModel.emplace_back(gas3);

	Model* gas4;
	gas4 = new Model("models/Gasolina/gasolina.fbx", main->cameraDetails);
	translate = glm::vec3(-58.0f, terreno->Superficie(10.0f, 10.0f), 83.0f);
	scale = glm::vec3(0.25f, 0.25f, 0.25f);
	gas4->setScale(&scale);
	gas4->setTranslate(&translate);
	gas4->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	gas4->setNextRotX(-90); // 45� rotation
	gas4->setNextRotZ(-90); // 45� rotation
	gas4->name = "Gasolina";
	ourModel.emplace_back(gas4);

	// gasRandom2 (original gas5)
	Model* gas5;
	gas5 = new Model("models/Gasolina/gasolina.fbx", main->cameraDetails);
	translate = glm::vec3(64.0f, terreno->Superficie(10.0f, 10.0f), 55.0f); // default antes de random
	scale = glm::vec3(0.25f, 0.25f, 0.25f);
	gas5->setScale(&scale);
	gas5->setTranslate(&translate);
	gas5->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	gas5->setNextRotX(-90);
	gas5->setNextRotZ(-90);
	gas5->name = "gasRandom2";
	ourModel.emplace_back(gas5);

	Model* gas6;
	gas6 = new Model("models/Gasolina/gasolina.fbx", main->cameraDetails);
	translate = glm::vec3(175.0f, terreno->Superficie(10.0f, 10.0f), -30.0f);
	scale = glm::vec3(0.25f, 0.25f, 0.25f);
	gas6->setScale(&scale);
	gas6->setTranslate(&translate);
	gas6->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	gas6->setNextRotX(-90);
	gas6->setNextRotZ(-90);
	gas6->name = "Gasolina";
	ourModel.emplace_back(gas6);

	Model* gas7;
	gas7 = new Model("models/Gasolina/gasolina.fbx", main->cameraDetails);
	translate = glm::vec3(174.0f, terreno->Superficie(10.0f, 10.0f), -190.0f);
	scale = glm::vec3(0.25f, 0.25f, 0.25f);
	gas7->setScale(&scale);
	gas7->setTranslate(&translate);
	gas7->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	gas7->setNextRotX(-90);
	gas7->setNextRotZ(-90);
	gas7->name = "Gasolina";
	ourModel.emplace_back(gas7);

	Model* gas8;
	gas8 = new Model("models/Gasolina/gasolina.fbx", main->cameraDetails);
	translate = glm::vec3(50.0f, terreno->Superficie(10.0f, 10.0f), -190.0f);
	scale = glm::vec3(0.25f, 0.25f, 0.25f);
	gas8->setScale(&scale);
	gas8->setTranslate(&translate);
	gas8->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	gas8->setNextRotX(-90);
	gas8->setNextRotZ(-90);
	gas8->name = "Gasolina";
	ourModel.emplace_back(gas8);

	// gasRandom1 (original gas9)
	Model* gas9;
	gas9 = new Model("models/Gasolina/gasolina.fbx", main->cameraDetails);
	translate = glm::vec3(-86.0f, terreno->Superficie(10.0f, 10.0f), -153.0f); // default antes de random
	scale = glm::vec3(0.25f, 0.25f, 0.25f);
	gas9->setScale(&scale);
	gas9->setTranslate(&translate);
	gas9->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	gas9->setNextRotX(-90);
	gas9->setNextRotZ(-90);
	gas9->name = "gasRandom1";
	ourModel.emplace_back(gas9);

	Model* gas10;
	gas10 = new Model("models/Gasolina/gasolina.fbx", main->cameraDetails);
	translate = glm::vec3(-191.0f, terreno->Superficie(10.0f, 10.0f), -101.0f);
	scale = glm::vec3(0.25f, 0.25f, 0.25f);
	gas10->setScale(&scale);
	gas10->setTranslate(&translate);
	gas10->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	gas10->setNextRotX(-90);
	gas10->setNextRotZ(-90);
	gas10->name = "Gasolina";
	ourModel.emplace_back(gas10);

	//2 modelos aleatorios: gasRandom1 y gasRandom2
	{
		static bool semillaInicializada = false;
		if (!semillaInicializada) { std::srand((unsigned)time(nullptr)); semillaInicializada = true; }
		int numRandom = 1 + (std::rand() % 4); // 1..4
		glm::vec3 posRandom1; glm::vec3 posRandom2;
		switch (numRandom) {
			case 1: posRandom1 = glm::vec3(-86.0f, terreno->Superficie(10.0f, 10.0f), -153.0f); posRandom2 = glm::vec3(64.0f, terreno->Superficie(10.0f, 10.0f), 55.0f); break;
			case 2: posRandom1 = glm::vec3(-110.0f, terreno->Superficie(10.0f, 10.0f), -191.0f); posRandom2 = glm::vec3(194.0f, terreno->Superficie(10.0f, 10.0f), 190.0f); break;
			case 3: posRandom1 = glm::vec3(-191.0f, terreno->Superficie(10.0f, 10.0f), -160.0f); posRandom2 = glm::vec3(124.0f, terreno->Superficie(10.0f, 10.0f), 193.0f); break;
			case 4: posRandom1 = glm::vec3(-68.0f, terreno->Superficie(10.0f, 10.0f), -172.0f); posRandom2 = glm::vec3(-140.0f, terreno->Superficie(10.0f, 10.0f), 131.0f); break;
		}
		// Buscar punteros a modelos gasRandom1 y gasRandom2 ya cargados
		Model* gr1 = nullptr; Model* gr2 = nullptr;
		for (Model* m : ourModel) {
			if (m->name == "gasRandom1") gr1 = m;
			else if (m->name == "gasRandom2") gr2 = m;
			if (gr1 && gr2) break;
		}
		if (gr1) { gr1->setTranslate(&posRandom1); gr1->setNextTranslate(&posRandom1); }
		if (gr2) { gr2->setTranslate(&posRandom2); gr2->setNextTranslate(&posRandom2); }
	}
	//FIN RANDOM

	
	//SPAM DE ARBOLES
	Model* pino;
	pino = new Model("models/Pino/pino.fbx", main->cameraDetails);
	translate = glm::vec3(30.0f, terreno->Superficie(10.0f, 10.0f), 10.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale);
	pino->setTranslate(&translate);
	pino->setNextTranslate(&translate);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	pino->setNextRotX(90); // 45� rotation
	pino->setNextRotZ(-90); // 45� rotation
	pino->setNextRotY(-90); // 45� rotation
	ourModel.emplace_back(pino);

	// REPLICAS PINO 1..15
	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(103.0f, terreno->Superficie(10.0f, 10.0f), 191.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(81.0f, terreno->Superficie(10.0f, 10.0f), 187.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(53.0f, terreno->Superficie(10.0f, 10.0f), 170.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(28.0f, terreno->Superficie(10.0f, 10.0f), 181.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(5.0f, terreno->Superficie(10.0f, 10.0f), 156.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(23.0f, terreno->Superficie(10.0f, 10.0f), 123.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(74.0f, terreno->Superficie(10.0f, 10.0f), 109.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(123.0f, terreno->Superficie(10.0f, 10.0f), 95.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(153.0f, terreno->Superficie(10.0f, 10.0f), 58.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(116.0f, terreno->Superficie(10.0f, 10.0f), 45.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(77.0f, terreno->Superficie(10.0f, 10.0f), 20.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(20.0f, terreno->Superficie(10.0f, 10.0f), 43.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(44.0f, terreno->Superficie(10.0f, 10.0f), 83.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(195.0f, terreno->Superficie(10.0f, 10.0f), 183.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(194.0f, terreno->Superficie(10.0f, 10.0f), 154.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	// REPLICAS PINO 16..26
	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-26.0f, terreno->Superficie(10.0f, 10.0f), 187.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-14.0f, terreno->Superficie(10.0f, 10.0f), 122.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-19.0f, terreno->Superficie(10.0f, 10.0f), 78.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-30.0f, terreno->Superficie(10.0f, 10.0f), 36.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-57.0f, terreno->Superficie(10.0f, 10.0f), 45.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-103.0f, terreno->Superficie(10.0f, 10.0f), 43.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-138.0f, terreno->Superficie(10.0f, 10.0f), 67.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-174.0f, terreno->Superficie(10.0f, 10.0f), 91.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-187.0f, terreno->Superficie(10.0f, 10.0f), 36.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-77.0f, terreno->Superficie(10.0f, 10.0f), 125.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-100.0f, terreno->Superficie(10.0f, 10.0f), 72.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation
	//27...38
	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(190.0f, terreno->Superficie(10.0f, 10.0f), -43.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(112.0f, terreno->Superficie(10.0f, 10.0f), -36.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(74.0f, terreno->Superficie(10.0f, 10.0f), -56.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(56.0f, terreno->Superficie(10.0f, 10.0f), -84.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(71.0f, terreno->Superficie(10.0f, 10.0f), -102.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(125.0f, terreno->Superficie(10.0f, 10.0f), -105.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(159.0f, terreno->Superficie(10.0f, 10.0f), -103.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(166.0f, terreno->Superficie(10.0f, 10.0f), -159.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(72.0f, terreno->Superficie(10.0f, 10.0f), -148.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(91.0f, terreno->Superficie(10.0f, 10.0f), -185.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(144.0f, terreno->Superficie(10.0f, 10.0f), -191.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(7.0f, terreno->Superficie(10.0f, 10.0f), -173.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation
	//39 ...51
	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-164.0f, terreno->Superficie(10.0f, 10.0f), -13.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-78.0f, terreno->Superficie(10.0f, 10.0f), -24.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-20.0f, terreno->Superficie(10.0f, 10.0f), -36.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-4.0f, terreno->Superficie(10.0f, 10.0f), -64.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-11.0f, terreno->Superficie(10.0f, 10.0f), -98.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-43.0f, terreno->Superficie(10.0f, 10.0f), -129.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-69.0f, terreno->Superficie(10.0f, 10.0f), -167.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-101.0f, terreno->Superficie(10.0f, 10.0f), -94.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-125.0f, terreno->Superficie(10.0f, 10.0f), -162.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-155.0f, terreno->Superficie(10.0f, 10.0f), -188.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-188.0f, terreno->Superficie(10.0f, 10.0f), -160.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-168.0f, terreno->Superficie(10.0f, 10.0f), -96.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation

	mAttr = pino->getModelAttributes()->at(0);
	mAttr.hitbox = CollitionBox::GenerateAABB(translate, pino->AABBsize, main->cameraDetails);
	pino->getModelAttributes()->push_back(mAttr);
	idClone = pino->getModelAttributes()->size() - 1;
	translate = glm::vec3(-145.0f, terreno->Superficie(10.0f, 10.0f), -87.0f);
	scale = glm::vec3(0.1f, 0.2f, 0.1f);
	pino->setScale(&scale, idClone);
	pino->setTranslate(&translate, idClone);
	pino->setNextTranslate(&translate, idClone);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	pino->setNextRotX(90, idClone);
	pino->setNextRotZ(-90, idClone);
	pino->setNextRotY(-90, idClone); // 45� rotation
	pino->setRotX(90, idClone);
	pino->setRotZ(-90, idClone);
	pino->setRotY(-90, idClone); // 45� rotation




	//Lol

	//Model *pez = new Model("models/pez/pez.obj", main->cameraDetails);
	//translate = glm::vec3(0.0f, terreno->Superficie(0.0f, 50.0f), 50.0f);
	//pez->setNextTranslate(&translate);
	//pez->setTranslate(&translate);
	//ourModel.emplace_back(pez);
	//ModelAttributes m;
	//m.setTranslate(&translate);
	//m.setNextTranslate(&translate);
	//m.translate.x = 5;
	//model = CollitionBox::GenerateAABB(m.translate, pez->AABBsize, main->cameraDetails);
	//model->setTranslate(&m.translate);
	//model->setNextTranslate(&m.translate);
	//m.hitbox = model;
	//pez->getModelAttributes()->push_back(m);
	//m.setTranslate(&translate);
	//m.setNextTranslate(&translate);
	//m.translate.x = 10;
	//model = CollitionBox::GenerateAABB(m.translate, pez->AABBsize, main->cameraDetails);
	//model->setTranslate(&m.translate);
	//model->setNextTranslate(&m.translate);
	//m.hitbox = model; // Le decimos al ultimo ModelAttribute que tiene un hitbox asignado
	//pez->getModelAttributes()->push_back(m);

	//model = new Model("models/dancing_vampire/dancing_vampire.dae", main->cameraDetails);
	//translate = glm::vec3(0.0f, terreno->Superficie(0.0f, 60.0f), 60.0f);
	//scale = glm::vec3(0.02f, 0.02f, 0.02f); 	// it's a bit too big for our scene, so scale it down
	//model->setTranslate(&translate);
	//model->setNextTranslate(&translate);
	//model->setScale(&scale);
	//model->setNextRotY(90);
	//ourModel.emplace_back(model);
	//try{
	//	std::vector<Animation> animations = Animation::loadAllAnimations("models/dancing_vampire/dancing_vampire.dae", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
	//	std::vector<Animation> animation = Animation::loadAllAnimations("models/dancing_vampire/dancing_vampire.dae", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
	//	std::move(animation.begin(), animation.end(), std::back_inserter(animations));
	//	for (Animation animation : animations)
	//		model->setAnimator(Animator(animation));
	//	model->setAnimation(1);
	//}catch(...){
	//	ERRORL("Could not load animation!", "ANIMACION");
	//}

	//Model* silly = new Model("models/Silly_Dancing/Silly_Dancing.fbx", main->cameraDetails);
	//translate = glm::vec3(10.0f, terreno->Superficie(10.0f, 60.0f) , 60.0f);
	//scale = glm::vec3(0.02f, 0.02f, 0.02f); 	// it's a bit too big for our scene, so scale it down
	//silly->setTranslate(&translate);
	//silly->setNextTranslate(&translate);
	//silly->setScale(&scale);
	//silly->setNextRotY(180);
	//ourModel.emplace_back(silly);
	//try{
	//	std::vector<Animation> animations = Animation::loadAllAnimations("models/Silly_Dancing/Silly_Dancing.fbx", silly->GetBoneInfoMap(), silly->getBonesInfo(), silly->GetBoneCount());
	//	for (Animation animation : animations)
	//		silly->setAnimator(Animator(animation));
	//	silly->setAnimation(0);
	//}catch(...){
	//	ERRORL("Could not load animation!", "ANIMACION");
	//}
	//m.setTranslate(&translate);
	//m.setNextTranslate(&translate);
	//m.translate.x += 10;
	//m.setScale(&scale);
	//m.setNextRotY(180);
	//m.setRotY(180);
	//model = CollitionBox::GenerateAABB(m.translate, silly->AABBsize, main->cameraDetails);
	//model->setTranslate(&m.translate);
	//model->setNextTranslate(&m.translate);
	//model->setScale(&scale);
	//model->setNextRotY(180);
	//model->setRotY(180);
	//m.hitbox = model; // Le decimos al ultimo ModelAttribute que tiene un hitbox asignado
	//silly->getModelAttributes()->push_back(m);
	//// Import model and clone with bones and animations
	//model = new Model("models/Silly_Dancing/Silly_Dancing.fbx", main->cameraDetails);
	//translate = glm::vec3(30.0f, terreno->Superficie(30.0f, 60.0f) , 60.0f);
	//scale = glm::vec3(0.02f, 0.02f, 0.02f); 	// it's a bit too big for our scene, so scale it down
	//model->name = "Silly_Dancing1";
	//model->setTranslate(&translate);
	//model->setNextTranslate(&translate);
	//model->setScale(&scale);
	//model->setNextRotY(180);
	//ourModel.emplace_back(model);
	//// Para clonar la animacion se eliminan los huesos del modelo actual y se copian los modelos y animators
	//model->GetBoneInfoMap()->clear();
	//model->getBonesInfo()->clear();
	//*model->GetBoneInfoMap() = *silly->GetBoneInfoMap();
	//*model->getBonesInfo() = *silly->getBonesInfo();
	//model->setAnimator(silly->getAnimator());

	//	model = new Model("models/IronMan.obj", main->cameraDetails);
//	translate = glm::vec3(0.0f, 20.0f, 30.0f);
//	scale = glm::vec3(0.025f, 0.025f, 0.025f); 	// it's a bit too big for our scene, so scale it down
//	model->setScale(&scale);
//	model->setTranslate(&translate);
//	ourModel.emplace_back(model);
	//model = new Model("models/backpack/backpack.obj", main->cameraDetails, false, false);
	//translate = glm::vec3(20.0f, terreno->Superficie(20.0f, 0.0f) + 2, 0.0f);
	//scale = glm::vec3(1.0f, 1.0f, 1.0f); 	// it's a bit too big for our scene, so scale it down
	//model->setTranslate(&translate);
	//model->setNextTranslate(&translate);
	//model->setScale(&scale);
	//ourModel.emplace_back(model);
	//model->lightColor = glm::vec3(10,0,0);
	//model = new CollitionBox(60.0f, 15.0f, 10.0f, 10, 10, 10, main->cameraDetails);
	//scale = glm::vec3(1.0f, 1.0f, 1.0f); 	// it's a bit too big for our scene, so scale it down
	//model->setNextTranslate(model->getTranslate());
	//model->setScale(&scale);
	//ourModel.emplace_back(model);
	
	// Agregar hitbox vacía en (116, 23.5, 137)
	//{
	//	glm::vec3 hbPos(116.0f, 23.5f, 137.0f);
	//	float halfW = 1.0f, halfH = 1.0f, halfD = 1.0f; // dimensiones pequeñas
	//	CollitionBox* hitboxVacia = new CollitionBox(hbPos.x, hbPos.y, hbPos.z, halfW, halfH, halfD, main->cameraDetails);
	//	hitboxVacia->name = "HitboxVacia"; // nombre identificable
	//	ourModel.emplace_back(hitboxVacia);
	//}


	inicializaBillboards();
	std::wstring prueba(L"Esta es una prueba");
	ourText.emplace_back(new Texto(prueba, 20, 0, 0, SCR_HEIGHT, 0, camara));
	billBoard2D.emplace_back(new Billboard2D((WCHAR*)L"billboards/vida6.png", 6, 6, 50, 50, 0, camara->cameraDetails));
	scale = glm::vec3(500.0f, 45.0f, 100.0f); 	// it's a bit too big for our scene, so scale it down
	billBoard2D.back()->setScale(&scale);

	billBoard2D.emplace_back(new Billboard2D((WCHAR*)L"billboards/gas.png", 6, 6, 50, 100, 0, camara->cameraDetails));
	scale = glm::vec3(100.0f, 100.0f, 100.0f); 
	billBoard2D.back()->setScale(&scale);

	// Contador de gas (texto inicial "0/10")
	// Ajusta xCounter si quieres más separación respecto al icono.
	float xCounter = 50.0f + 130.0f; // 50 icono + desplazamiento a la derecha
	float yCounter = 130.0f;         // misma altura
	Texto* gasCounter = new Texto((WCHAR*)L"0/10", 50.0f, 0.0f, xCounter, yCounter, 0.0f, camara);
	gasCounter->name = "GasCounter";
	ourText.emplace_back(gasCounter);

	}


void Scenario::inicializaBillboards() {
    float ye = terreno->Superficie(0, 0);
    /*billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol.png", 6, 6, 0, ye - 1, 0, camara->cameraDetails));*/

	ye = terreno->Superficie(-9, -15);
	//billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol3.png", 8, 8, -9, ye - 1, -15, camara->cameraDetails));

	BillboardAnimation *billBoardAnimated = new BillboardAnimation();
	ye = terreno->Superficie(5, -5);
	/*for (int frameArbol = 1; frameArbol < 4; frameArbol++){
		wchar_t textura[50] = {L"billboards/Arbol"};
		if (frameArbol != 1){
			wchar_t convert[25];
			swprintf(convert, 25, L"%d", frameArbol);
			wcscat_s(textura, 50, convert);
		}
		wcscat_s(textura, 50, L".png");
		billBoardAnimated->pushFrame(new Billboard((WCHAR*)textura, 6, 6, 5, ye - 1, -5, camara->cameraDetails));		
	}*/
	billBoardAnim.emplace_back(billBoardAnimated);

    // PNG en mundo: "textoviejito.png" en (131.0, 23.5, 163.0)
    billBoard.emplace_back(new Billboard(
        (WCHAR*)L"billboards/textoviejito.png",
        6, 6,                   // ancho, alto del quad (ajústalos a gusto)
        131.0f, 23.5f, 163.0f,  // x, y, z
        camara->cameraDetails));

    // Opcional: si quieres ajustar por escala en vez de ancho/alto
    // glm::vec3 s = glm::vec3(1.0f, 1.0f, 1.0f);
    // billBoard.back()->setScale(&s);
}

	//el metodo render toma el dispositivo sobre el cual va a dibujar
	//y hace su tarea ya conocida
Scene* Scenario::Render() {
	//borramos el biffer de color y el z para el control de profundidad a la 
	//hora del render a nivel pixel.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//OLIVER CAMBIOS--------------

	// Day-Night cycle background color using angulo
	float radiansA = glm::radians(this->angulo);
	float k = (cosf(radiansA) + 1.0f) * 0.5f; // 0..1
	glm::vec3 dayColor(0.53f, 0.81f, 0.92f);   // light sky blue
	glm::vec3 nightColor(0.01f, 0.01f, 0.03f); // dark blue
	glm::vec3 skyColor = nightColor + k * (dayColor - nightColor);
	glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);

	//OLIVER CAMBIOS--------------

	if (this->animacion > 10) { // Timer se ejecuta cada 1000/30 = 33.333 ms
		for (BillboardAnimation *b : billBoardAnim){
			b->nextAnimation();
		}
		this->animacion = 0;
	} else {
		animacion = animacion + (1 * gameTime.deltaTime/100);
	}

	//OLIVER CAMBIOS--------------

	// Decimos que dibuje la media esfera (solo si existe)
	if (sky != NULL) {
		sky->Draw();
	}

	//OLIVER CAMBIOS--------------


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
