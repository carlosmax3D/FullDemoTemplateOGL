#include "Scenario.h"
#include "Ammo.h"
#include "SpriteSec.h"
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
    glm::vec3 translate;
    glm::vec3 translate_2;
    glm::vec3 scale;
    glm::vec3 rotation;
    translate = glm::vec3(140.0f, 3.0f, -82.0f);
    float matAmbient[] = { 1,1,1,1 };
	float matDiff[] = { 1,1,1,1 };
	angulo = 0;
	camara = main;
	//creamos el objeto skydome
	sky = new SkyDome(32, 32, 20, (WCHAR*)L"skydome/nigth.jpg", main->cameraDetails);

    //sky_2 = new SkyDome(32, 32, 20, (WCHAR*)L"skydome/earth.jpg", main->cameraDetails);
    

    //translate = glm::vec3(40.0f, 11.0f, 20.0f);
    //translate_2 = glm::vec3(173.0f, 12.0f, 68.0f);

    //scale = glm::vec3(1.5f, 1.5f, 1.5f);


    //sky->setTranslate(&translate);


 
    //sky_2->setScale(&scale);
    sky->poner_bandera(true);


	//creamos el terreno
	terreno = new Terreno((WCHAR*)L"skydome/terreno.jpg", (WCHAR*)L"skydome/tex_terr_2.jpg", 400, 400, main->cameraDetails);
	water = new Water((WCHAR*)L"textures/terreno.bmp", (WCHAR*)L"textures/water.bmp", 150, 150, camara->cameraDetails);

    

    glm::vec3 translate_water = glm::vec3(133.0f, 7.0f, -70.0f);


	water->setTranslate(&translate_water);

    water->setRotX(180);
    water->setNextRotX(180);
    
	// load models
	// -----------
	ourModel.emplace_back(main);


	/*Model* model;*/
	//model = new Model("models/fogata/fogata.obj", main->cameraDetails);
	//translate = glm::vec3(0.0f, 10.0f, 25.0f);
	//model->setTranslate(&translate);
	//model->setNextTranslate(&translate);
	//rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	//model->setNextRotX(45); // 45� rotation
	//ourModel.emplace_back(model);


    Model* bala = new Ammo("models/bullet/bullet_9_mm.glb", main->cameraDetails);
    translate = glm::vec3(0.0f, terreno->Superficie(0.0f, 70.0f) + 5, 70.0f);
    scale = glm::vec3(0.10f, 0.10f, 0.10f);	// it's a bit too big for our scene, so scale it down
    bala->setTranslate(&translate);
    bala->setNextTranslate(&translate);
    bala->setScale(&scale);
    bala->setRotY(-90);
    bala->setNextRotY(-90);
    delete bala->getModelAttributes()->at(0).hitbox;
    bala->getModelAttributes()->at(0).hitbox = NULL;
    ourModel.emplace_back(bala);


    Model* model;
    model = new Model("models/chevy_pop/Chevy_pop.fbx", main->cameraDetails);
    translate = glm::vec3(70.0f, 9.0f , 60.0f);
    //translate = glm::vec3(0.0f, 10.0f, 25.0f);
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    scale = glm::vec3(.25f, .25f, .25f);
    model->setScale(&scale);
    model->setNextRotX(0); // 45� rotation
    ourModel.emplace_back(model);


    Model* barril;
    barril = new Model("models/barriles/barril.fbx", main->cameraDetails);
    translate = glm::vec3(-130.0f, 25.5f, -100.0f);
    //translate = glm::vec3(40.0f, 50.0f, 10.0f);
    barril->setTranslate(&translate);
    barril->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    barril->setScale(&scale);
    barril->setNextRotX(-90); // 45� rotation
    ourModel.emplace_back(barril);



    Model* almacen_1;
    almacen_1 = new Model("models/almacen_1/ALMACEN_1.fbx", main->cameraDetails);
    translate = glm::vec3(-145.0f,25.5f, -115.0f);
    //translate = glm::vec3(-80.0f, 3.0f, 60.0f);
    almacen_1->setTranslate(&translate);
    almacen_1->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    almacen_1->setNextRotY(85); // 45� rotation
    ourModel.emplace_back(almacen_1);

    delete almacen_1->getModelAttributes()->at(0).hitbox;
    almacen_1->getModelAttributes()->at(0).hitbox = NULL;



    Model* house;
    house = new Model("models/house/fbxHouse.fbx", main->cameraDetails);
    translate = glm::vec3(33.0f, 10.45f, 90.0f);
    //translate = glm::vec3(-80.0f, 3.0f, 60.0f);
    house->setTranslate(&translate);
    house->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    house->setNextRotY(45); // 45� rotation
    scale = glm::vec3(45.0f, 45.0f, 45.0f);	// it's a bit too big for our scene, so scale it down
    house->setScale(&scale);
    house->setTranslate(&translate);
    ourModel.emplace_back(house);

    delete house->getModelAttributes()->at(0).hitbox;
    house->getModelAttributes()->at(0).hitbox = NULL;



    Model* zombie_1;
    zombie_1 = new Model("models/zombie_1/zombie.fbx", main->cameraDetails);
    translate = glm::vec3(-30.0f, terreno->Superficie(10.0f, 60.0f), 90.0f);
    //translate = glm::vec3(-80.0f, 3.0f, 60.0f);
    zombie_1->setTranslate(&translate);
    zombie_1->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    zombie_1->setNextRotX(0); // 45� rotation
    scale = glm::vec3(.0045f, .0045f, .0045f);	// it's a bit too big for our scene, so scale it down
    zombie_1->setScale(&scale);
    zombie_1->setTranslate(&translate);
    ourModel.emplace_back(zombie_1);




    Model* zombie_2;
    zombie_2 = new Model("models/zombie_2/Zombie_2.fbx", main->cameraDetails);
    translate = glm::vec3(-30.0f, terreno->Superficie(10.0f, 60.0f), 100.0f);
    //translate = glm::vec3(20.0f, 100.0f, 100.0f);
    zombie_2->setTranslate(&translate);
    zombie_2->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    zombie_2->setNextRotX(0); // 45� rotation
    scale = glm::vec3(.02f, .02f, .02f);	// it's a bit too big for our scene, so scale it down
    //scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
    zombie_2->setScale(&scale);
    zombie_2->setTranslate(&translate);
    ourModel.emplace_back(zombie_2);


    Model* pistola;
    pistola = new Model("models/pistola/Pistola.fbx", main->cameraDetails);
    translate = glm::vec3(-20.0f, terreno->Superficie(10.0f, 60.0f), 100.0f);
    //translate = glm::vec3(-80.0f, 3.0f, 60.0f);
    pistola->setTranslate(&translate);
    pistola->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    pistola->setNextRotX(0); // 45� rotation
    scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
    pistola->setScale(&scale);
    pistola->setTranslate(&translate);
    ourModel.emplace_back(pistola);




    Model* escopeta;
    escopeta = new Model("models/Escopeta/M4.fbx", main->cameraDetails);
    translate = glm::vec3(-20.0f, terreno->Superficie(10.0f, 60.0f), 100.0f);
    //translate = glm::vec3(-80.0f, 3.0f, 60.0f);
    escopeta->setTranslate(&translate);
    escopeta->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    escopeta->setNextRotX(-90); // 45� rotation
    scale = glm::vec3(.02f, .02f, .02f);	// it's a bit too big for our scene, so scale it down
    escopeta->setScale(&scale);
    escopeta->setTranslate(&translate);
    ourModel.emplace_back(escopeta);


    Model* ak;
    ak = new Model("models/ak47/rifle.fbx", main->cameraDetails);
    translate = glm::vec3(-20.0f, terreno->Superficie(10.0f, 60.0f), 100.0f);
    //translate = glm::vec3(-80.0f, 3.0f, 60.0f);
    ak->setTranslate(&translate);
    ak->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    ak->setNextRotZ(90); // 45� rotation
    scale = glm::vec3(.05f, .05f, .05f);	// it's a bit too big for our scene, so scale it down
    ak->setScale(&scale);
    ak->setTranslate(&translate);
    ourModel.emplace_back(ak);


    Model* arbol;
    arbol = new Model("models/Arbol/ARBOL.fbx", main->cameraDetails);
    translate = glm::vec3(-40.0f, terreno->Superficie(10.0f, 60.0f), 100.0f);
    //translate = glm::vec3(-80.0f, 3.0f, 60.0f);
    arbol->setTranslate(&translate);
    arbol->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    //arbol->setNextRotY(0); // 45� rotation
    //arbol->setNextRotZ(0); // 45� rotation
    arbol->setNextRotX(75); // 45� rotation
    scale = glm::vec3(.005f, .005f, .005f);	// it's a bit too big for our scene, so scale it down
    arbol->setScale(&scale);
    arbol->setTranslate(&translate);
    ourModel.emplace_back(arbol);



    Model* caja_madera;
    caja_madera = new Model("models/cajas_madera/cajas_1.fbx", main->cameraDetails);
    translate = glm::vec3(-121.0f, 25.5f, -113.0f);
    //translate = glm::vec3(-80.0f, 3.0f, 60.0f);
    caja_madera->setTranslate(&translate);
    caja_madera->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    //arbol->setNextRotY(0); // 45� rotation
    //arbol->setNextRotZ(0); // 45� rotation
    caja_madera->setNextRotX(-90); // 45� rotation
    scale = glm::vec3(.05f, .05f, .05f);	// it's a bit too big for our scene, so scale it down
    caja_madera->setScale(&scale);
    caja_madera->setTranslate(&translate);
    ourModel.emplace_back(caja_madera);




    Model* mesa;
    mesa = new Model("models/mesa/mesa.fbx", main->cameraDetails);
    translate = glm::vec3(136.0f, 11.5f, 145.0f);
    //translate = glm::vec3(-80.0f, 3.0f, 60.0f);
    mesa->setTranslate(&translate);
    mesa->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    //arbol->setNextRotY(0); // 45� rotation
    //arbol->setNextRotZ(0); // 45� rotation
    mesa->setNextRotX(-90); // 45� rotation
    scale = glm::vec3(3.0f, 3.0f, 3.0f);	// it's a bit too big for our scene, so scale it down
    mesa->setScale(&scale);
    mesa->setTranslate(&translate);
    ourModel.emplace_back(mesa);



    Model* camion;
    camion = new Model("models/camion/camion.fbx", main->cameraDetails);
    translate = glm::vec3(-153.0f,24.5f, -72.0f);
    //translate = glm::vec3(-20.0f, 0.0f, 170.0f);
    camion->setTranslate(&translate);
    camion->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    camion->setNextRotY(90); // 45� rotation
    scale = glm::vec3(2.0f, 2.0f, 2.0f);	// it's a bit too big for our scene, so scale it down
    camion->setScale(&scale);

    ourModel.emplace_back(camion);




    Model* fuente;
    fuente = new Model("models/fuente/fbxFountain.fbx", main->cameraDetails);
    translate = glm::vec3(124.0f,11.30f , 159.0f);
    //translate = glm::vec3(-20.0f, 0.0f, 170.0f);
    fuente->setTranslate(&translate);
    fuente->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    fuente->setNextRotX(0); // 45� rotation
    scale = glm::vec3(3.10f, 3.10f, 3.10f);	// it's a bit too big for our scene, so scale it down
    fuente->setScale(&scale);
    fuente->setTranslate(&translate);
    ourModel.emplace_back(fuente);




    Model* barril_madera;
    barril_madera = new Model("models/barril/barril_madera.fbx", main->cameraDetails);
    translate = glm::vec3(-122.0f, 25.5f, -126.0f);
    //translate = glm::vec3(-20.0f, 0.0f, 170.0f);
    barril_madera->setTranslate(&translate);
    barril_madera->setNextTranslate(&translate);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    /*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    almacen_1->setScale(&scale);*/
    barril_madera->setNextRotX(-90); // 45� rotation
    scale = glm::vec3(2.10f, 2.10f, 2.10f);	// it's a bit too big for our scene, so scale it down
    barril_madera->setScale(&scale);
    barril_madera->setTranslate(&translate);
    ourModel.emplace_back(barril_madera);


    Model* MURO_FINAL;
    MURO_FINAL = new Model("models/Puerta_muro/muro.fbx", main->cameraDetails);
    translate = glm::vec3(-171.35f, 9.0f, 172.5f);
    //translate = glm::vec3(-20.0f, 0.0f, 170.0f);
    MURO_FINAL->setTranslate(&translate);
    MURO_FINAL->setNextTranslate(&translate);
    //rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    scale = glm::vec3(0.015f, 0.018f, 0.018f);
    MURO_FINAL->setScale(&scale);
    MURO_FINAL->setNextRotY(-35); // 45� rotation
    //scale = glm::vec3(2.10f, 2.10f, 2.10f);	// it's a bit too big for our scene, so scale it down
    //barril_madera->setScale(&scale);
    //barril_madera->setTranslate(&translate);
    ourModel.emplace_back(MURO_FINAL);


    Model* pared_muro;
    pared_muro = new Model("models/pared_muro/pared_new.fbx", main->cameraDetails);
    translate = glm::vec3(-138.0f, 9.0f, 196.0f);
    //translate = glm::vec3(-20.0f, 0.0f, 170.0f);
    pared_muro->setTranslate(&translate);
    pared_muro->setNextTranslate(&translate);

    scale = glm::vec3(.150f,.150f, .150f);
    pared_muro->setScale(&scale);
    pared_muro->setNextRotX(-90); 
    pared_muro->setNextRotZ(-35); 

    ourModel.emplace_back(pared_muro);
    ModelAttributes m;
    ModelAttributes clon;
    for (int i = 0; i < 8;i++) {
        if (i == 4) {
            
            translate.x -= 7.5;
            translate.z -= 5.9;
            continue;
        }

        translate.x -= 6.6;
        translate.z -= 4.6;

        clon.setTranslate(&translate);
        clon.setNextTranslate(&translate);
        clon.setScale(&scale);

        clon.setNextRotX(-90);
        clon.setRotX(-90);

        clon.setNextRotZ(-35);
        clon.setRotZ(-35);

        Model *hitbox = CollitionBox::GenerateAABB(clon.translate, pared_muro->AABBsize, main->cameraDetails);
        hitbox->setTranslate(&clon.translate);
        hitbox->setNextTranslate(&clon.translate);
        hitbox->setScale(&scale);


        hitbox->setNextRotX(-90);
        hitbox->setRotX(-90);

        hitbox->setNextRotZ(-35);
        hitbox->setRotZ(-35);

        clon.hitbox = hitbox;
        pared_muro->getModelAttributes()->push_back(clon);


    }
    




    //Model* cerca;
    //cerca = new Model("models/Cerca/muro.obj", main->cameraDetails);
    //translate = glm::vec3(-71.0f, 9.3f, 197.0f);
    ////translate = glm::vec3(-20.0f, 0.0f, 170.0f);
    //cerca->setTranslate(&translate);
    //cerca->setNextTranslate(&translate);
    //rotation = glm::vec3(0.0f, 0.0f, 0.0f); //rotation X
    ///*scale = glm::vec3(1.0f, 1.0f, 1.0f);
    //almacen_1->setScale(&scale);*/
    //cerca->setNextRotY(90); // 45� rotation
    //scale = glm::vec3(1.2f, 1.2f, 1.2f);	// it's a bit too big for our scene, so scale it down
    //cerca->setScale(&scale);
    //cerca->setTranslate(&translate);
    //ourModel.emplace_back(cerca);







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
	//scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
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
	//scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
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
	//scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
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
//	scale = glm::vec3(0.025f, 0.025f, 0.025f);	// it's a bit too big for our scene, so scale it down
//	model->setScale(&scale);
//	model->setTranslate(&translate);
//	ourModel.emplace_back(model);




	//model = new Model("models/backpack/backpack.obj", main->cameraDetails, false, false);
	//translate = glm::vec3(20.0f, terreno->Superficie(20.0f, 0.0f) + 2, 0.0f);
	//scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
	//model->setTranslate(&translate);
	//model->setNextTranslate(&translate);
	//model->setScale(&scale);
	//ourModel.emplace_back(model);
	//model->lightColor = glm::vec3(10,0,0);
	//model = new CollitionBox(60.0f, 15.0f, 10.0f, 10, 10, 10, main->cameraDetails);
	//scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
	//model->setNextTranslate(model->getTranslate());
	//model->setScale(&scale);
	//ourModel.emplace_back(model);
	

	inicializaBillboards();
	std::wstring prueba(L"Esta es una prueba");
	ourText.emplace_back(new Texto(prueba, 20, 0, 0, SCR_HEIGHT, 0, camara));
	billBoard2D.emplace_back(new Billboard2D((WCHAR*)L"billboards/awesomeface.png", 6, 6, 100, 200, 0, camara->cameraDetails));
	scale = glm::vec3(100.0f, 100.0f, 0.0f);	// it's a bit too big for our scene, so scale it down
	billBoard2D.back()->setScale(&scale);
}

