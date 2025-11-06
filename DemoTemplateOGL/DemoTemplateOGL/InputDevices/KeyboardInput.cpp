#include "KeyboardInput.h"

#ifdef _WIN32
extern HWND hWnd;
#else
extern void *hWnd;
#endif
extern bool mouseEnabled;//ZITOS
bool mouseLocked;//ZITOS

class MouseInput;
MouseInput::MouseInput() {
	prevP.x = 0;
	prevP.y = 0;
	currP.x = 0;
	currP.y = 0;
	delta.x = 0;
	delta.y = 0;
	lbtn = false;
	rbtn = false;
	mouseWheel = 0;
}
float MouseInput::getDY() { return delta.y; }
float MouseInput::getDX() { return delta.x; }
char MouseInput::getMouseWheel() { return mouseWheel; }
void MouseInput::setMouseWheel(char x) { mouseWheel = x; }
bool MouseInput::getLbtn() { return lbtn; }
bool MouseInput::getRbtn() { return rbtn; }
void MouseInput::setLbtn(bool v) { lbtn = v; }
void MouseInput::setRbtn(bool v) { rbtn = v; }
glm::vec2 MouseInput::getDelta() { return delta; }
glm::vec2 MouseInput::setPosition(double x, double y, bool isDelta) {
	prevP = currP;
	currP.x = x;
	currP.y = y;
	if (!isDelta) {
		prevP = currP;
		delta.x = 0;
		delta.y = 0;
	}
	else {
		delta.x = prevP.x - currP.x;
		delta.y = prevP.y - currP.y;
	}
	return delta;
}
glm::vec2 MouseInput::setPosition(glm::vec2 d, bool isDelta) {
	return setPosition(d.x, d.y, isDelta);
}
struct Input input;
bool KEYS[256];
MouseInput cDelta;

void Init() {
	for (int i = 0; i < 256; i++) {
		KEYS[i] = false;
	}
}//Initizalizes keys

void ForceShowCursor(bool show) {
#ifdef _WIN32
    if (show) {
        // Asegura que el contador sea >= 0
        while (ShowCursor(TRUE) < 0);
    }
    else {
        // Asegura que el contador sea < 0
        while (ShowCursor(FALSE) >= 0);
    }
#endif
}

bool KeysEvents(GameActions* actions) {


    //checar esto para error del mouse

//    if (KEYS[input.M]) {
//        actions->lockMouse = true;
//        mouseEnabled = !mouseEnabled; // alterna estado
//
//#ifdef _WIN32
//        ShowCursor(!mouseEnabled);    // si mouseEnabled es true, oculta cursor
//#else
//        glfwSetInputMode(window, GLFW_CURSOR, mouseEnabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
//#endif
//
//        KEYS[input.M] = false; // evita que quede presionada
//
//
//        
//
//
//        //ZITOS
//    }


    //ZITOS
    if (KEYS[input.M]) {
		actions->lockMouse = true;
        KEYS[input.M] = false;
    }

    //ZITOS  
    //bool checkCollition = false;

    // Teclas de juego
    if (KEYS[input.P]) {
        actions->firstPerson = true;
        KEYS[input.P] = false;
    }
    if (KEYS[input.C]) {
        actions->displayHitboxStats = !actions->displayHitboxStats;
        KEYS[input.C] = false;
    }


	bool checkCollition = false;
    static int lastKey = 0; // 0 = nada, 1 = A, 2 = D
	if (KEYS[input.P]) {
		actions->firstPerson = true;
		KEYS[input.P] = false;
	}
	if (KEYS[input.C]) {
		actions->displayHitboxStats = !actions->displayHitboxStats;
		KEYS[input.C] = false;
	}





	if (KEYS[input.D]) {
        
		if (KEYS[KEYB_HMOVEMENT])
			actions->hAdvance = -1;
		else
			actions->sideAdvance = -1;
//		KEYS[input.D] = false;
	}
	if (KEYS[input.A]) {
		if (KEYS[KEYB_HMOVEMENT])
			actions->hAdvance = 1;
		else
			actions->sideAdvance = 1;
//		KEYS[input.A] = false;
	}


   




	if (KEYS[input.W]) {
		actions->advance = 1;
//		KEYS[input.W] = false;
	}
	if (KEYS[input.S]) {
		actions->advance = -1;
//		KEYS[input.S] = false;
	}
	if (KEYS[input.Space] && *actions->jump == 0){
		*actions->jump = 20;
	}


    if (cDelta.getLbtn()) {
        actions->fired = true;
        if (cDelta.getDX() != 0) {
            actions->setAngle(cDelta.getDX() > 0 ? 1 : -1);
        }
        cDelta.setLbtn(false);
    }

    //ZITOS
    if (mouseEnabled) { // Solo si est� bloqueado
        if (cDelta.getLbtn() && cDelta.getDX() != 0)
            actions->setAngle(cDelta.getDX() > 0 ? 1 : -1);

        if (cDelta.getRbtn() && cDelta.getDY() != 0)
            actions->setPitch(cDelta.getDY() > 0 ? 1 : -1);
    }
    //ZITOS

	//if (cDelta.getLbtn() && cDelta.getDX() != 0) {
	//	actions->setAngle(cDelta.getDX() > 0 ? 1 : -1);
	//}
 //   if (cDelta.getRbtn() && cDelta.getDY() != 0) { //KEYS[KEYB_CAMERA]
 //       actions->setPitch(cDelta.getDY() > 0 ? 1 : -1);

 //       
 //   }
	
	if ((!KEYS[KEYB_CAMERA]) && cDelta.getMouseWheel() != 0) {
		actions->setPlayerZoom(cDelta.getMouseWheel() > 0 ? 1 : -1);
    }
    //else {
    //    actions->setPlayerZoom(1.0f);

    //}
	if (KEYS[KEYB_CAMERA] && cDelta.getMouseWheel() != 0) {
		actions->setZoom(cDelta.getMouseWheel() > 0 ? 1 : -1);
	}
	cDelta.setMouseWheel(0);
	return checkCollition;
}
