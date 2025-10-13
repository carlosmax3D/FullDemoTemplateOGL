// DemoTemplateOGL.cpp : Defines the entry point for the application.
//
#include "WinAPIHeaders/framework.h"
#include "WinAPIHeaders/DemoTemplateOGL.h"
#include "Base/Utilities.h"
#include "InputDevices/KeyboardInput.h"
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Base/glext.h"
#include "Base/wglext.h"
#include "Base/model.h"
#include "Base/Scene.h"
#include "Scenario.h"

#define MAX_LOADSTRING 100
#ifdef _WIN32 
#include "InputDevices/GamePadRR.h"
HINSTANCE hInst;                                // current instance
HWND hWnd;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HDC dc;
HGLRC rc;
GamePadRR* gamPad;                  // Manejador de gamepad
// Funciones para activar OpenGL version > 2.0
int prepareRenderWindow(HINSTANCE hInstance, int nCmdShow);
bool SetUpPixelFormat(HDC hDC, PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB, PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB);
// Callback principal de la ventana en WINAPI
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
#else
#include <GLFW/glfw3.h>
#define ZeroMemory(x,y) memset(x,0,y)
#define wcscpy_s(x,y,z) wcscpy(x,z)
#define wcscat_s(x,y,z) wcscat(x,z)
GLFWwindow* window;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
#endif

#define Timer1 100
bool renderiza;                     // Variable para controlar el render
bool checkInput(GameActions* actions, Scene* scene);
void mouseActions();
int isProgramRunning(void *ptr);
void swapGLBuffers();
int finishProgram(void *ptr);
int gamePadEvents(GameActions *actions);
void updatePosCords(Texto* coordenadas);
void updateFPS(Texto *fps, int totFrames);
int startGameEngine(void* ptrMsg);
void window_size_callback(void* window, int width, int height);
void changeLockingMouse();

// Propiedades de la ventana
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
glm::vec2 windowSize;
glm::vec2 windowCenter;
bool lockMouse = true;
bool showHitbox = true;
bool showStats = true;
bool newContext = false; // Bandera para identificar si OpenGL 2.0 > esta activa
struct GameTime gameTime;
Camera* Camera::cameraInstance = NULL;

// Objecto de escena y render
Scene *OGLobj;

#ifdef _WIN32 
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DEMOTEMPLATEOGL, szWindowClass, MAX_LOADSTRING);
    // Si no logra activar OpenGL 2 o superior termina el programa
    if (prepareRenderWindow(hInstance, nCmdShow))
        return 1;
    LOGGER::LOGS::getLOGGER().setWindow(&hWnd);
    // game loop
    gamPad = new GamePadRR(1); // Obtenemos el primer gamepad conectado
    MSG msg = { 0 };
    void *ptrMsg = (void*)&msg;
    window_size_callback(&hWnd, windowSize.x, windowSize.y);
#else
int main(int argc, char** argv){
    if (!glfwInit()){
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT, "DemoTemplateOGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        ERRORL("No opengl load", "OPENGL");
        glfwTerminate();
        return -1;
    }
    window_size_callback((void*)window, SCR_WIDTH, SCR_HEIGHT);
    void *ptrMsg = NULL;
#endif
    return startGameEngine(ptrMsg);
}

