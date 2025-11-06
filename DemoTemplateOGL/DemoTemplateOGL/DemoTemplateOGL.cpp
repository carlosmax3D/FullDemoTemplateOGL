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
#include "Musica.h" //para manejar musica

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
void window_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
#endif

#define Timer1 100
bool renderiza;                     // Variable para controlar el render
bool checkInput(GameActions* actions, Scene* scene);
void mouseActions();
int isProgramRunning(void* ptr);
void swapGLBuffers();
int finishProgram(void* ptr);
int gamePadEvents(GameActions* actions);
void updatePosCords(Texto* coordenadas);
void updateFPS(Texto* fps, int totFrames);
int startGameEngine(void* ptrMsg);

// Propiedades de la ventana
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
glm::vec2 windowSize;
bool showHitbox = true;
bool showStats = true;
bool newContext = false; // Bandera para identificar si OpenGL 2.0 > esta activa
struct GameTime gameTime;
Camera* Camera::cameraInstance = NULL;

// Objecto de escena y render
Scene* OGLobj;

// Sistema de audio
Musica musica;

// Variables para control de animaciones
double lastMoveTime = 0.0;
bool isMoving = false;

// Definiciones de índices de animación
#define ANIM_IDLE_STOP 0
#define ANIM_WALK_FRONT 1
#define ANIM_WALK_BACK 2
#define ANIM_RUN_FRONT 3
#define ANIM_RUN_BACK 4
#define ANIM_IDLE_MOVE 5

// Función para cambiar animaciones
bool setAnimation(Model* model, int animationId) {
    if (model->getAnimator().size() > animationId) {
        model->setAnimation(animationId);
        return true;
    }
    else {
        WARNING("Animation index " + std::to_string(animationId) + " not found!", "ANIMATION");
        return false;
    }
}

// Función para cargar todas las animaciones del jugador
void loadPlayerAnimations(Model* playerModel) {
    try {
        // Cargar animación Idle Stop (ya está cargada con el modelo base)
        std::vector<Animation> idleStopAnim = Animation::loadAllAnimations("models/Jugador/Idle Stop.fbx",
            playerModel->GetBoneInfoMap(), playerModel->getBonesInfo(), playerModel->GetBoneCount());
        if (!idleStopAnim.empty()) {
            playerModel->setAnimator(Animator(idleStopAnim[0]));
        }

        // Cargar Walk Front
        std::vector<Animation> walkFrontAnim = Animation::loadAllAnimations("models/Jugador/Walk Front.fbx",
            playerModel->GetBoneInfoMap(), playerModel->getBonesInfo(), playerModel->GetBoneCount());
        if (!walkFrontAnim.empty()) {
            playerModel->setAnimator(Animator(walkFrontAnim[0]));
        }

        // Cargar Walk Back
        std::vector<Animation> walkBackAnim = Animation::loadAllAnimations("models/Jugador/Walk Back.fbx",
            playerModel->GetBoneInfoMap(), playerModel->getBonesInfo(), playerModel->GetBoneCount());
        if (!walkBackAnim.empty()) {
            playerModel->setAnimator(Animator(walkBackAnim[0]));
        }

        // Cargar Run Front
        std::vector<Animation> runFrontAnim = Animation::loadAllAnimations("models/Jugador/Run Front.fbx",
            playerModel->GetBoneInfoMap(), playerModel->getBonesInfo(), playerModel->GetBoneCount());
        if (!runFrontAnim.empty()) {
            playerModel->setAnimator(Animator(runFrontAnim[0]));
        }

        // Cargar Run Back
        std::vector<Animation> runBackAnim = Animation::loadAllAnimations("models/Jugador/Run Back.fbx",
            playerModel->GetBoneInfoMap(), playerModel->getBonesInfo(), playerModel->GetBoneCount());
        if (!runBackAnim.empty()) {
            playerModel->setAnimator(Animator(runBackAnim[0]));
        }

        // Cargar Idle Move
        std::vector<Animation> idleMoveAnim = Animation::loadAllAnimations("models/Jugador/Idle Move.fbx",
            playerModel->GetBoneInfoMap(), playerModel->getBonesInfo(), playerModel->GetBoneCount());
        if (!idleMoveAnim.empty()) {
            playerModel->setAnimator(Animator(idleMoveAnim[0]));
        }

        // Iniciar con animación Idle Stop
        playerModel->setAnimation(ANIM_IDLE_STOP);

    }
    catch (const std::exception& e) {
        ERRORL("Could not load animations: " + std::string(e.what()), "ANIMACION_JUGADOR");
    }
    catch (...) {
        ERRORL("Unknown error loading animations!", "ANIMACION_JUGADOR");
    }
}

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
    void* ptrMsg = (void*)&msg;
