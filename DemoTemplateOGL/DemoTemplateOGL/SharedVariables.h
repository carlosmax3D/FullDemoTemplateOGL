// SharedVariables.h
#pragma once
#include <glm/glm.hpp>

// Declaraciones externas de variables globales
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern glm::vec2 windowSize;
extern bool showHitbox;
extern bool showStats;
extern bool newContext;
extern bool mouseEnabled;
extern struct GameTime gameTime;
extern class Scene* OGLobj;

#ifdef _WIN32
extern struct HINSTANCE__* hInst;
extern struct HWND__* hWnd;
extern wchar_t szTitle[100];
extern wchar_t szWindowClass[100];
extern struct HDC__* dc;
extern struct HGLRC__* rc;
extern class GamePadRR* gamPad;
extern bool renderiza;
#endif