int startGameEngine(void *ptrMsg){
    // Main character with it's camera
    glm::vec3 translate, scale, v(0, 0, -1);
    translate = glm::vec3(5.0f, 10.0f, -5.0f);
    //5, ye - 1,-5
    //MainModel *model = new MainModel(hWnd, "models/Cube.obj", translate);
    Camera* camera = Camera::getInstance();
    Model* model = new Model("models/BaseSpiderman/BaseSpiderman.obj", translate, camera);
    model->setTranslate(&translate);
    camera->setFront(v);
    camera->setCharacterHeight(4.0);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
    model->setScale(&scale);
    model->setTranslate(&translate);
    Texto *fps = NULL;
    Texto *coordenadas = NULL;
    try{
        OGLobj = new Scenario(model); // Creamos nuestra escena con esa posicion de inicio
        translate = glm::vec3(5.0f, OGLobj->getTerreno()->Superficie(5.0, -5.0), -5.0f);
        model->setTranslate(&translate);
        model->setNextTranslate(&translate);
        renderiza = false;

        int running = 1;
        fps = new Texto((WCHAR*)L"0 fps", 20, 0, 0, 22, 0, model);
        fps->name = "FPSCounter";
        OGLobj->getLoadedText()->emplace_back(fps);
        coordenadas = new Texto((WCHAR*)L"0", 20, 0, 0, 0, 0, model);;
        coordenadas->name = "Coordenadas";
        OGLobj->getLoadedText()->emplace_back(coordenadas);
        updatePosCords(coordenadas);
        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        gameTime.lastTick = get_nanos() / 1000000.0; // ms
        int totFrames = 0;
        double deltasCount = 0;
        double jump = 0;
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        while (isProgramRunning(ptrMsg)) {
            deltasCount += gameTime.deltaTime;
            totFrames++;
            if (deltasCount >= 1000.0f){
                updateFPS(fps, totFrames);
                deltasCount -= 1000.0f;
                totFrames = 1;
            }
            updatePosCords(coordenadas);
            GameActions actions;
            actions.jump = &jump;
            // render
            // ------
            bool checkCollition = checkInput(&actions, OGLobj);
            int cambio = OGLobj->update();
            Scene *escena = OGLobj->Render();
            if (escena != OGLobj) {
                delete OGLobj;
                OGLobj = escena;
                OGLobj->getLoadedText()->emplace_back(fps);
                OGLobj->getLoadedText()->emplace_back(coordenadas);
            }
            swapGLBuffers();
        }
    }catch(...){
    }
    model = OGLobj != NULL ? OGLobj->getMainModel() : model;
    if (OGLobj != NULL) delete OGLobj;
    if (camera != NULL) delete camera;
    if (model != NULL) delete model;
    if (fps != NULL) delete fps;
    if (coordenadas != NULL) delete coordenadas;
    font_atlas::clearInstance();
    return finishProgram(ptrMsg);
}

bool checkInput(GameActions *actions, Scene* scene) {
    bool changeAnimation = false;
    if (gamePadEvents(actions)){
    } else {
        mouseActions();
        KeysEvents(actions);
    }
    Model* OGLobj = scene->getMainModel();
    if (actions->displayHitboxStats){
        showHitbox = !showHitbox;
        showStats = !showStats;
    }
    if (actions->lockMouse) {
        lockMouse = !lockMouse;
        changeLockingMouse();
    }
    if (actions->firstPerson) {
        OGLobj->cameraDetails->setFirstPerson(!OGLobj->cameraDetails->getFirstPerson());
    }
    if (actions->sideAdvance != 0) {
        glm::vec3 pos = *OGLobj->getNextTranslate();
        glm::vec3 right = OGLobj->cameraDetails->getFront();
        float rotY = glm::radians(OGLobj->getNextRotY());
        float speed = 3.0f * gameTime.deltaTime / 100.0f;
        pos += right * (speed * -actions->sideAdvance);
        OGLobj->setNextTranslate(&pos);
    }
    if (actions->hAdvance != 0) {
        glm::vec3 pos = *OGLobj->getNextTranslate();
        pos.x += actions->hAdvance * (3 * gameTime.deltaTime / 100) * glm::cos(glm::radians(OGLobj->getRotY()));
        pos.z += actions->hAdvance * (3 * gameTime.deltaTime / 100) * glm::sin(glm::radians(OGLobj->getRotY()));
        OGLobj->setNextTranslate(&pos);
    }
    if (actions->advance != 0) {
        glm::vec3 pos = *OGLobj->getNextTranslate();
        pos.x += actions->advance * (3 * gameTime.deltaTime / 100) * glm::sin(glm::radians(OGLobj->getRotY()));
        pos.z += actions->advance * (3 * gameTime.deltaTime / 100) * glm::cos(glm::radians(OGLobj->getRotY()));
        // Posicionamos la camara/modelo pixeles arriba de su posicion en el terreno
//        pos.y = *actions->jump > 0 ? pos.y : scene->getTerreno()->Superficie(pos.x, pos.z);
        OGLobj->setNextTranslate(&pos);
    }
    if (*actions->jump > 0){
        glm::vec3 pos = *OGLobj->getNextTranslate();
        double del = (*actions->jump) * gameTime.deltaTime / 100;
        pos.y += del;
        (*actions->jump) -= del;
        if (*actions->jump < 0.01f)
            *actions->jump = 0.0f;
        // Posicionamos la camara/modelo pixeles arriba de su posicion en el terreno
        OGLobj->setNextTranslate(&pos);
    }
    if (actions->getAngle() != NULL) {
//        OGLobj->cameraDetails->calculateAngleAroundPlayer((*actions->getAngle()) * (6 * gameTime.deltaTime / 100));
    }
    if (actions->getPitch() != NULL) {
//        OGLobj->cameraDetails->setPitch(OGLobj->cameraDetails->getPitch() + (*actions->getPitch()) * (6 * gameTime.deltaTime / 100));
    }
    if (actions->getZoom() != NULL) {
        OGLobj->cameraDetails->setZoom(OGLobj->cameraDetails->getZoom() + *actions->getZoom() * (6 * gameTime.deltaTime / 100));
    }
    if (actions->getPlayerZoom() != NULL) {
        OGLobj->cameraDetails->calculateZoomPlayer(*actions->getPlayerZoom() * (6 * gameTime.deltaTime / 100));
    }

    return true; // siempre buscar colision
}

