#ifndef CAMERA_H
#define CAMERA_H

#include "Utilities.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
private:
    static Camera* cameraInstance;
    // camera Attributes
    float DistanceFromPlayer = 15;
    float angleAroundPlayer = 0;
    glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Front = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Up = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Right = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 WorldUp = glm::vec3(0.0f, 0.0f, 0.0f);
    // euler Angles
    float Yaw = 0;
    float Pitch = 20;
    // camera options
    float MovementSpeed = 0;
    float MouseSensitivity = 0;
    float Zoom = 0;
    glm::mat4 projection;
    // Obtenemos la vista
    glm::mat4 view;
    float characterHeight = 0;
    bool firstPerson = false;

    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors() {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
        //        if (Up.y > 0.0f)
        //            Up.y *= -1.0f;
    }
    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
//    Camera(){}
public:
    static Camera* getInstance() {
        if (!cameraInstance) {
            cameraInstance = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), YAW, PITCH);
        }
        return cameraInstance;
    }    
    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    void GetViewMatrix(glm::mat4& viewMatrix) {
        viewMatrix = glm::mat4(1.0f); // constructs an identity matrix
        viewMatrix = glm::rotate(viewMatrix, glm::radians(Pitch), glm::vec3(1, 0, 0));
        viewMatrix = glm::rotate(viewMatrix, glm::radians(Yaw), glm::vec3(0, 1, 0));
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-this->Position.x, -this->Position.y, -this->Position.z));
        //glm::lookAt(Position, Position + Front, Up);
    }

    void calculateCameraPosition(float pRotY, glm::vec3 *pTrans, float dHorizontal, float dVertical) {
        if (firstPerson) {
            float theta = pRotY + Camera::angleAroundPlayer; //Model::getRotationAngle() +
            float offsetX = dHorizontal * glm::sin(glm::radians(theta));
            float offsetZ = dHorizontal * glm::cos(glm::radians(theta));
            getPosition().x = pTrans->x - offsetX;
            getPosition().z = pTrans->z - offsetZ;
            getPosition().y = pTrans->y + dVertical;
        } else {
            setPosition(*pTrans);
            getPosition().y += characterHeight;
        }
    }

    glm::mat4 CamaraUpdate(float pRotY, glm::vec3 *pTrans) {
        calculateCameraPosition(pRotY, pTrans, calculateHorizontalDistance(), calculateVerticalDistance());
        setYaw(180 - pRotY - angleAroundPlayer);
        // Obtenemos la proyeccion en base a la ventana
        projection = glm::perspective(glm::radians(getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // Obtenemos la vista
        GetViewMatrix(view);
        return glm::lookAt(Position,
            Position + Front,
            glm::vec3(0, 1, 0));
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    void movePosition(glm::vec3& avance, float vel) {
        avance.x = Position.x + Front.x * vel;
        avance.y = Position.y + Front.y * vel;
        avance.z = Position.z + Front.z * vel;
        Position = avance;
    }
    void CamaraAvanza() {
//        setPosition(nextPosition);
        /*gluLookAt(posc.X, posc.Y, posc.Z,
            posc.X + dirc.X, posc.Y, posc.Z + dirc.Z,
            0, 1, 0);*/
        glm::vec3 tmp(Front.x, 0, Front.z);
        glm::lookAt(Position, Position + tmp, Up);
    }
    void CamaraAvanza(float vel) {
        glm::vec3 newPos;
        movePosition(newPos, vel);        
        setPosition(newPos);
        /*gluLookAt(posc.X, posc.Y, posc.Z,
            posc.X + dirc.X, posc.Y, posc.Z + dirc.Z,
            0, 1, 0);*/
        glm::vec3 tmp(Front.x,0,Front.z);
        glm::lookAt(Position, Position + tmp, Up);
    }

    void CamaraGiraY(float grados) {
        glm::vec3 v = Transforma(Front, grados, Ejes::EjeY);
        setFront(v);
    }
    void CamaraGiraX(float grados) {
        glm::vec3 v = Transforma(Front, grados, Ejes::EjeX);
        setFront(v);
    }

    //esto es de Graficas Computacionales no ocupan aprenderselo para la revision
    glm::vec3 Transforma(glm::vec3 &v, float grados2, int eje = 0)
    {
        float Matriz[4][4];
        double grados = grados2 * 3.141516 / 180.0;

        Identidad(Matriz);

        if (eje == Ejes::EjeX)
        {
            Matriz[1][1] = cos(grados);
            Matriz[2][2] = Matriz[1][1];
            Matriz[1][2] = -sin(grados);
            Matriz[2][1] = sin(grados);
        }

        if (eje == Ejes::EjeY)
        {
            Matriz[0][0] = cos(grados);
            Matriz[2][2] = Matriz[0][0];
            Matriz[0][2] = -sin(grados);
            Matriz[2][0] = sin(grados);
        }

        if (eje == Ejes::EjeZ)
        {
            Matriz[0][0] = cos(grados);
            Matriz[1][1] = Matriz[0][0];
            Matriz[0][1] = -sin(grados);
            Matriz[1][0] = sin(grados);
        }

        glm::vec3 aux;

        aux.x = v.x * Matriz[0][0] + v.y * Matriz[0][1] + v.z * Matriz[0][2];
        aux.y = v.x * Matriz[1][0] + v.y * Matriz[1][1] + v.z * Matriz[1][2];
        aux.z = v.x * Matriz[2][0] + v.y * Matriz[2][1] + v.z * Matriz[2][2];

        v = aux;
        return v;
    }

    //esto es de Graficas Computacionales no ocupan aprenderselo para la revision
    void Identidad(float Mat[4][4])
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                Mat[i][j] = 0;
                if (i == j)
                    Mat[i][j] = 1;
            }
        }
    }
    void setFirstPerson(bool fps) {
        firstPerson = fps;
        angleAroundPlayer = 0;
    }
    bool getFirstPerson() {
        return firstPerson;
    }
    float getAngleAroundPlayer() { return angleAroundPlayer; }
    void calculateAngleAroundPlayer(double val) {
        angleAroundPlayer -= val * 0.5;
        if ((!firstPerson) && angleAroundPlayer > 90)
            angleAroundPlayer = 90;
        if ((!firstPerson) && angleAroundPlayer < -90)
            angleAroundPlayer = -90;
    }
    void calculateZoomPlayer(float val) {
        DistanceFromPlayer -= val * 0.5f;
    }
    float calculateHorizontalDistance() {
        return DistanceFromPlayer * glm::cos(glm::radians(Pitch));
    }
    float calculateVerticalDistance() {
        return DistanceFromPlayer * glm::sin(glm::radians(Pitch));
    }
    glm::vec3& getPosition() { return Position; };
    void setPosition(glm::vec3& Position) {
        this->Position = Position;
        //        cameraDetails.Position = &this->Position;
    }
    glm::vec3 &getFront() { return Front; };
    void setFront(glm::vec3 &Front) { 
        this->Front = Front; 
//        cameraDetails.Front = &this->Front;
    }
    glm::vec3 &getUp() { return Up; };
    void setUp(glm::vec3 &Up) { 
        this->Up = Up; 
//        cameraDetails.Up = &this->Up;
    }
    glm::vec3 &getRight() { return Right; };
    void setRight(glm::vec3 &Right) { 
        this->Right = Right; 
//        cameraDetails.Right = &this->Right; 
    }
    glm::vec3 &getWorldUp() { return WorldUp; };
    void setWorldUp(glm::vec3 &WorldUp) { 
        this->WorldUp = WorldUp; 
//        cameraDetails.WorldUp = &this->WorldUp; 
    }
    // euler Angles
    float getYaw() { return Yaw; };
    void setYaw(float Yaw) { 
        this->Yaw = Yaw;
//        cameraDetails.Yaw = &this->Yaw; 
    }
    float getPitch() { return Pitch; };
    void setPitch(float Pitch) { 
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        else if (Pitch < -89.0f)
            Pitch = -89.0f;
        else this->Pitch = Pitch; //cameraDetails.Pitch = &this->Pitch; 
    }
    // camera options
    float getCharacterHeight() { return characterHeight; }
    void setCharacterHeight(float characterHeight) { this->characterHeight = characterHeight; }
    float getMovementSpeed() { return MovementSpeed; };
    void setMovementSpeed(float MovementSpeed) { this->MovementSpeed = MovementSpeed; //cameraDetails.MovementSpeed = &this->MovementSpeed; 
    }
    float getMouseSensitivity() { return MouseSensitivity; };
    void setMouseSensitivity(float MouseSensitivity) { this->MouseSensitivity = MouseSensitivity; //cameraDetails.MouseSensitivity = &this->MouseSensitivity; 
    };
    float getZoom() { return Zoom; };
    void setZoom(float Zoom) { 
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        else if (Zoom > 45.0f)
            Zoom = 45.0f;
        else this->Zoom = Zoom; //cameraDetails.Zoom = &this->Zoom; 
    }
    glm::mat4 getProjection() {
        return projection;
    }
    glm::mat4 getView() {
        return view;
    }
};
#endif