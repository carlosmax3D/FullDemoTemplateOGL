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
#include "Player.h"

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
#include "Player.h"
#define ZeroMemory(x,y) memset(x,0,y)
#define wcscpy_s(x,y,z) wcscpy(x,z)
#define wcscat_s(x,y,z) wcscat(x,z)
GLFWwindow* window;
void window_size_callback(GLFWwindow* window, int width, int height);
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
void updateVida(Texto* vidaTexto);
int startGameEngine(void* ptrMsg);
void lockCursor(bool mouse);//ZITOS

// Propiedades de la ventana
//unsigned int SCR_WIDTH = 1920;
//unsigned int SCR_HEIGHT = 1200;
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
bool lockMouse = true;
glm::vec2 windowSize;
bool showHitbox = true;
bool showStats = true;
bool newContext = false; // Bandera para identificar si OpenGL 2.0 > esta activa
struct GameTime gameTime;
Camera* Camera::cameraInstance = NULL;

//ZITOS
bool mouseEnabled = true;
bool freeMouseCamera = false;
// Variables para controlar el mouse
double lastX_cam = 0.0;
double lastY_cam = 0.0;
bool firstMouse_cam = true;

extern bool mouseLocked; // desde KeyboardInput.cpp
extern bool mouseEnabled;
extern MouseInput cDelta;
bool ignoreNextDelta = false;


//glm::vec2 g_windowCenter(0.0f, 0.0f); // Centro de la ventana
//bool ignoreNextDelta = true; // <-- ignorar el primer delta después de resize
glm::vec2 g_windowCenter(0.0f, 0.0f);
glm::vec2 g_lastMousePos(0.0f, 0.0f);

bool firstMouse = true; // ignorar primer delta

//ZITOS

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
    ShowCursor(FALSE);//ZITOS
#else
int main(int argc, char** argv){
    if (!glfwInit()){
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT, "DemoTemplateOGL", NULL, NULL);
    windowSize = glm::vec2(SCR_WIDTH, SCR_HEIGHT);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        ERRORL("No opengl load", "OPENGL");
        glfwTerminate();
        return -1;
    }
    void *ptrMsg = NULL;
#endif
    return startGameEngine(ptrMsg);
}

