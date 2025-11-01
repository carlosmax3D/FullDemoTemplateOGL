#pragma once
#include "Base/Utilities.h"
#include "Base/Billboard.h"
#include "Base/camera.h"
#include "Base/shader.h"
#include "Terreno.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Splash : Billboard {
private:
    float life;   // remaining time
public:
    Splash(int maxDrops, Camera *cameraDetails);
    void respawn(glm::vec3 &pos, int idx = 0);
};

class RainDrop : public Billboard {
private:
    vector<glm::vec3> velocity;
    int areaSize;
    int height;
public:
    RainDrop(int maxDrops, int areaSize, int height, Camera *cameraDetails);
    void respawn(Vertex& drop, int idx = 0);
};

class RainSystem {
public:
    RainSystem(int maxDrops, float areaSize, float height, Camera *cameraDetails);

    void update(Terreno *t);
    void render();

private:
    std::vector<RainDrop> drops;
    std::vector<Splash> splashes;
    Camera *cam;

    unsigned int dropVAO, dropVBO;
    unsigned int splashVAO, splashVBO;

    int maxDrops;
    float areaSize;
    float height;

    Shader *dropShader = NULL;
    Shader *splashShader = NULL;
    unsigned int dropTexture;
    unsigned int splashTexture;

    void initGL();
    void respawn(RainDrop& drop);
    void spawnSplash(const glm::vec3& pos, int idx);
};