#ifdef _WIN32
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            RECT rect;
            if (GetClientRect(hWnd, &rect)) 
                windowSize = glm::vec2(rect.right - rect.left, rect.bottom - rect.top);
            else windowSize = glm::vec2(0);
        } break;
        case WM_COMMAND: {
            switch (wParam) {
                case 9999: memset(KEYS, 0, 256 * sizeof(bool));
                        break;
            }
        } break;
        case WM_MOUSEMOVE: {
            int value = lParam;
        }break;
        case WM_TIMER: {
        } break;
        case WM_PAINT: {
        }break;
        case WM_DESTROY: {
            if (newContext) {
                ReleaseDC(hWnd, dc);
                wglDeleteContext(rc);
                PostQuitMessage(0);
            }
        } break;
        case WM_SIZE: {
            if (newContext) {
                //esta opcion del switch se ejecuta una sola vez al arrancar y si se
                //afecta el tama�o de la misma se dispara de nuevo
                int height = HIWORD(lParam),
                    width = LOWORD(lParam);
                window_size_callback(&hWnd, width, height);
            }
        } break;
        case WM_LBUTTONDOWN: {
            cDelta.setLbtn(true);
        }break;
        case WM_LBUTTONUP: {
            cDelta.setLbtn(false);
        }break;
        case WM_RBUTTONDOWN: {
            cDelta.setRbtn(true);
        }break;
        case WM_RBUTTONUP: {
            cDelta.setRbtn(false);
        }break;
        case WM_MOUSEWHEEL: {
            char delta = HIWORD(wParam);
            cDelta.setMouseWheel(delta);
        }break;
        case WM_KEYDOWN: {
            KEYS[wParam] = true;
        } break;
        case WM_KEYUP: {
//            if (wParam == KEYB_CAMERA || wParam == KEYB_HMOVEMENT)
                KEYS[wParam] = false;
        } break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

bool SetUpPixelFormat(HDC hDC, PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB, PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB) {
    const int pixelAttribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
//        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
//        WGL_SAMPLES_ARB, 4,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_ALPHA_BITS_ARB, 8,
        0 
    };
    int pixelFormatID;
    UINT numFormats;
    bool status = wglChoosePixelFormatARB(hDC, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
    if (status == false || numFormats == 0)
        return true;
    PIXELFORMATDESCRIPTOR PFD;
    DescribePixelFormat(hDC, pixelFormatID, sizeof(PFD), &PFD);
    SetPixelFormat(hDC, pixelFormatID, &PFD);
    return false;
}

