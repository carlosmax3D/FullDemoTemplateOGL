#pragma once
#include "Base/Utilities.h"
#include "Base/Billboard.h"
#include "Base/camera.h"
#include "Base/shader.h"
#include "Terreno.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Splash : public Billboard {
private:
public:
    vector<float> life;   // remaining time
    Splash(int maxDrops, Camera *cameraDetails);
    void respawn(glm::vec3 &pos, int idx = 0);
    void Draw();
};

class RainDrop : public Billboard {
private:
public:
    int areaSize;
    int height;
    vector<glm::vec3> velocity;
    RainDrop(int maxDrops, int areaSize, int height, Camera *cameraDetails);
    void respawn(ModelAttributes& drop, int idx = 0);
    void Draw();
};

class RainSystem {
public:
    RainSystem(int maxDrops, float areaSize, float height, Camera *cameraDetails);

    void update(Terreno *t);
    void render();

private:
    RainDrop drops;
    Splash splashes;
    Camera *cam;

    int maxDrops;
    float areaSize;
    float height;
};
