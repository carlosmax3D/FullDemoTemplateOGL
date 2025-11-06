#include "Scenario.h"
#ifdef __linux__ 
#define ZeroMemory(x,y) memset(x,0,y)
#define wcscpy_s(x,y,z) wcscpy(x,z)
#define wcscat_s(x,y,z) wcscat(x,z)
#endif

Scenario::Scenario(Camera* cam) {
    glm::vec3 translate;
    glm::vec3 scale;
    Model* model = new Model("models/Cube/Cube.obj", cam);
    translate = glm::vec3(0.0f, 0.0f, 3.0f);
    scale = glm::vec3(0.25f, 0.25f, 0.25f);
    model->setScale(&scale);
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    InitGraph(model);

    // Inicializar variables del drone
    isDroneActive = false;
    previousMainModel = nullptr;

}

Scenario::Scenario(Model* camIni) {
    InitGraph(camIni);
}

void Scenario::enterDrone(Model* player) {
    if (isDroneActive || Drone == nullptr) return;

    // Guardar posición EXACTA del jugador
    previousMainModel = player;
    glm::vec3 savedPlayerPos = *player->getTranslate();

    // CONGELAR al jugador en su posición actual
    player->setActive(false);
    player->setVisible(false);

    // IMPORTANTE: Forzar que el jugador NO se mueva
    player->setNextTranslate(&savedPlayerPos);

    // El drone se queda donde está (NO lo movemos)
    // Solo activamos el drone
    Drone->setActive(true);
    Drone->setVisible(true);

    // FORZAR cámara en primera persona para el drone
    Drone->cameraDetails->setFirstPerson(true);

    // Cambiar modelo principal al drone
    setMainModel(Drone);

    INFO("Entrando al Drone.", "DRONE");
    isDroneActive = true;
}

void Scenario::exitDrone() {
    if (!isDroneActive || previousMainModel == nullptr) return;

    // Verificar altura del drone
    glm::vec3 dronePos = *Drone->getTranslate();
    float terrainHeight = getTerreno()->Superficie(dronePos.x, dronePos.z);

    // NO PERMITIR salir si está muy alto (más de 2 unidades sobre el terreno)
    if (dronePos.y > terrainHeight + 2.0f) {
        INFO("No puedes salir del drone mientras está en el aire. Desciende primero (Tecla F).", "DRONE");
        return;
    }

    // RECUPERAR la posición EXACTA donde el jugador entró al drone
    // El jugador NO se movió, sigue en la misma posición
    glm::vec3 playerOriginalPos = *previousMainModel->getTranslate();

    // Reactivar al jugador EN SU POSICIÓN ORIGINAL
    previousMainModel->setTranslate(&playerOriginalPos);
    previousMainModel->setNextTranslate(&playerOriginalPos);
    previousMainModel->setActive(true);
    previousMainModel->setVisible(true);

    // Restaurar cámara al jugador (en tercera persona)
    setMainModel(previousMainModel);
    previousMainModel->cameraDetails->setFirstPerson(false);

    // El drone se queda donde lo dejaste
    Drone->setActive(true);
    Drone->setVisible(true);

    INFO("Saliendo del Drone. El jugador regresa a su posición original.", "DRONE");
    isDroneActive = false;
    previousMainModel = nullptr;
}

