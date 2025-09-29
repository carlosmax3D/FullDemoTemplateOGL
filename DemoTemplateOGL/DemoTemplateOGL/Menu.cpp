#include "Menu.h"

Menu::Menu(Camera *cam){
	InitGraph(cam);
}

Menu::Menu(Model *camIni){
    InitGraph(camIni->cameraDetails);
}

void Menu::InitGraph(Camera *camara){
    glm::vec3 translate;
	glm::vec3 scale;
    glm::vec3 rotation;
    Billboard2D* main = new Billboard2D((WCHAR*)L"textures/arrow.png", 6, 6, 620, 145, 0, camara);
	scale = glm::vec3(100.0f, 100.0f, 0.0f);	// it's a bit too big for our scene, so scale it down
	main->setScale(&scale);
    main->setRotZ(90);
	this->camara = main;
    billBoard2D.push_back(main);
    billBoard2D.push_back(new Billboard2D((WCHAR*)L"textures/Menu.png", 6, 6, 380, 300, 0, camara));
	scale = glm::vec3(600.0f, 600.0f, 0.0f);	// it's a bit too big for our scene, so scale it down
    billBoard2D.back()->setScale(&scale);
}

int Menu::update(int &menuOption){
    glm::vec3 translate;
    switch(menuOption){
        case 1: translate = glm::vec3(620,145,0);
                break;
        case 2: translate = glm::vec3(620,250,0);
                break;
        case 3: translate = glm::vec3(620,355,0);
                break;
        case 4: translate = glm::vec3(620,460,0);
                break;
        default: translate = glm::vec3(620,menuOption > 4 ? 460 : 145,0);
                 menuOption = menuOption > 4 ? 4 : 1;
                break;
    }
    camara->setTranslate(&translate);
    camara->setNextTranslate(&translate);
    return 0;
}

Menu* Menu::Render(){
    for(Billboard2D *b : billBoard2D)
        b->Draw();
    return this;
}

std::vector<Billboard2D*> *Menu::getLoadedBillboards2D(){
    return &billBoard2D;
}

Model* Menu::getMainModel(){
    return camara;
}

void Menu::setMainModel(Billboard2D* mainModel){
    this->camara = mainModel;
}

//    void update() override;
Menu::~Menu(){
    for(Billboard2D *b : billBoard2D)
        delete b;
}