//arboles


void Scenario::inicializaBillboards() {
	float ye = terreno->Superficie(0, 0);
	//billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol.png", 6, 6, 0, ye - 1, 0, camara->cameraDetails));

    //cerca


    // Carga la textura una sola vez
    WCHAR* arbolTex = (WCHAR*)L"billboards/Arbol.png";

    // Estructura para almacenar los datos de cada billboard
    struct BillboardData {
        float x, y, z;
        float width, height;
    };

    // Lista completa de billboards
    BillboardData arboles[] = {
        // Sección 1
        {120, 8, 30, 6, 6},{125, 8, 32, 6, 6},{135, 8, 25, 6, 6},{120, 8, 37, 6, 6},
        {192, 8, 18, 6, 6},{195, 8, 22, 6, 6},{194, 8, 35, 6, 6},{192, 8, 40, 6, 6},
        {180, 8, 35, 6, 6},{185, 8, 35, 6, 6},{183, 8, 31, 6, 6},{188, 8, 26, 6, 6},
        {190, 8, 31, 6, 6},{171, 8, 31, 6, 6},{171, 8, 25, 6, 6},{179, 8, 23, 6, 6},
        {182, 8, 30, 6, 6},{199, 8, 15, 6, 6},{188, 8, 47, 6, 6},{178, 8, 38, 6, 6},
        {168, 8, 36, 6, 6},{159, 8, 29, 6, 6},{165, 8, 21, 6, 6},{196, 8, 49, 6, 6},
        {185, 8, 41, 6, 6},{124, 8, 23, 6, 6},{110, 8, 21, 6, 6},{111, 8, 34, 6, 6},
        {102, 8, 29, 6, 6},{95, 8, 13, 6, 6},{120, 8, 15, 6, 6},{108, 8, 2, 6, 6},
        {196, 8, 61, 6, 6},{195, 8, 68, 6, 6},{189, 8, 77, 6, 6},{193, 8, 86, 6, 6},
        {185, 8, 105, 6, 6},{177, 9, 108, 6, 6},{178, 8, 121, 6, 6},{189, 8, 126, 6, 6},
        {152, 10, 109, 6, 6},{191, 8, 57, 6, 6},{189, 8, 66, 6, 6},{194, 8, 96, 6, 6},
        {194, 8, 112, 6, 6},{177, 9, 113, 6, 6},{183, 8, 92, 6, 6},{196, 8, 74, 6, 6},
        {183, 8, 82, 6, 6},{187, 8, 71, 6, 6},{197, 8, 91, 6, 6},{193, 8, 102, 6, 6},
        {174, 9, 98, 6, 6},{129, 8, 24, 6, 6},{117, 8, 21, 6, 6},{112, 8, 27, 6, 6},
        {112, 8, 11, 6, 6},{105, 8, 13, 6, 6},{98, 8, 20, 6, 6},{99, 8, 2, 6, 6},
        {91, 8, 1, 6, 6},{86, 8, -11, 6, 6},{81, 8, -28, 6, 6},

        // Sección 2
        {145, 10, 117, 6, 6},{167, 9, 112, 6, 6},{105, 11, 118, 6, 6},{112, 10, 101, 6, 6},
        {131, 10, 106, 6, 6},{120, 10, 107, 6, 6},{115, 10, 112, 6, 6},{140, 10, 108, 6, 6},
        {155, 10, 118, 6, 6},{102, 10, 105, 6, 6},{108, 10, 93, 6, 6},{125, 10, 97, 6, 6},
        {133, 10, 110, 6, 6},{159, 10, 118, 6, 6},{145, 10, 111, 6, 6},{118, 10, 101, 6, 6},
        {123, 10, 117, 6, 6},{158, 10, 104, 6, 6},{95, 10, 118, 6, 6},{119, 10, 118, 6, 6},
        {135, 10, 119, 6, 6},{118, 10, 94, 6, 6},{93, 10, 98, 6, 6},{90, 10, 107, 6, 6},
        {86, 10, 115, 6, 6},{86, 10, 127, 6, 6},{93, 10, 123, 6, 6},{97, 10, 89, 6, 6},
        {98, 10, 149, 6, 6},{102, 10, 143, 6, 6},{104, 10, 134, 6, 6},{92, 10, 139, 6, 6},
        {95, 10, 130, 6, 6},{99, 10, 97, 6, 6},{79, 10, 123, 6, 6},{88, 10, 139, 6, 6},
        {107, 11, 106, 6, 6},{101, 11, 124, 6, 6},{128, 11, 118, 6, 6},{107, 11, 127, 6, 6},

        // Sección 
        {57, 8, 198, 6, 6},{54, 8, 190, 6, 6},{48, 8, 179, 6, 6},{38, 8, 168, 6, 6},
        {24, 8, 161, 6, 6},{6, 8, 160, 6, 6},{-18, 8, 169, 6, 6},{-7, 8, 158, 6, 6},
        {59, 8, 194, 6, 6},{-53, 8, 181, 6, 6},{-41, 8, 169, 6, 6},{-27, 8, 158, 6, 6},
        {-17, 8, 165, 6, 6},{-6, 8, 171, 6, 6},{5, 8, 179, 6, 6},{23, 8, 184, 6, 6},
        {42, 8, 185, 6, 6},{-54, 8, 196, 6, 6},{-52, 8, 190, 6, 6},{-42, 8, 193, 6, 6},
        {-39, 8, 180, 6, 6},{-30, 8, 172, 6, 6},{-26, 8, 183, 6, 6},{-28, 8, 192, 6, 6},
        {-20, 8, 186, 6, 6},{-14, 8, 181, 6, 6},{-8, 8, 190, 6, 6},{-1, 8, 196, 6, 6},
        {10, 8, 193, 6, 6},{12, 8, 180, 6, 6},{5, 8, 169, 6, 6},{-6, 8, 164, 6, 6},
        {-28, 8, 165, 6, 6},{-12, 8, 181, 6, 6},{49, 8, 194, 6, 6},{39, 8, 193, 6, 6},
        {34, 8, 186, 6, 6},{38, 8, 179, 6, 6},{30, 8, 171, 6, 6},{22, 8, 170, 6, 6},
        {14, 8, 166, 6, 6},{29, 8, 184, 6, 6},{21, 8, 192, 6, 6},{14, 8, 186, 6, 6},
        {21, 8, 172, 6, 6},{15, 8, 164, 6, 6},{-3, 8, 177, 6, 6},{-17, 8, 193, 6, 6},
        {-22, 8, 175, 6, 6},{-46, 8, 179, 6, 6},{63, 8, 196, 6, 6},{29, 8, 193, 6, 6},
        {13, 8, 174, 6, 6},{7, 8, 186, 6, 6},{-3, 8, 185, 6, 6},{-14, 8, 173, 6, 6},
        {-32, 8, 179, 6, 6},{-36, 8, 189, 6, 6},{-45, 8, 185, 6, 6},{-36, 8, 196, 6, 6},
        {-36, 8, 159, 6, 6},{-14, 8, 154, 6, 6},{47, 8, 187, 6, 6},{26, 8, 177, 6, 6},
        {18, 8, 178, 6, 6},{0, 8, 163, 6, 6},{30, 8, 161, 6, 6},{42, 8, 197, 6, 6},
        {33, 8, 197, 6, 6},{24, 8, 197, 6, 6},{16, 8, 197, 6, 6},{6, 8, 197, 6, 6},
        {2, 8, 191, 6, 6}/*,{-20, 8, 197, 6, 6},{11, 8, 158, 6, 6},{45, 8, 172, 6, 6},
        {54, 8, 182, 6, 6},{-20, 8, 160, 6, 6},{-35, 8, 167, 6, 6}*/
    
    };

    //Crear los billboards de forma iterativa
    for (auto& b : arboles) {
        billBoard.emplace_back(new Billboard(arbolTex, b.width, b.height, b.x, b.y, b.z, camara->cameraDetails));
    }
    //billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/cerca_n.png", 6, 6, -101, 9, 185, camara->cameraDetails));
    //agregar variable para que no use lo de girar
    
    //billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/cerca.png", 6, 6, -101, 9, 195, camara->cameraDetails));


    //billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol.png", 6, 6, 3, ye - 1, 0, camara->cameraDetails));


	ye = terreno->Superficie(-9, -15);
	//billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/CAMINAR/caminar.png", 8, 8, -9, ye - 1, -15, camara->cameraDetails));
	//billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol3.png", 8, 8, -9, ye - 1, -15, camara->cameraDetails));

	BillboardAnimation *billBoardAnimated = new BillboardAnimation();
	ye = terreno->Superficie(5, -5);
	/*for (int frameArbol = 1; frameArbol < 5; frameArbol++){
		wchar_t textura[50] = {L"billboards/CAMINAR/caminar"};
		if (frameArbol != 1){
			wchar_t convert[25];
			swprintf(convert, 25, L"%d", frameArbol);
			wcscat_s(textura, 50, convert);
		}
		wcscat_s(textura, 50, L".png");
		billBoardAnimated->pushFrame(new Billboard((WCHAR*)textura, 6, 6, 5, ye - 1, -5, camara->cameraDetails));		
	}*/
	billBoardAnim.emplace_back(billBoardAnimated);




    //animacionsp *anim = new animacionsp();
    //ye = terreno->Superficie(5, -5);
    //for (int frameArbol = 1; frameArbol < 5; frameArbol++) {
    //    wchar_t textura[50] = { L"billboards/CAMINAR/caminar" };
    //    if (frameArbol != 1) {
    //        wchar_t convert[25];
    //        swprintf(convert, 25, L"%d", frameArbol);
    //        wcscat_s(textura, 50, convert);
    //    }
    //    wcscat_s(textura, 50, L".png");
    //    anim->pushFrame(new Billboard((WCHAR*)textura, 6, 6, 5, ye - 1, -5, camara->cameraDetails));
    //}
    //billBoardAnim.emplace_back(anim);



    //ZITOS PRACTICA
    // Crear animación del árbol
    // Crear animación del árbol


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
    //sky_2->Draw();
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
