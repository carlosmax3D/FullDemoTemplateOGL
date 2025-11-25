#ifndef _XBOX_CONTROLLER_H_
#define _XBOX_CONTROLLER_H_

//no agrega en la compilada drivers de comunicaciones y otras cosas
#define WIN32_LEAN_AND_MEAN

// Incluimos estas librerias para avanzar
#include <windows.h>
#include <XInput.h> //libreria del Xbox solo gamepads o joysticks compatibles
#include "KeyboardInput.h" // Para GameActions si es necesario

// XBOX Controller Class Definition
class GamePadRR
{

private:
	XINPUT_STATE estadoControlador;
	int numeroControlador;
	// Deadzone para sticks
	static constexpr int DEADZONE_LX = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	static constexpr int DEADZONE_LY = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	static constexpr int DEADZONE_RX = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
	static constexpr int DEADZONE_RY = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
public:
	GamePadRR(int jugadorNumero) { numeroControlador = jugadorNumero - 1; }
	XINPUT_STATE GetState() {
		// limpia la estructura del estado del gamepad
		ZeroMemory(&estadoControlador, sizeof(XINPUT_STATE));

		// obtiene el estado del gamepad
		XInputGetState(numeroControlador, &estadoControlador);

		return estadoControlador;
	}
	bool IsConnected() {
		// limpia la estructura de estado
		ZeroMemory(&estadoControlador, sizeof(XINPUT_STATE));

		// obtiene el estado
		DWORD Resultado = XInputGetState(numeroControlador, &estadoControlador);

		if (Resultado == ERROR_SUCCESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	void Vibrate(int leftVal = 0, int rightVal = 0) {
		// Createl estado de vibracion
		XINPUT_VIBRATION Vibracion;

		// limpia los valores previos de la estructura
		ZeroMemory(&Vibracion, sizeof(XINPUT_VIBRATION));

		// establece los valores de vibracion
		Vibracion.wLeftMotorSpeed = leftVal;
		Vibracion.wRightMotorSpeed = rightVal;

		// Vibra el controlador
		XInputSetState(numeroControlador, &Vibracion);
	}
	void ApplyToActions(GameActions* actions) {
		if (!actions) return;
		XINPUT_STATE st = GetState();
		WORD buttons = st.Gamepad.wButtons;
		// D-Pad como WASD
		if (buttons & XINPUT_GAMEPAD_DPAD_UP) {
			actions->advance = 1;
		} else if (buttons & XINPUT_GAMEPAD_DPAD_DOWN) {
			actions->advance = -1;
		}
		if (buttons & XINPUT_GAMEPAD_DPAD_LEFT) {
			actions->sideAdvance = 1; // igual que tecla A
		} else if (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) {
			actions->sideAdvance = -1; // igual que tecla D
		}
		// Botón A -> salto (Space)
		if ((buttons & XINPUT_GAMEPAD_A) && actions->jump && *actions->jump == 0) {
			*actions->jump = 20; // misma fuerza que teclado
		}
		// Botón B -> primera persona (como tecla P)
		if (buttons & XINPUT_GAMEPAD_B) {
			actions->firstPerson = true; // se procesa como toggle en checkInput
		}
		// Botón X -> toggle hitbox/stats (como C)
		if (buttons & XINPUT_GAMEPAD_X) {
			actions->displayHitboxStats = true;
		}
		// Stick derecho -> cámara (yaw/pitch)
		SHORT rx = st.Gamepad.sThumbRX;
		SHORT ry = st.Gamepad.sThumbRY;
		if (abs(rx) > DEADZONE_RX) {
			float normX = (float)rx / 32767.0f; // -1..1
			// Invertido antes; cambiamos el signo para que derecha = gira a la derecha
			actions->setAngle(-normX * 3.0f); // ajustar yaw correcto
		}
		if (abs(ry) > DEADZONE_RY) {
			float normY = (float)ry / 32767.0f; // -1..1
			actions->setPitch(-normY * 3.0f); // mantener inversión vertical natural
		}
		// Gatillos -> rueda mouse (zoom). Usamos umbral para evitar ruido
		const int triggerThreshold = 30; // ~12% presión
		bool leftTrig = st.Gamepad.bLeftTrigger > triggerThreshold;
		bool rightTrig = st.Gamepad.bRightTrigger > triggerThreshold;
		if (leftTrig ^ rightTrig) { // solo uno
			int wheelDir = rightTrig ? 1 : -1; // derecho sube, izquierdo baja
			// Emular lógica de mouseWheel: si KEYB_CAMERA presionado -> zoom de cámara, si no -> player zoom
			if (KEYS[KEYB_CAMERA]) actions->setZoom((float)wheelDir);
			else actions->setPlayerZoom((float)wheelDir);
		}
	}

	// estas son las equivalencias a los botonazos mas comunes
	/*XINPUT_GAMEPAD_DPAD_UP          0x00000001
	XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
	XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
	XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
	XINPUT_GAMEPAD_START            0x00000010
	XINPUT_GAMEPAD_BACK             0x00000020
	XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
	XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080
	XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
	XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
	XINPUT_GAMEPAD_A                0x1000
	XINPUT_GAMEPAD_B                0x2000
	XINPUT_GAMEPAD_X                0x4000
	XINPUT_GAMEPAD_Y                0x8000*/
};

#endif