// Funciones de inicializacion para ventana compatible con OpenGL
int prepareRenderWindow(HINSTANCE hInstance, int nCmdShow) {
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = szWindowClass;
    RegisterClassEx(&wc);
    hInst = hInstance;
    HWND fakeWND = CreateWindow(
        szWindowClass, L"Fake Window",      // window class, title
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // style
        0, 0,                       // position x, y
        1, 1,                       // width, height
        NULL, NULL,                 // parent window, menu
        hInstance, NULL);           // instance, param
    HDC fakeDC = GetDC(fakeWND);        // Device Context
    PIXELFORMATDESCRIPTOR fakePFD;
    ZeroMemory(&fakePFD, sizeof(fakePFD));
    fakePFD.nSize = sizeof(fakePFD);
    fakePFD.nVersion = 1;
    fakePFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    fakePFD.iPixelType = PFD_TYPE_RGBA;
    fakePFD.cColorBits = 32;
    fakePFD.cAlphaBits = 8;
    fakePFD.cDepthBits = 24;
    int fakePFDID = ChoosePixelFormat(fakeDC, &fakePFD);
    if (fakePFDID == 0) {
        MessageBox(fakeWND, L"ChoosePixelFormat() failed.", L"", 0);
        return 1;
    }
    if (SetPixelFormat(fakeDC, fakePFDID, &fakePFD) == false) {
        MessageBox(fakeWND, L"SetPixelFormat() failed.", L"", 0);
        return 1;
    }
    HGLRC fakeRC = wglCreateContext(fakeDC);    // Rendering Contex
    if (fakeRC == 0) {
        MessageBox(fakeWND, L"wglCreateContext() failed.", L"", 0);
        return 1;
    }
    if (wglMakeCurrent(fakeDC, fakeRC) == false) {
        MessageBox(fakeWND, L"wglMakeCurrent() failed.", L"", 0);
        return 1;
    }
    wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
    if (wglChoosePixelFormatARB == nullptr) {
        MessageBox(fakeWND, L"wglGetProcAddress() failed.", L"", 0);
        return 1;
    }
    wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
    if (wglCreateContextAttribsARB == nullptr) {
        MessageBox(fakeWND, L"wglGetProcAddress() failed.", L"", 0);
        return 1;
    }
    gladLoadGL();
    // create window
    RECT wr = { 0, 0, SCR_WIDTH, SCR_HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindow(szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100, 100,
        wr.right - wr.left, //SCR_WIDTH, 
        wr.bottom - wr.top, //SCR_HEIGHT,
        NULL, NULL, hInstance, NULL);
    if (hWnd == NULL) {
        return 1;
    }
    dc = NULL;
    dc = GetDC(hWnd);
    if (SetUpPixelFormat(dc, wglChoosePixelFormatARB, wglCreateContextAttribsARB)) {
        MessageBox(hWnd, L"wglChoosePixelFormatARB() failed.", L"", 0);
        return 1;
    }
    const int major_min = 3, minor_min = 3;
    int  contextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    rc = NULL;
//    rc = wglCreateContext(dc);
    rc = wglCreateContextAttribsARB(dc, 0, contextAttribs);
    if (rc == NULL) {
        MessageBox(hWnd, L"wglCreateContextAttribsARB() failed.", L"", 0);
        return 1;
    }
//    wglMakeCurrent(dc, rc);
    wglDeleteContext(fakeRC);
    ReleaseDC(fakeWND, fakeDC);
    DestroyWindow(fakeWND);
    if (!wglMakeCurrent(dc, rc)) {
        MessageBox(hWnd, L"wglMakeCurrent() failed.", L"", 0);
        return 1;
    }
    gladLoadGL();
    newContext = true;
    return 0;
}
#endif

void window_size_callback(void* window, int width, int height){
    int centerX, centerY;
    if (height == 0)
        width = 1;
    SCR_HEIGHT = height;
    SCR_WIDTH = width;
    glViewport(0, 0, width, height);
#ifndef _WIN32
    windowSize = glm::vec2(SCR_WIDTH, SCR_HEIGHT);
    glfwMakeContextCurrent((GLFWwindow*)window);
    // --- Step 1: Get framebuffer size (actual pixel area used by OpenGL) ---
    int fbWidth, fbHeight;
    glfwGetFramebufferSize((GLFWwindow*)window, &fbWidth, &fbHeight);
    // --- Step 2: Calculate framebuffer center ---
    centerX = fbWidth / 2.0;
    centerY = fbHeight / 2.0;
    if (lockMouse) {
        glfwSetInputMode((GLFWwindow*)window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode((GLFWwindow*)window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
#else
    HWND &hWnd = *((HWND*)window);
    RECT rect;
    if (GetClientRect(hWnd, &rect)) {
        // Center in client coordinates (relative to window)
        POINT topLeft = { rect.left, rect.top };
        POINT bottomRight = { rect.right, rect.bottom };
        ClientToScreen(hWnd, &topLeft);
        ClientToScreen(hWnd, &bottomRight);
        rect.left = topLeft.x;
        rect.top = topLeft.y;
        rect.right = bottomRight.x;
        rect.bottom = bottomRight.y;
        windowSize = glm::vec2(rect.right - rect.left, rect.bottom - rect.top);
    }
    else windowSize = glm::vec2(0);

    windowCenter = glm::vec2(windowSize.x / 2.0f, windowSize.y / 2.0f);

    // If you need the center in *screen coordinates*, convert it:
    POINT pt;
    pt.x = static_cast<LONG>(windowCenter.x);
    pt.y = static_cast<LONG>(windowCenter.y);

    // ClientToScreen converts from window-relative to screen coordinates
    ClientToScreen(hWnd, &pt);
    centerX = pt.x;
    centerY = pt.y;
    if (lockMouse) {
        CURSORINFO cursor = { 0 };
        GetCursorInfo(&cursor);
        if (cursor.flags != CURSOR_SUPPRESSED || cursor.flags != 0)
            while (ShowCursor(FALSE) == 0);
        ClipCursor(&rect);
    }
#endif
    // --- Step 3: Set window center coordinates ---
    windowCenter = glm::vec2(centerX, centerY);
}

#ifndef _WIN32
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    char k = (key == GLFW_KEY_LEFT_SHIFT)? input.Shift : key;
    if (action == GLFW_PRESS || action == GLFW_REPEAT || action == GLFW_RELEASE){
        if (k > 5 && k < 10)
            switch(k){
                case 6: KEYS[input.Right] = GLFW_RELEASE == action ? false : true;
                    break;
                case 7: KEYS[input.Left] = GLFW_RELEASE == action ? false : true;
                    break;
                case 8: KEYS[input.Down] = GLFW_RELEASE == action ? false : true;
                    break;
                case 9: KEYS[input.Up] = GLFW_RELEASE == action ? false : true;
                    break;
            }
        else
            KEYS[k] = GLFW_RELEASE == action ? false : true;
    }else
        if (k == KEYB_CAMERA || k == KEYB_HMOVEMENT)
            KEYS[k] = false;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
        cDelta.setRbtn(action == GLFW_PRESS);
    else if (button == GLFW_MOUSE_BUTTON_LEFT)
        cDelta.setLbtn(action == GLFW_PRESS);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    char delta = yoffset;
    cDelta.setMouseWheel(delta);
}
#endif

void mouseActions() {
    static int lastX = 0.0;
    static int lastY = 0.0;
    static bool first = true;
    double x, y;
    if (!lockMouse)
        return;
#ifdef _WIN32 
    POINT p;
    GetCursorPos(&p);
    x = (int)p.x;
    y = (int)p.y;
    if (first) {
        lastX = windowCenter.x;
        lastY = windowCenter.y;
        x = lastX;
        y = lastY;
    }
#else
    glfwGetCursorPos(window, &x, &y);
#endif
    // Compute delta movement
    double dx = 0.0, dy = 0.0;
    if (!first) {
        dx = x - lastX;
        dy = y - lastY;
    } else {
        first = false;
    }
    // Ignore if there’s no movement
    if (dx == 0.0 && dy == 0.0) return;
    // Convert to normalized scale
    glm::vec2 scale(dx / windowSize.x, dy / windowSize.y);

    // Example: use dy to modify pitch, dx to rotate camera
    auto cam = OGLobj->getMainModel()->cameraDetails;
    Model* main = OGLobj->getMainModel();
    float nextRotY = main->getNextRotY();

    main->setNextRotY(nextRotY - (scale.x * 1000.0f * gameTime.deltaTime / 100));
//    cDelta.setPosition(dx, dy, cDelta.getLbtn() || cDelta.getRbtn());
    if (scale.y != 0)
        cam->setPitch(cam->getPitch() + (scale.y * 100.0f)); // invert Y as needed
//        cam->calculateAngleAroundPlayer((scale.x / abs(scale.x)) * -3.0);
    scale = cDelta.setPosition(x, y, true);
#ifdef _WIN32 
    lastX = (int)windowCenter.x;
    lastY = (int)windowCenter.y;
    // set center to fps
    SetCursorPos(lastX, lastY);
#else
    lastX = x;
    lastY = y;
    // set center to fps
    glfwSetCursorPos(window, lastX, lastY);
#endif
}

int isProgramRunning(void *ptr){
    double currentTime = get_nanos() / 1000000.0;
    gameTime.deltaTime =  currentTime - gameTime.lastTick; // ms
    gameTime.lastTick = currentTime;  // ms
    int flag = 1;
#ifndef _WIN32
    if (!renderiza){
        glfwSetKeyCallback(window, key_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
        glfwSetWindowSizeCallback(window, (GLFWwindowsizefun)window_size_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        renderiza = true;
    }
    flag = !glfwWindowShouldClose(window);
    if (flag)
        glfwPollEvents();
#else
    if (!renderiza)
        renderiza = true;
    MSG &msg = *(MSG*)ptr;
    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
        flag = msg.message == WM_QUIT? 0 : 1;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#endif
    return flag;
}

void swapGLBuffers(){
#ifdef _WIN32
    SwapBuffers(dc);
#else
    glfwSwapBuffers(window);
#endif

}

int finishProgram(void *ptr){
#ifdef _WIN32
    MSG &msg = *(MSG*)ptr;
    return (int)msg.wParam;
#else
    glfwTerminate();
    return 0;
#endif
}

int gamePadEvents(GameActions *actions){
#ifdef _WIN32 
    if (gamPad->IsConnected()) {
        //convierto a flotante el valor analogico de tipo entero
        double grados = (float)gamPad->GetState().Gamepad.sThumbLX / 32767.0;
        //debido a que los controles se aguadean con el uso entonces ya no dan el cero
        //en el centro, por eso lo comparo con una ventana de aguadencia de mi control
        if (grados > 0.19 || grados < -0.19)
            //model->CamaraGiraY(grados * 3.0);
            actions->setAngle(grados * 3.0);
        float velocidad = (float)gamPad->GetState().Gamepad.sThumbLY / 32767;
        if (velocidad > 0.19 || velocidad < -0.19) {
            //model->movePosition(velocidad);
            actions->advance = velocidad;
        }
        return 1;
    } else
        return 0;
#else
    if (false)
        ERRORL("This should be the gamepad code", "GAMEPAD");
    return 0;
#endif
}

void updatePosCords(Texto* coordenadas) {
    wchar_t wCoordenadas[350] = { 0 };
	wchar_t componente[100] = { 0 };
	wcscpy_s(wCoordenadas, 350, L"X: ");
	swprintf(componente, 100, L"%f", OGLobj->getMainModel()->getTranslate()->x);
	wcscat_s(wCoordenadas, 350, componente);
	wcscat_s(wCoordenadas, 350, L" Y: ");
	swprintf(componente, 100, L"%f", OGLobj->getMainModel()->getTranslate()->y);
	wcscat_s(wCoordenadas, 350, componente);
	wcscat_s(wCoordenadas, 350, L" Z: ");
	swprintf(componente, 100, L"%f", OGLobj->getMainModel()->getTranslate()->z);
	wcscat_s(wCoordenadas, 350, componente);
	coordenadas->initTexto((WCHAR*)wCoordenadas);
}

void updateFPS(Texto *fps, int totFrames){
    WCHAR conv[50] = { 0 };
    swprintf((wchar_t*)conv, 50, L"%d", totFrames);
    wcscat_s((wchar_t*)conv, 50, L" FPS");
    fps->initTexto(conv);
}

void changeLockingMouse() {
    if (lockMouse) {
#ifdef _WIN32 
        window_size_callback(&hWnd, SCR_WIDTH, SCR_HEIGHT);
#else
        window_size_callback(window, SCR_WIDTH, SCR_HEIGHT);
#endif
    } else {
#ifdef _WIN32 
        CURSORINFO cursor{ 0 };
        GetCursorInfo(&cursor);
        if (cursor.flags != CURSOR_SHOWING)
            while (ShowCursor(TRUE) == CURSOR_SHOWING);
        ClipCursor(NULL);
#else
        glfwSetInputMode((GLFWwindow*)window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif
    }
}