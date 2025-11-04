#pragma once
#include "Billboard2D.h"
#include "Base/camera.h"
class Menu
{
private:
	vector<Billboard2D*> billBoard2D;
	Camera *camDetails = NULL;
	Billboard* arrow;

	void initMenu();
public:
	Menu(Camera *cam);

	void Update(int opc);

	void Draw();
};

