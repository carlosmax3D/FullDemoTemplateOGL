#ifndef MENU_H
#define MENU_H

#ifdef _WIN32 
#include <windows.h>
#include <windowsx.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "Base/camera.h"
#include "Base/Billboard.h"
#include "Base/BillboardAnimation.h"
#include <iostream>
#include "Base/Scene.h"
#include "Texto.h"
#include "Billboard2D.h"
#include "CollitionBox.h"

class Menu {
private:
	std::vector<Billboard2D*> billBoard2D;
	Billboard2D* camara;
public:
	Menu(Camera *cam);
	Menu(Model *camIni);
	void InitGraph(Camera *main);

    int update(int &menuOption);

    //el metodo render toma el dispositivo sobre el cual va a dibujar
	//y hace su tarea ya conocida
	Menu* Render();

	std::vector<Billboard2D*> *getLoadedBillboards2D();
	Model* getMainModel();
	void setMainModel(Billboard2D* mainModel);
    ~Menu();
};

#endif