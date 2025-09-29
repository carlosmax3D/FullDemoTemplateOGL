#pragma once
#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H
#define MOUSE_INPUT_H
#include "../Base/Utilities.h"

	struct Input {
		enum Keys {
			Backspace = 0x08, Tab,
			Clear = 0x0C, Enter,
			Shift = 0x10, Control, Alt,
			Escape = 0x1B,
			Space = 0x20, PageUp, PageDown, End, Home, Left, Up, Right, Down,
			Zero = 0x30, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
			A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
			NumPad0 = 0x60, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7, NumPad8, NumPad9,
			F1 = 0x70, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
		};
	};

#define KEYB_CAMERA input.V
#define KEYB_HMOVEMENT input.Control
#define KEYB_SPRINT input.Shift

class MouseInput {
	private:
		glm::vec2 prevP;
		glm::vec2 currP;
		glm::vec2 delta;
		char mouseWheel;
		bool lbtn;
		bool rbtn;
	public:
		MouseInput();
		float getDY();
		float getDX();
		glm::vec2 getDelta();
		glm::vec2 setPosition(double x, double y, bool isDelta = false);
		glm::vec2 setPosition(glm::vec2 d, bool isDelta = false);
		bool getLbtn();
		bool getRbtn();
		void setLbtn(bool v);
		void setRbtn(bool v);
		char getMouseWheel();
		void setMouseWheel(char x);
};
	extern struct Input input;
	extern bool KeysEvents(GameActions *obj);
	extern void Init();
	extern MouseInput cDelta;

#endif