void Scenario::InitGraph(Model* main) {
    float matAmbient[] = { 1,1,1,1 };
    float matDiff[] = { 1,1,1,1 };
    angulo = 0;
    camara = main;

    // CREAR SKYDOME CON CICLO DÍA/NOCHE COMPLETO
    sky = new SkyDome(32, 32, 20,
        (WCHAR*)L"skydome/day_sky.jpg",
        (WCHAR*)L"skydome/sunset_sky.jpg",
        (WCHAR*)L"skydome/night_sky.jpg",
        main->cameraDetails,
        0.0f);

    std::cout << "\n=== SISTEMA DIA/NOCHE INICIADO ===" << std::endl;
    std::cout << "Ciclo completo: 6 minutos (360 segundos)" << std::endl;
    std::cout << "Cada periodo: 2 minutos (120 segundos)" << std::endl;
    std::cout << "NOCHE: 0:00-8:00 (0-120s)" << std::endl;
    std::cout << "DIA: 8:00-16:00 (120-240s)" << std::endl;
    std::cout << "TARDE: 16:00-24:00 (240-360s)" << std::endl;
    std::cout << "===================================\n" << std::endl;

    // Creamos el terreno
    terreno = new Terreno(
        (WCHAR*)L"skydome/terreno.jpg",      // Mapa de alturas
        (WCHAR*)L"skydome/texterr2.jpg",     // Textura 1 (pasto)
        (WCHAR*)L"skydome/texterr.jpg",       // Textura 2 (tierra)
		(WCHAR*)L"skydome/multitextura.jpg", // Splatmap escala de grises
        400, 400,
        main->cameraDetails
    );

    // Agua
    water = new Water((WCHAR*)L"textures/terreno.bmp", (WCHAR*)L"textures/water.bmp", 20, 20, camara->cameraDetails);
    glm::vec3 translate, scale, rotation;
    translate = glm::vec3(0.0f, -3000.0f, 30.0f);
    water->setTranslate(&translate);

    // El drone se crea en startGameEngine()

    // Load models
    ourModel.emplace_back(main);

    //CARRITO DE COMIDA
    Model* model;
    model = new Model("models/Food cart/FoodCart.obj", main->cameraDetails);
    translate = glm::vec3(-6.0f, 4.7f, 19.0f);
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setNextRotY(45);
    // Colisión inline - sin declarar variables separadas
    model->buildCollider(
		0.1f, 1.0f, -1.6f, //posición relativa al modelo
		1.0f, 4.0f, 2.3f); // dimensiones de la caja
    ourModel.emplace_back(model);

    //BARANDALES
 
	//barandal 1
/*    model = new Model("models/Barandal/Barandal.obj", main->cameraDetails);
    translate = glm::vec3(4.7f, 4.7f, 9.0f);
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setNextRotY(180);
    // Colisión diferente inline
    model->buildCollider(
        0.0f, 2.0f, 0.0f,
        10.0f, 2.0f, 0.5f);  // Barandal: más ancho y plano
    ourModel.emplace_back(model);

	//barandal 2
    model = new Model("models/Barandal/Barandal.obj", main->cameraDetails);
    translate = glm::vec3(4.7f, 4.7f, -9.0f);
    model->setTranslate(&translate);
    model->setNextTranslate(&translate);
    model->setNextRotY(180);
    // Colisión diferente inline
    model->buildCollider(
        0.0f, 2.0f, 0.0f,
        10.0f, 2.0f, 0.5f);  // Barandal: más ancho y plano
    ourModel.emplace_back(model);*/

    
    //ESTADIO SIN COLISIONES 

    // ============================================================
  // CREAR PAREDES INVISIBLES 
  // ============================================================
    std::vector<CollitionBox*> invisibleWalls;

    // Pared NORTE
    CollitionBox* northWall = new CollitionBox(
        0.0f, 5.0f, -36.0f,      // Posición (x, y, z) -36 para igualar con sur
        25.0f, 5.0f, 0.5f,       // halfWidth, halfHeight, halfDepth
        main->cameraDetails
    );
    northWall->setVisible(false);
    invisibleWalls.push_back(northWall);

    // Pared SUR
    CollitionBox* southWall = new CollitionBox(
        0.0f, 5.0f, 36.0f,
        25.0f, 5.0f, 0.5f, 
        main->cameraDetails);
    southWall->setVisible(false);
    invisibleWalls.push_back(southWall);

    // Pared ESTE
    CollitionBox* eastWall = new CollitionBox(
        23.0f, 5.0f, 0.0f,
        12.5f, 5.0f, 50.0f, 
        main->cameraDetails);
    eastWall->setVisible(false);
    invisibleWalls.push_back(eastWall);

    //// Pared OESTE
    //CollitionBox* westWall = new CollitionBox(
    //    -50.0f, 5.0f, 0.0f,
    //    0.5f, 5.0f, 50.0f,
    //    main->cameraDetails);
    //westWall->setVisible(false);
    //invisibleWalls.push_back(westWall);

    // Agregarlas al vector de modelos para que se detecten colisiones
    for (auto wall : invisibleWalls) {
        ourModel.emplace_back(wall);
    }

    std::cout << "Paredes invisibles del mapa creadas: " << invisibleWalls.size() << std::endl;
    // ============================================================

    inicializaBillboards();

    // Texto
    std::wstring prueba(L"Esta es una prueba");
    ourText.emplace_back(new Texto(prueba, 20, 0, 0, SCR_HEIGHT, 0, camara));

    // Billboard 2D
    billBoard2D.emplace_back(new Billboard2D((WCHAR*)L"billboards/awesomeface.png", 6, 6, 100, 200, 0, camara->cameraDetails));
    scale = glm::vec3(100.0f, 100.0f, 0.0f);
    billBoard2D.back()->setScale(&scale);
}