#else
int main(int argc, char** argv) {
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DemoTemplateOGL", NULL, NULL);
    windowSize = glm::vec2(SCR_WIDTH, SCR_HEIGHT);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        ERRORL("No opengl load", "OPENGL");
        glfwTerminate();
        return -1;
    }
    void* ptrMsg = NULL;
#endif
    return startGameEngine(ptrMsg);
}

int startGameEngine(void* ptrMsg) {
    // INICIALIZAR AUDIO
    if (!musica.iniciar()) {
        ERRORL("No se pudo inicializar el sistema de audio", "AUDIO");
    }
    if (!musica.cargarListaMusica("Musica")) {
        ERRORL("No se encontraron archivos de música", "AUDIO");
    }
    else {
        musica.reproducirMusicaAleatoria();
    }

    // Main character with it's camera
    glm::vec3 translate, scale, v(0, 0, -1);
    translate = glm::vec3(5.0f, 10.0f, -5.0f);

    // Cargar el modelo base del jugador
    Camera* camera = Camera::getInstance();
    Model* model = new Model("models/Jugador/Idle Stop.fbx", translate, camera);
    model->setTranslate(&translate);

    // INICIALIZAR ROTACIÓN A 0 grados en todos los ejes
    model->setRotY(0.0f);
    model->setNextRotY(0.0f);
    model->setRotX(0.0f);
    model->setNextRotX(0.0f);
    model->setRotZ(0.0f);
    model->setNextRotZ(0.0f);

    // También inicializar la cámara a 0 grados (TERCERA PERSONA por defecto)
    camera->setPitch(0.0f);
    camera->calculateAngleAroundPlayer(0.0f);
    camera->setFirstPerson(false); // Iniciar en tercera persona

    // Aplicar escala visual
    glm::vec3 desiredScale(0.05f, 0.05f, 0.05f);
    model->setScale(&desiredScale);

    // Construir KDtree normal
    model->buildKDtree();

    // ESCALAR MANUALMENTE LA COLISIÓN
    model->AABBsize.m_halfWidth *= 0.03f;
    model->AABBsize.m_halfHeight *= 0.22f;
    model->AABBsize.m_halfDepth *= 0.020f;

    // TRASLADAR LA COLISIÓN
    glm::vec3 collisionOffset(0.0f, -0.5f, 35.7f);
    model->AABBsize.m_center.x += collisionOffset.x;
    model->AABBsize.m_center.y += collisionOffset.y;
    model->AABBsize.m_center.z += collisionOffset.z;

    // Reconstruir el collider
    model->buildCollider(
        model->AABBsize.m_center.x,
        model->AABBsize.m_center.y,
        model->AABBsize.m_center.z,
        model->AABBsize.m_halfWidth,
        model->AABBsize.m_halfHeight,
        model->AABBsize.m_halfDepth
    );

    camera->setFront(v);
    camera->setCharacterHeight(4.0);
    model->setTranslate(&translate);

    // Cargar todas las animaciones del jugador
    loadPlayerAnimations(model);
    Texto *fps = NULL;
    Texto *coordenadas = NULL;
    try{

        OGLobj = new Scenario(model);
        translate = glm::vec3(5.0f, OGLobj->getTerreno()->Superficie(5.0, -5.0), -5.0f);
        model->setTranslate(&translate);
        model->setNextTranslate(&translate);
        renderiza = false;

        // Inicializar timer de movimiento
        lastMoveTime = get_nanos() / 1000000.0;

        // === CONFIGURACIÓN DEL DRONE (UNA SOLA VEZ) ===
        Scenario* scenario = dynamic_cast<Scenario*>(OGLobj);
        if (scenario != nullptr) {
            Model* drone = new Model("models/Drone/Drone.obj", camera);
            glm::vec3 dronePos(20.0f, OGLobj->getTerreno()->Superficie(20.0f, 20.0f) + 2.0f, 20.0f);
            glm::vec3 droneScale(2.0f, 2.0f, 2.0f);

            drone->setTranslate(&dronePos);
            drone->setNextTranslate(&dronePos);
            drone->setScale(&droneScale);
            drone->setRotY(180);
            drone->name = "Drone";
            drone->buildKDtree();

            // IMPORTANTE: Hacer que el drone NO tenga colisiones con el jugador
            drone->ignoreAABB = true; // Desactivar colisiones del drone

            // Añadir a la escena
            scenario->getLoadedModels()->emplace_back(drone);
            scenario->setDrone(drone);

            INFO("Drone cargado correctamente (sin colisiones).", "DRONE_INIT");
        }

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
        gameTime.lastTick = get_nanos() / 1000000.0;
        int totFrames = 0;
        double deltasCount = 0;

        std::cout << "\n=== CONTROLES ===" << std::endl;
        std::cout << "JUGADOR (Tercera persona por defecto):" << std::endl;
        std::cout << "  W/S: Adelante/Atrás" << std::endl;
        std::cout << "  A/D: Rotar" << std::endl;
        std::cout << "  Shift: Correr" << std::endl;
        std::cout << "  P: Cambiar a primera persona" << std::endl;
        std::cout << "  Mouse: Control de cámara" << std::endl;
        std::cout << "  E: Entrar al drone (cerca)" << std::endl;
        std::cout << "\nDRONE (Primera persona obligatoria):" << std::endl;
        std::cout << "  W/S: Adelante/Atrás" << std::endl;
        std::cout << "  A/D: Strafe lateral" << std::endl;
        std::cout << "  V: Subir" << std::endl;
        std::cout << "  F: Bajar" << std::endl;
        std::cout << "  Flechas ←/→: Rotar cámara 360°" << std::endl;
        std::cout << "  Flechas ↑/↓: Pitch de cámara" << std::endl;
        std::cout << "  E: Salir del drone (solo en el suelo)" << std::endl;
        std::cout << "==================\n" << std::endl;

        while (isProgramRunning(ptrMsg)) {
            deltasCount += gameTime.deltaTime;
            totFrames++;

            if (musica.necesitaSiguienteCancion()) {
                musica.siguienteCancion();
            }

            if (deltasCount >= 1000.0f) {
                updateFPS(fps, totFrames);
                deltasCount -= 1000.0f;
                totFrames = 1;
            }
            updatePosCords(coordenadas);

            // === DETECCIÓN DE PROXIMIDAD AL DRONE ===
            bool nearDrone = false;
            Scenario* scenario = dynamic_cast<Scenario*>(OGLobj);
            if (scenario != nullptr) {
                Model* drone = scenario->getDrone();
                if (drone != nullptr && !scenario->getIsDroneActive()) {
                    glm::vec3 playerPos = *scenario->getMainModel()->getTranslate();
                    glm::vec3 dronePos = *drone->getTranslate();
                    float distanceToDrone = glm::distance(playerPos, dronePos);

                    if (distanceToDrone < 5.0f) {
                        nearDrone = true;
                    }
                }
            }

            // === LÓGICA PARA ENTRAR AL DRONE ===
            if (nearDrone && KEYS[KEYB_DRONE]) {
                if (scenario != nullptr && !scenario->getIsDroneActive()) {
                    scenario->enterDrone(scenario->getMainModel());
                    std::cout << ">>> ENTRANDO AL DRONE (Primera persona)" << std::endl;
                }
                KEYS[KEYB_DRONE] = false;
            }

            // === LÓGICA PARA SALIR DEL DRONE ===
            if (scenario != nullptr && scenario->getIsDroneActive() && KEYS[KEYB_DRONE]) {
                scenario->exitDrone();
                // El mensaje se muestra dentro de exitDrone()
                KEYS[KEYB_DRONE] = false;
            }

            GameActions actions;

            // render
            bool checkCollition = checkInput(&actions, OGLobj);
            int cambio = OGLobj->update();
            Scene* escena = OGLobj->Render();

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

    // === SISTEMA DRONE ===
    Scenario* scenario = dynamic_cast<Scenario*>(scene);
    Model* currentModel = nullptr;
    bool isDrone = false;

    if (scenario != nullptr) {
        currentModel = scenario->getActiveModel();
        isDrone = scenario->getIsDroneActive();
    }
    else {
        currentModel = scene->getMainModel();
    }

    // ========================================
    // === CONTROLES EXCLUSIVOS DEL DRONE ===
    // ========================================
    if (isDrone) {
        // NO HAY ANIMACIONES PARA EL DRONE

        // Velocidad independiente del drone (más rápido que el jugador)
        float droneSpeed = 0.15f; // Velocidad base del drone
        float deltaTimeFactor = gameTime.deltaTime / 16.0f;

        glm::vec3 pos = *currentModel->getNextTranslate();

        // === MOVIMIENTO FRONTAL/TRASERO (W/S) ===
        if (actions->advance != 0) {
            float moveSpeed = droneSpeed * deltaTimeFactor * actions->advance;
            pos.x += moveSpeed * glm::sin(glm::radians(currentModel->getRotY()));
            pos.z += moveSpeed * glm::cos(glm::radians(currentModel->getRotY()));
        }

        // === MOVIMIENTO LATERAL - STRAFE (A/D) ===
        if (actions->sideAdvance != 0) {
            float strafeSpeed = droneSpeed * deltaTimeFactor * actions->sideAdvance;
            pos.x += strafeSpeed * glm::cos(glm::radians(currentModel->getRotY()));
            pos.z += strafeSpeed * -glm::sin(glm::radians(currentModel->getRotY()));
        }

        // === MOVIMIENTO VERTICAL (V subir / F bajar) ===
        if (KEYS[input.V] || KEYS[input.F]) {
            float verticalSpeed = droneSpeed * deltaTimeFactor;

            if (KEYS[input.V]) {
                pos.y += verticalSpeed; // Elevarse
            }
            if (KEYS[input.F]) {
                // Descender controlado
                float terrainHeight = scenario->getTerreno()->Superficie(pos.x, pos.z);
                if (pos.y - verticalSpeed > terrainHeight + 1.0f) {
                    pos.y -= verticalSpeed;
                }
                else {
                    pos.y = terrainHeight + 1.0f; // Altura mínima
                }
            }
        }

        // Asegurar que el drone no se hunda en el terreno
        float terrainHeight = scenario->getTerreno()->Superficie(pos.x, pos.z);
        if (pos.y < terrainHeight + 1.0f) {
            pos.y = terrainHeight + 1.0f;
        }

        currentModel->setNextTranslate(&pos);

        // === CONTROL DE CÁMARA (Flechas) ===
        Camera* camera = currentModel->cameraDetails;

        // ROTACIÓN HORIZONTAL 360° (FLECHAS IZQUIERDA/DERECHA)
        if (KEYS[input.Left]) {
            float angleIncrement = 2.5f * deltaTimeFactor;
            camera->calculateAngleAroundPlayer(-angleIncrement);
        }
        if (KEYS[input.Right]) {
            float angleIncrement = 2.5f * deltaTimeFactor;
            camera->calculateAngleAroundPlayer(angleIncrement);
        }

        // PITCH DE CÁMARA (FLECHAS ARRIBA/ABAJO)
        if (KEYS[input.Up]) {
            float newPitch = camera->getPitch() + (1.5f * deltaTimeFactor);
            if (newPitch > 89.0f) newPitch = 89.0f;
            camera->setPitch(newPitch);
        }
        if (KEYS[input.Down]) {
            float newPitch = camera->getPitch() - (1.5f * deltaTimeFactor);
            if (newPitch < -89.0f) newPitch = -89.0f;
            camera->setPitch(newPitch);
        }

        // Confirmar movimiento del drone
        currentModel->setTranslate(&pos);

        // NO PERMITIR cambio de cámara con P en drone
        // El drone SIEMPRE está en primera persona

    }
    // =========================================
    // === CONTROLES EXCLUSIVOS DEL JUGADOR ===
    // =========================================
    else {
        // === CONTROL DE ANIMACIONES ===
        isMoving = (actions->advance != 0 || actions->sideAdvance != 0);

        if (isMoving) {
            lastMoveTime = gameTime.lastTick;
        }

        bool hasForwardMovement = (actions->advance > 0);
        bool hasBackwardMovement = (actions->advance < 0);

        if (hasForwardMovement) {
            if (KEYS[KEYB_SPRINT]) {
                changeAnimation = setAnimation(currentModel, ANIM_RUN_FRONT);
            }
            else {
                changeAnimation = setAnimation(currentModel, ANIM_WALK_FRONT);
            }
        }
        else if (hasBackwardMovement) {
            if (KEYS[KEYB_SPRINT]) {
                changeAnimation = setAnimation(currentModel, ANIM_RUN_BACK);
            }
            else {
                changeAnimation = setAnimation(currentModel, ANIM_WALK_BACK);
            }
        }
        else {
            if (gameTime.lastTick - lastMoveTime > 30000.0) {
                changeAnimation = setAnimation(currentModel, ANIM_IDLE_MOVE);
            }
            else {
                changeAnimation = setAnimation(currentModel, ANIM_IDLE_STOP);
            }
        }

        // Velocidad independiente del jugador (más lento que el drone)
        float playerSpeed = 0.30f; // Velocidad base del jugador (más lenta que drone)
        float deltaTimeFactor = gameTime.deltaTime / 16.0f;

        // === ROTACIÓN DEL JUGADOR (A/D) ===
        if (actions->sideAdvance != 0) {
            float rotationSpeed = 2.0f * deltaTimeFactor;

            // Multiplicador de velocidad si está corriendo
            if (KEYS[KEYB_SPRINT]) {
                rotationSpeed *= 1.5f;
            }

            currentModel->setNextRotY(currentModel->getNextRotY() + (rotationSpeed * actions->sideAdvance));
            currentModel->setRotY(currentModel->getNextRotY());
        }

        // === MOVIMIENTO FRONTAL/TRASERO (W/S) ===
        if (actions->advance != 0) {
            glm::vec3 pos = *currentModel->getNextTranslate();
            float moveSpeed = playerSpeed * deltaTimeFactor;

            // Multiplicador de velocidad si está corriendo
            if (KEYS[KEYB_SPRINT]) {
                moveSpeed *= 2.0f;
            }

            pos.x += actions->advance * moveSpeed * glm::sin(glm::radians(currentModel->getRotY()));
            pos.z += actions->advance * moveSpeed * glm::cos(glm::radians(currentModel->getRotY()));
            currentModel->setNextTranslate(&pos);
        }

        // === CONTROL DE CÁMARA CON MOUSE (solo jugador) ===
        // El mouse ya se procesa en mouseActions()

        // PERMITIR cambio entre primera y tercera persona (P)
        if (actions->firstPerson) {
            currentModel->cameraDetails->setFirstPerson(!currentModel->cameraDetails->getFirstPerson());
        }
    }

    // =======================================
    // === CONTROLES GENERALES (UI) ===
    // =======================================
    if (actions->displayHitboxStats) {
        showHitbox = !showHitbox;
        showStats = !showStats;
    }

    // === ZOOM (ambos pueden hacer zoom) ===
    if (actions->getZoom() != NULL) {
        currentModel->cameraDetails->setZoom(currentModel->cameraDetails->getZoom() + *actions->getZoom() * 0.1f);
    }
    if (actions->getPlayerZoom() != NULL) {
        currentModel->cameraDetails->calculateZoomPlayer(*actions->getPlayerZoom() * 0.1f);
    }

    return true;
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
                if (height == 0)
                    width = 1;
                SCR_HEIGHT = height;
                SCR_WIDTH = width;
                glViewport(0, 0, width, height);
                RECT rect;
                if (GetClientRect(hWnd, &rect)) 
                    windowSize = glm::vec2(rect.right - rect.left, rect.bottom - rect.top);
                else windowSize = glm::vec2(0);
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

void mouseActions() {
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

    Scenario* scenario = dynamic_cast<Scenario*>(OGLobj);
    bool isDrone = scenario && scenario->getIsDroneActive();

    // === CONTROL DE MOUSE SOLO PARA JUGADOR ===
    if (!isDrone) {
        glm::vec2 scale = cDelta.setPosition(x, y, true);

        // Control vertical de cámara (pitch)
        if (scale.y != 0) {
            Camera* camera = OGLobj->getMainModel()->cameraDetails;
            float newPitch = camera->getPitch() + (scale.y * 0.5f);
            camera->setPitch(newPitch);
        }

        // Control horizontal de cámara (yaw) - solo con botón izquierdo presionado
        if (cDelta.getLbtn() && scale.x != 0) {
            Camera* camera = OGLobj->getMainModel()->cameraDetails;
            camera->calculateAngleAroundPlayer(scale.x * -0.5f);
        }
    }
    else {
        // === EN DRONE: EL MOUSE NO HACE NADA ===
        // Solo las flechas controlan la cámara del drone
        // Resetear delta para que no se acumule
        cDelta.setPosition(x, y, false);
    }
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
        glfwSetWindowSizeCallback(window, window_size_callback);
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