int startGameEngine(void *ptrMsg){
    // Main character with it's camera
    glm::vec3 translate, scale, v(0, 0, -1);
    translate = glm::vec3(180.0f, 10.0f, 75.0f);

    //5, ye - 1,-5
    //MainModel *model = new MainModel(hWnd, "models/Cube.obj", translate);
    Camera* camera = Camera::getInstance();
    Model* model = new Model("models/Gonzalo/Idle.fbx", translate, camera);
    Player& player = Player::getInstance();


    model->setTranslate(&translate);
    camera->setFront(v);
    camera->setCharacterHeight(4.0);

    scale = glm::vec3(.00020f, .00020f, .00020f);	// it's a bit too big for our scene, so scale it down
    model->setScale(&scale);
    model->setTranslate(&translate);



    float width = model->AABBsize.m_halfWidth;
    float Height = model->AABBsize.m_halfHeight;
    float Depth = model->AABBsize.m_halfDepth;

    
    Texto *fps = NULL;
    Texto *coordenadas = NULL;
    try{
        OGLobj = new Scenario(model); // Creamos nuestra escena con esa posicion de inicio
        translate = glm::vec3(180.0f, OGLobj->getTerreno()->Superficie(5.0, -5.0), 75.0f);
        model->setTranslate(&translate);
        model->setNextTranslate(&translate);
        renderiza = false;


        delete model->getModelAttributes()->at(0).hitbox;
        Node n = model->AABBsize;

        n.m_center.y = 9000;
        n.m_halfWidth = width * 60;
        n.m_halfHeight = Height * 100;
        n.m_halfDepth = Depth * 100;

        model->getModelAttributes()->at(0).hitbox = CollitionBox::GenerateAABB(translate, n, camera);

        model->setScale(&scale);




        try{
            std::vector<Animation> animations = Animation::loadAllAnimations("models/Gonzalo/Idle.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
            std::vector<Animation> animation_caminar = Animation::loadAllAnimations("models/Gonzalo/Caminar_normal.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
            std::vector<Animation> animation_caminar_atras = Animation::loadAllAnimations("models/Gonzalo/Caminar_atras.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
            std::vector<Animation> animation_caminar_derecha = Animation::loadAllAnimations("models/Gonzalo/Caminar_derecha.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
            std::vector<Animation> animation_caminar_Izquierda = Animation::loadAllAnimations("models/Gonzalo/Caminar_izquierda.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
            std::vector<Animation> animation_saltar = Animation::loadAllAnimations("models/Gonzalo/Jump.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
            
            // Mover las animaciones de "animation" al final de "animations"
            std::move(animation_caminar.begin(), animation_caminar.end(), std::back_inserter(animations));

            // Mover las animaciones de "animation_1" al final de "animations"
            std::move(animation_caminar_atras.begin(), animation_caminar_atras.end(), std::back_inserter(animations));

            std::move(animation_caminar_derecha.begin(), animation_caminar_derecha.end(), std::back_inserter(animations));
            
            std::move(animation_caminar_Izquierda.begin(), animation_caminar_Izquierda.end(), std::back_inserter(animations));

            std::move(animation_saltar.begin(), animation_saltar.end(), std::back_inserter(animations));

            for (Animation animation : animations) 
            {
                
                model->setAnimator(Animator(animation));
            
            }
                
            model->setAnimation(0);
        }catch(...){
            ERRORL("Could not load animation!", "ANIMACION");
        }







        int running = 1;

        //ZITOS
        Texto* vida = new Texto((WCHAR*)L"Vida: 100", 50, 0, 0, 60, 0, model);
        vida->name = "Vida";
        OGLobj->getLoadedText()->emplace_back(vida);
        //ZITOS

        Texto *fps = new Texto((WCHAR*)L"0 fps", 20, 0, 0, 22, 0, model);
        fps->name = "FPSCounter";
        OGLobj->getLoadedText()->emplace_back(fps);
        Texto *coordenadas = new Texto((WCHAR*)L"0", 20, 0, 0, 0, 0, model);;
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


        //CUANDO ESTA CORRIENDO EL PROGRAMA
        while (isProgramRunning(ptrMsg)) {
            deltasCount += gameTime.deltaTime;
            totFrames++;
            if (deltasCount >= 1000.0f){
                updateFPS(fps, totFrames);
                deltasCount -= 1000.0f;
                totFrames = 1;
            }
            updatePosCords(coordenadas);


            //ZITOS
            updateVida(vida);
            //ZITOS



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

bool checkInput(GameActions* actions, Scene* scene) {
    bool changeAnimation = false;
    if (gamePadEvents(actions)) {
    }
    else {
        mouseActions();
        KeysEvents(actions);
    }
    //Model* OGLobj = scene->getMainModel();
    Model* playerModel = scene->getMainModel();  // ← CAMBIADO


    if (actions->fired && lockMouse) {
        auto models = scene->getLoadedModels();
        for (Model* m : *models) {
            if (m->name.compare("bullet_9_mm") != 0)
                continue;
            auto& attr = *m->getModelAttributes();
            ModelAttributes mattr = attr.at(0);
            mattr.forward = -playerModel->cameraDetails->getRight();
            mattr.hitbox = NULL;//CollitionBox::GenerateAABB(*OGLobj->getNextTranslate(), m->AABBsize, m->cameraDetails);
            attr.push_back(mattr);
            int idx = attr.size() - 1;
            glm::vec3 trans = *playerModel->getNextTranslate();
            trans.y += playerModel->cameraDetails->getCharacterHeight() / 2;
            trans = trans + mattr.forward * 1.5f;
            attr.back().origin = trans;
            attr.back().owner = OGLobj;
            attr.back().life = 30.0f;
            m->setTranslate(&trans, idx);
            m->setNextTranslate(&trans, idx);
            m->setNextRotX(mattr.nextRotX, idx);
            m->setNextRotY(playerModel->getNextRotY() + mattr.nextRotY, idx);
            m->setNextRotZ(mattr.nextRotZ, idx);
            m->setRotX(mattr.rotX, idx);
            m->setRotY(playerModel->getNextRotY() + mattr.rotY, idx);
            m->setRotZ(mattr.rotZ, idx);
            m->setScale(&mattr.scale, idx);
            break;
        }
    }
  

    //lineas a checarsa
    if (actions->lockMouse) {
        mouseEnabled = !mouseEnabled; // alterna estado
        lockCursor(false);
    }

    if (actions->displayHitboxStats) {
        showHitbox = !showHitbox;
        showStats = !showStats;
    }


    if (actions->firstPerson) {
        playerModel->cameraDetails->setFirstPerson(!playerModel->cameraDetails->getFirstPerson());
    }
    /*if (actions->sideAdvance != 0) {
        OGLobj->setNextRotY(OGLobj->getNextRotY() + ((6 * gameTime.deltaTime / 100) * actions->sideAdvance));
    }*/


    //ZITOS
    if (actions->sideAdvance != 0) {

        glm::vec3 pos = *playerModel->getNextTranslate();
        float currentRotationY = playerModel->getRotY();

        // MOVIMIENTO LATERAL corregido
        float angle = glm::radians(currentRotationY + (actions->sideAdvance > 0 ? 90.0f : -90.0f));

        pos.x += (3 * gameTime.deltaTime / 100) * glm::sin(angle);
        pos.z += (3 * gameTime.deltaTime / 100) * glm::cos(angle);



        // mantener en el terreno si no está saltando
        if (*actions->jump <= 0)
            pos.y = scene->getTerreno()->Superficie(pos.x, pos.z);

        playerModel->setNextTranslate(&pos);


    }


    //ZITOS

    if (actions->hAdvance != 0) {


        //ZITOS
        glm::vec3 pos = *playerModel->getTranslate();
        float currentRotationY = playerModel->getRotY();

        // MOVIMIENTO HORIZONTAL: en la dirección de mirada (o perpendicular, depende de lo que quieras)
        pos.x += actions->hAdvance * (3 * gameTime.deltaTime / 100) * glm::sin(glm::radians(currentRotationY + 90.0f));
        pos.z += actions->hAdvance * (3 * gameTime.deltaTime / 100) * glm::cos(glm::radians(currentRotationY + 90.0f));
        playerModel->setNextTranslate(&pos);




        //ZITOS
    }



    if (*actions->jump > 0) {
        glm::vec3 pos = *playerModel->getNextTranslate();
        double del = (*actions->jump) * gameTime.deltaTime / 100;
        pos.y += del;
        (*actions->jump) -= del;
        if (*actions->jump < 0.01f)
            *actions->jump = 0.0f;
        // Posicionamos la camara/modelo pixeles arriba de su posicion en el terreno
        playerModel->setNextTranslate(&pos);
    }
    //ZITOS
    if (actions->advance != 0) {


        //ZITOS

        glm::vec3 pos = *playerModel->getNextTranslate();
        float currentRotationY = playerModel->getRotY();

        // MOVIMIENTO ADELANTE/ATRÁS: en la dirección de mirada
        pos.x += actions->advance * (3 * gameTime.deltaTime / 100) * glm::sin(glm::radians(currentRotationY));
        pos.z += actions->advance * (3 * gameTime.deltaTime / 100) * glm::cos(glm::radians(currentRotationY));
        playerModel->setNextTranslate(&pos);



    }

//ZITOS
    //PONER ANIMACIONES

    int animationToPlay = 0; // Idle por defecto


    if (*actions->jump > 0) {
        animationToPlay = 5; // Saltar

    }
    else if (actions->advance > 0) {
        animationToPlay = 1; // caminar adelante


        //Player& player = Player::getInstance();

        //// Quitar 20 puntos de vida
        //player.takeDamage(1.0f);
    }

    else if (actions->advance < 0) {
        animationToPlay = 2; // caminar atrás
    }
    else if (actions->sideAdvance < 0) {
        animationToPlay = 3; // caminar derecha
    }
    else if (actions->sideAdvance > 0) {
        animationToPlay = 4; // caminar izquierda
    }
    // else mantiene 0 = idle

    playerModel->setAnimation(animationToPlay);

//ZITOS

    if (actions->getAngle() != NULL) {
        //ZITOS
        //OGLobj->cameraDetails->calculateAngleAroundPlayer((*actions->getAngle()) * (6 * gameTime.deltaTime / 100));
    }
    if (actions->getPitch() != NULL) {
        //playerModel->cameraDetails->setPitch(playerModel->cameraDetails->getPitch() + (*actions->getPitch()) * (6 * gameTime.deltaTime / 100));
    }
    if (actions->getZoom() != NULL) {
        playerModel->cameraDetails->setZoom(playerModel->cameraDetails->getZoom() + *actions->getZoom() * (6 * gameTime.deltaTime / 100));
    }
    if (actions->getPlayerZoom() != NULL) {
        playerModel->cameraDetails->calculateZoomPlayer(*actions->getPlayerZoom() * (6 * gameTime.deltaTime / 100));
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
        //int value = lParam;
        // 
        //ZITOS
        //int currMouseX = LOWORD(lParam);
        //int currMouseY = HIWORD(lParam);

        //if (mouseEnabled) {
        //    // Calcula delta solo si el mouse está bloqueado
        //    cDelta.setPosition((float)currMouseX, (float)currMouseY, true);
        //}
        //else {
        //    // Mouse libre, delta cero
        //    cDelta.setPosition((float)currMouseX, (float)currMouseY, false);
        //}
        //ZITOS
        int currMouseX = LOWORD(lParam);
        int currMouseY = HIWORD(lParam);

        if (mouseEnabled) {
            if (ignoreNextDelta) {
                // Ignoramos este movimiento porque proviene de SetCursorPos
                cDelta.setPosition((float)currMouseX, (float)currMouseY, false);
                ignoreNextDelta = false; // limpiar bandera
            }
            else {
                cDelta.setPosition((float)currMouseX, (float)currMouseY, true);
            }
        }
        else {
            cDelta.setPosition((float)currMouseX, (float)currMouseY, false);
        }
        //ZITOS


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

        //ZITOS
        ClipCursor(NULL);   // Libera el cursor al sistema
        ShowCursor(TRUE);   // Lo vuelve a mostrar      
        //PostQuitMessage(0);

        //default:
        //return DefWindowProc(hWnd, message, wParam, lParam);
        ////ZITOS
    } break;
    // 🔹 Liberar el cursor antes de cerrar el juego
    //    ClipCursor(NULL);   // Libera el cursor al sistema
    //    ShowCursor(TRUE);   // Lo vuelve a mostrar
    //    PostQuitMessage(0);
    //    break;

    //default:
    //    return DefWindowProc(hWnd, message, wParam, lParam);

    //    return 0;
    case WM_SIZE: {

            //RECT rect;
            //if (newContext) {
            //    //checar esto pra lo del mouse 
            //    
            //    //esta opcion del switch se ejecuta una sola vez al arrancar y si se
            //   // afecta el tama�o de la misma se dispara de nuevo
            //    int height = HIWORD(lParam),
            //        width = LOWORD(lParam);
            //    if (height == 0)
            //        width = 1;

            //    SCR_HEIGHT = height;
            //    SCR_WIDTH = width;

            //    glViewport(0, 0, width, height);

            //    //RECT rect;

            //    if (GetClientRect(hWnd, &rect)) 
            //        windowSize = glm::vec2(rect.right - rect.left, rect.bottom - rect.top);
            //    else windowSize = glm::vec2(0);
            //}



        RECT rect;
        if (newContext) {
            int height = HIWORD(lParam),
                width = LOWORD(lParam);
            if (height == 0)
                width = 1;

            SCR_HEIGHT = height;
            SCR_WIDTH = width;

            glViewport(0, 0, width, height);

            if (GetClientRect(hWnd, &rect))
                windowSize = glm::vec2(rect.right - rect.left, rect.bottom - rect.top);
            else
                windowSize = glm::vec2(0);
        }

        // Bloquear cursor dentro de la ventana al reescalar
        RECT clipRect = rect;
        POINT ul = { clipRect.left, clipRect.top };
        POINT lr = { clipRect.right, clipRect.bottom };
        MapWindowPoints(hWnd, NULL, &ul, 1);
        MapWindowPoints(hWnd, NULL, &lr, 1);
        clipRect.left = ul.x; clipRect.top = ul.y;
        clipRect.right = lr.x; clipRect.bottom = lr.y;
        ClipCursor(&clipRect);

        // Reposicionar cursor al centro
        POINT center = { (clipRect.left + clipRect.right) / 2, (clipRect.top + clipRect.bottom) / 2 };
        SetCursorPos(center.x, center.y);

        // Evitar salto de cámara
        cDelta.setPosition((float)center.x, (float)center.y, false);
        ignoreNextDelta = true;
          

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
//      if (wParam == KEYB_CAMERA || wParam == KEYB_HMOVEMENT)
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
     //create window


    //ZITOS
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    //ZITOS



    RECT wr = { 0, 0, SCR_WIDTH, SCR_HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    //hWnd = CreateWindow(szWindowClass, szTitle,
    //    WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    //    100, 100,
    //    wr.right - wr.left, //SCR_WIDTH, 
    //    wr.bottom - wr.top, //SCR_HEIGHT,
    //    NULL, NULL, hInstance, NULL);

    //ZITOS
    hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        /*WS_POPUP | WS_VISIBLE,*/
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,  // Con bordes
        0, 0,                               // Posición inicial (arriba izquierda)
        wr.right - wr.left,                 // Ancho ajustado
        wr.bottom - wr.top,                 // Alto ajustado
        NULL, NULL, hInstance, NULL
    );
    int posX = (screenWidth - (wr.right - wr.left)) / 2;
    int posY = (screenHeight - (wr.bottom - wr.top)) / 2;
    SetWindowPos(hWnd, HWND_TOP, posX, posY, wr.right - wr.left, wr.bottom - wr.top, SWP_SHOWWINDOW);
    //ZITOS

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






    return 0;


}
#else
void window_size_callback(GLFWwindow* window, int width, int height){
    if (height == 0)
        width = 1;
    SCR_HEIGHT = height;
    SCR_WIDTH = width;
    glViewport(0, 0, width, height);
    windowSize = glm::vec2(SCR_WIDTH, SCR_HEIGHT);
}

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

//void mouseActions() {
//    double x, y;
//#ifdef _WIN32 
//    POINT p;
//    GetCursorPos(&p);
//    ScreenToClient(hWnd, &p);
//    x = p.x;
//    y = p.y;
//#else
//    glfwGetCursorPos(window, &x, &y);
//#endif
//    glm::vec2 scale = glm::vec2(x, y) / windowSize;
//    OGLobj->getMainModel()->cameraDetails->setPitch(scale.y * 70.0f - 30.f);
//    scale = cDelta.setPosition(x, y, cDelta.getLbtn() || cDelta.getRbtn());
////   scale = cDelta.setPosition(x, y, true);
//    if (scale.x != 0)
//        OGLobj->getMainModel()->cameraDetails->calculateAngleAroundPlayer((scale.x / abs(scale.x)) * -3.0);
//}

//HORIZONTAL SI


void mouseActions() {

    // Si el mouse no está habilitado, salir de la función
    if (!mouseEnabled) {
        return;
    }
    double x, y;
#ifdef _WIN32 
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(hWnd, &p);
    x = p.x;
    y = p.y;
#else
    glfwGetCursorPos(window, &x, &y);
#endif

    // Obtener el centro de la ventana
    int centerX = windowSize.x / 2.0;
    int centerY = windowSize.y / 2.0;

    // Calcular el desplazamiento del mouse desde el centro
    double deltaX = x - centerX;
    double deltaY = y - centerY;



    // Solo procesar si el mouse se ha movido significativamente
    if (abs(deltaX) > 0.1 || abs(deltaY) > 0.1) {
        Scenario* scenario = dynamic_cast<Scenario*>(OGLobj);
        if (!scenario) return;

        Model* player = scenario->getMainModel();

        // Factor de sensibilidad del mouse - puedes ajustar este valor
        float sensitivity = 0.1f;

        // Rotación horizontal (Yaw) - CORREGIDO: sin invertir
        float rotationAmount = -deltaX * sensitivity;
        float newRotationY = player->getNextRotY() + rotationAmount;
        player->setNextRotY(newRotationY);

        // Rotación vertical (Pitch) - CORREGIDO: quitado el signo negativo
        float pitchChange = deltaY * sensitivity; // Sin negativo para corrección
        float newPitch = player->cameraDetails->getPitch() + pitchChange;

        // Limitar el ángulo de pitch para evitar volteretas
        newPitch = glm::clamp(newPitch, -89.0f, 89.0f);
        player->cameraDetails->setPitch(newPitch);

        // Actualizar la cámara
        player->cameraDetails->CamaraUpdate(player->getNextRotY(), player->getNextTranslate());

         //Centrar el cursor nuevamente en la ventana
#ifdef _WIN32 
        POINT centerPoint = { static_cast<LONG>(centerX), static_cast<LONG>(centerY) };
        ClientToScreen(hWnd, &centerPoint);
        SetCursorPos(centerPoint.x, centerPoint.y);
#else
        glfwSetCursorPos(window, centerX, centerY);
#endif
    }
}




//void mouseActions() {
//    if (!mouseEnabled) return;
//
//    double xpos, ypos;
//#ifdef _WIN32
//    POINT p;
//    GetCursorPos(&p);
//    ScreenToClient(hWnd, &p);
//    xpos = p.x;
//    ypos = p.y;
//#else
//    glfwGetCursorPos(window, &xpos, &ypos);
//#endif
//
//    // Primer movimiento
//    if (firstMouse_cam) {
//        lastX_cam = xpos;
//        lastY_cam = ypos;
//        firstMouse_cam = false;
//    }
//
//    double offsetX = xpos - lastX_cam;
//    double offsetY = ypos - lastY_cam;
//
//    lastX_cam = xpos;
//    lastY_cam = ypos;
//
//    float sensitivity = 0.1f;
//    offsetX *= sensitivity;
//    offsetY *= sensitivity;
//
//    Scenario* scenario = dynamic_cast<Scenario*>(OGLobj);
//    if (!scenario) return;
//    Model* player = scenario->getMainModel();
//    if (!player) return;
//
//    // ✅ Movimiento horizontal libre como en el primer código
//    player->setNextRotY(player->getRotY() - offsetX);
//
//    // ✅ Movimiento vertical limitado
//    float newPitch = player->cameraDetails->getPitch() + offsetY;
//    newPitch = glm::clamp(newPitch, -89.0f, 89.0f);
//    player->cameraDetails->setPitch(newPitch);
//
//    // Actualizar cámara
//    player->cameraDetails->CamaraUpdate(player->getRotY(), player->getTranslate());
//}



















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
        glfwSetWindowSizeCallback(window, window_size_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        lockCursor(true);
        renderiza = true;
    }
    flag = !glfwWindowShouldClose(window);
    if (flag)
        glfwPollEvents();
#else
    if (!renderiza){
        renderiza = true;
        lockCursor(true);
    }
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




//ZITOS
int finishProgram(void *ptr){
#ifdef _WIN32
    ShowCursor(TRUE); // Siemmostrar el cursor al salir
    MSG& msg = *(MSG*)ptr;
    return (int)msg.wParam;
#else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Restaurar cursor
    glfwTerminate();
    return 0;
#endif
}
//ZITOS


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

void updateFPS(Texto* fps, int totFrames) {
    WCHAR conv[50] = { 0 };
    swprintf((wchar_t*)conv, 50, L"%d", totFrames);
    wcscat_s((wchar_t*)conv, 50, L" FPS");
    fps->initTexto(conv);
}



//ZITOS
void updateVida(Texto* vidaTexto)
{
    if (!vidaTexto) return;

    Player& player = Player::getInstance();
    float vidaActual = player.getHealth();

    WCHAR conv[100] = { 0 };
    swprintf(conv, 100, L"Vida: %.1f", vidaActual);

    vidaTexto->initTexto(conv);
}

//ZITOS



void lockCursor(bool mouse){
    //ZITOS
#ifdef _WIN32
        if (mouseEnabled) {
            // Bloquea el mouse
            RECT rect;
            GetClientRect(hWnd, &rect);
            POINT ul = { rect.left, rect.top };
            POINT lr = { rect.right, rect.bottom };
            MapWindowPoints(hWnd, NULL, &ul, 1);
            MapWindowPoints(hWnd, NULL, &lr, 1);
            rect.left = ul.x; rect.top = ul.y; rect.right = lr.x; rect.bottom = lr.y;
            ClipCursor(&rect);
            ShowCursor(FALSE);
            firstMouse = mouse; // <-- importante para evitar saltos
        }
        else {
            // Libera el mouse
            ClipCursor(NULL);
            ShowCursor(TRUE);
        }
#else
// Para GLFW
    if (mouseEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }else{
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
#endif
    //ZITOS
}