void Scenario::inicializaBillboards() {
  /*  float ye = terreno->Superficie(0, 0);
    billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol.png", 6, 6, 0, ye - 1, 0, camara->cameraDetails));*/

    float ye = terreno->Superficie(2, -12);
    billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol3.png", 8, 8, -9, ye - 1, -15, camara->cameraDetails));

    //BillboardAnimation* billBoardAnimated = new BillboardAnimation();
    //ye = terreno->Superficie(5, -5);
    //for (int frameArbol = 1; frameArbol < 4; frameArbol++) {
    //    wchar_t textura[50] = { L"billboards/Arbol" };
    //    if (frameArbol != 1) {
    //        wchar_t convert[25];
    //        swprintf(convert, 25, L"%d", frameArbol);
    //        wcscat_s(textura, 50, convert);
    //    }
    //    wcscat_s(textura, 50, L".png");
    //    billBoardAnimated->pushFrame(new Billboard((WCHAR*)textura, 6, 6, 5, ye - 1, -5, camara->cameraDetails));
    //}
    //billBoardAnim.emplace_back(billBoardAnimated);
}

Scene* Scenario::Render() {
    // ============================================================
    // ACTUALIZAR CICLO DÍA/NOCHE
    // ============================================================
    sky->update(gameTime.deltaTime / 1000.0f);

    // ============================================================
    // OBTENER INFORMACIÓN DE ILUMINACIÓN DEL SKYDOME
    // ============================================================
    glm::vec3 ambient = sky->getAmbientLight();
    glm::vec3 diffuse = sky->getDiffuseLight();
    glm::vec3 sunDir = sky->getSunDirection();
    float currentTime = sky->getTimeOfDay();

    // ============================================================
    // COLOR DE FONDO DINÁMICO
    // ============================================================
    glm::vec3 clearColor = ambient * 1.2f; //contraste
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);

    // ============================================================
    // LIMPIAR BUFFERS
    // ============================================================
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ============================================================
    // ANIMACIÓN DE BILLBOARDS
    // ============================================================
    if (this->animacion > 10) {
        for (BillboardAnimation* b : billBoardAnim) {
            b->nextAnimation();
        }
        this->animacion = 0;
    }
    else {
        animacion = animacion + (1 * gameTime.deltaTime / 100);
    }

    // ============================================================
    // DIBUJAR SKYDOME (FONDO)
    // ============================================================
    sky->Draw();

    // ============================================================
    // DIBUJAR TERRENO CON ILUMINACIÓN DINÁMICA
    // ============================================================
    if (terreno != NULL) {
        // Si el terreno usa shader personalizado
        if (terreno->gpuDemo != NULL) {
            terreno->gpuDemo->use();

            // Pasar iluminación dinámica
            terreno->gpuDemo->setVec3("dirLightAmbient", ambient);
            terreno->gpuDemo->setVec3("dirLightDiffuse", diffuse);
            terreno->gpuDemo->setVec3("dirLightSpecular", glm::vec3(1.0f, 1.0f, 1.0f));
            terreno->gpuDemo->setVec3("dirLightDirection", -sunDir);
            terreno->gpuDemo->setFloat("timeOfDay", currentTime);
            terreno->gpuDemo->setVec3("viewPos", terreno->cameraDetails->getPosition());

            terreno->gpuDemo->desuse();
        }
        terreno->Draw();
    }

    // ============================================================
    // DIBUJAR AGUA (SI EXISTE)
    // ============================================================
    if (water != NULL) {
        water->Draw();
    }

    // ============================================================
    // DIBUJAR BILLBOARDS
    // ============================================================
    for (int i = 0; i < billBoard.size(); i++)
        billBoard[i]->Draw();

    for (int i = 0; i < billBoardAnim.size(); i++)
        billBoardAnim[i]->Draw();

    for (int i = 0; i < billBoard2D.size(); i++)
        billBoard2D[i]->Draw();

    // ============================================================
    // DIBUJAR MODELOS CON ILUMINACIÓN DINÁMICA
    // ============================================================
    for (int i = 0; i < ourModel.size(); i++) {
        if (ourModel[i] != NULL) {
            // Si el modelo tiene shader personalizado
            if (ourModel[i]->gpuDemo != NULL) {
                ourModel[i]->gpuDemo->use();

                // PASAR ILUMINACIÓN DINÁMICA - MISMOS VALORES QUE EL TERRENO
                ourModel[i]->gpuDemo->setVec3("dirLight.ambient", ambient);
                ourModel[i]->gpuDemo->setVec3("dirLight.diffuse", diffuse);
                ourModel[i]->gpuDemo->setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
                ourModel[i]->gpuDemo->setVec3("dirLight.direction", -sunDir);
                ourModel[i]->gpuDemo->setFloat("timeOfDay", currentTime);
                ourModel[i]->gpuDemo->setVec3("viewPos", ourModel[i]->cameraDetails->getPosition());

                // ELIMINAR: ourModel[i]->gpuDemo->setBool("useDynamicLighting", true);

                ourModel[i]->gpuDemo->desuse();
            }

            // Dibujar el modelo
            ourModel[i]->Draw();
        }
    }

    // ============================================================
    // DIBUJAR TEXTO
    // ============================================================
    for (int i = 0; i < ourText.size(); i++) {
        ourText[i]->Draw();
    }

    return this;
}

