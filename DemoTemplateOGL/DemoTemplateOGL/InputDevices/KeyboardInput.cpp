#include "KeyboardInput.h"

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

bool KeysEvents(GameActions *actions){
	bool checkCollition = false;
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
	if (cDelta.getLbtn() && cDelta.getDX() != 0) {
		actions->setAngle(cDelta.getDX() > 0 ? 1 : -1);
	}
	if (cDelta.getRbtn() && cDelta.getDY() != 0) { //KEYS[KEYB_CAMERA]
		actions->setPitch(cDelta.getDY() > 0 ? 1 : -1);
	}
	if ((!KEYS[KEYB_CAMERA]) && cDelta.getMouseWheel() != 0) {
		actions->setPlayerZoom(cDelta.getMouseWheel() > 0 ? 1 : -1);
	}
	if (KEYS[KEYB_CAMERA] && cDelta.getMouseWheel() != 0) {
		actions->setZoom(cDelta.getMouseWheel() > 0 ? 1 : -1);
	}
	cDelta.setMouseWheel(0);
	return checkCollition;
}