std::vector<Model*>* Scenario::getLoadedModels() {
    return &ourModel;
}

std::vector<Billboard*>* Scenario::getLoadedBillboards() {
    return &billBoard;
}

std::vector<Billboard2D*>* Scenario::getLoadedBillboards2D() {
    return &billBoard2D;
}

std::vector<Texto*>* Scenario::getLoadedText() {
    return &ourText;
}

std::vector<BillboardAnimation*>* Scenario::getLoadedBillboardsAnimation() {
    return &billBoardAnim;
}

Model* Scenario::getMainModel() {
    return this->camara;
}

void Scenario::setMainModel(Model* mainModel) {
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
    // Limpiar sky
    if (this->sky != NULL) {
        delete this->sky;
        this->sky = NULL;
    }

    // Limpiar terreno
    if (this->terreno != NULL) {
        delete this->terreno;
        this->terreno = NULL;
    }

    // Limpiar water si existe
    if (this->water != NULL) {
        delete this->water;
        this->water = NULL;
    }

    // Limpiar billboards
    if (billBoard.size() > 0) {
        for (int i = 0; i < billBoard.size(); i++)
            delete billBoard[i];
        billBoard.clear();
    }

    if (billBoardAnim.size() > 0) {
        for (int i = 0; i < billBoardAnim.size(); i++)
            delete billBoardAnim[i];
        billBoardAnim.clear();
    }

    if (billBoard2D.size() > 0) {
        for (int i = 0; i < billBoard2D.size(); i++)
            delete billBoard2D[i];
        billBoard2D.clear();
    }

    // Limpiar texto (excepto FPS y coordenadas que se limpian en otro lugar)
    if (ourText.size() > 0) {
        for (int i = 0; i < ourText.size(); i++) {
            if (ourText[i]->name != "FPSCounter" && ourText[i]->name != "Coordenadas") {
                delete ourText[i];
            }
        }
        ourText.clear();
    }

    // Limpiar modelos (excepto la cámara principal)
    if (ourModel.size() > 0) {
        for (int i = 0; i < ourModel.size(); i++) {
            if (ourModel[i] != camara) {
                delete ourModel[i];
            }
        }
        ourModel.clear();
    }
}
// M�todos para veh�culo
Model* Scenario::getDrone() { return Drone; }
void Scenario::setDrone(Model* drone) { Drone = drone; } 
bool Scenario::getIsDroneActive() { return isDroneActive; }
float Scenario::getDroneSpeedMultiplier() { return DroneSpeedMultiplier; }
Model* Scenario::getActiveModel() { return isDroneActive ? Drone : getMainModel(); }
