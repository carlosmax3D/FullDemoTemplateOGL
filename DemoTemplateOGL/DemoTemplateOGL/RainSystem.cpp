#include "RainSystem.h"
#include "Base/shader.h"
#include <glad/glad.h>
#include <random>
#ifdef __linux__
#define strcpy_s(x,y,z) strcpy(x,z)
#endif
Splash::Splash(WCHAR *textura, int maxDrops, Camera* cameraDetails) 
    : Billboard(textura, 0.5f, 0.5f, 0, 0, 0, cameraDetails)
{
    this->cameraDetails = cameraDetails;
    getModelAttributes()->resize(maxDrops);
    life.resize(maxDrops);
}

void Splash::respawn(glm::vec3& pos, int idx) {
    ModelAttributes& s = getModelAttributes()->at(idx);
    s.translate = pos;
    life[idx] = 0.05f;   // half a second
    s.scale = glm::vec3(2.1f);
    s.active = true;
}

void Splash::Draw() {
    Billboard::Draw();
}

RainDrop::RainDrop(WCHAR* textura, int maxDrops, int areaSize, int height, Camera *cameraDetails)
    : Billboard(textura, 0.2f, 0.2f, 0, 0, 0, cameraDetails)
{
    this->cameraDetails = cameraDetails;
    // Load textures
    this->areaSize = areaSize;
    this->height = height;
    this->velocity.resize(maxDrops);
    getModelAttributes()->resize(maxDrops);
    for (int i = 0; i < maxDrops; i++){
        auto &drop = getModelAttributes()->at(i);
        respawn(drop, i);
    }
}

void RainDrop::respawn(ModelAttributes& drop, int idx){
    static std::default_random_engine eng;
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    float x = static_cast<float>(rand() % (int)areaSize) - areaSize / 2;
    float z = static_cast<float>(rand() % (int)areaSize) - areaSize / 2;
    float y = static_cast<float>(rand() % (int)height);

    drop.translate = glm::vec3(x, y, z);

    // downward velocity with small random wind
    velocity[idx] = glm::vec3(dist(eng) * 2.0f, 0.75f + static_cast<float>(rand() % 100) / 500.0f * (gameTime.deltaTime / 10000), dist(eng) * 2.0f);
}

void RainDrop::Draw() {
    // Render raindrops
    Billboard::Draw();
}

RainSystem::RainSystem(int maxDrops, float areaSize, float height, Camera* cameraDetails) {
    splashes = new Splash((WCHAR*)L"textures/rainSplash.png", maxDrops, cameraDetails);
    drops = new RainDrop((WCHAR*)L"textures/rainDrop.png", maxDrops, areaSize, height, cameraDetails);
    this->maxDrops = maxDrops;
    this->areaSize = areaSize;
    this->height = height;
    cam = cameraDetails;
    drops->getModelAttributes()->resize(maxDrops);
    splashes->getModelAttributes()->resize(maxDrops);
    splashes->life.resize(maxDrops);
    for (int i = 0; i < maxDrops; i++){
        splashes->getModelAttributes()->at(i).active = false;
    }
}

void RainSystem::update(Terreno *t) {
    auto& aDrops = *drops->getModelAttributes();
    auto& aSplashes = *splashes->getModelAttributes();
    for (int i = 0; i < aDrops.size(); i++) {
        // Update drops
        auto &d = aDrops[i];
        d.translate -= drops->velocity[i];
        float dy = t->Superficie(d.translate.x, d.translate.z);
        if (d.translate.y <= dy) {
            glm::vec3 sp = glm::vec3(d.translate.x, dy, d.translate.z);
            splashes->respawn(sp, i);
            drops->respawn(d, i);
        }
        // Update splashes
        auto &s = aSplashes[i];
        if (splashes->life[i] <= 0.00f)
            s.active = false;
        else {
            splashes->life[i] -= gameTime.deltaTime / 1000;
            s.scale.x += gameTime.deltaTime / 1000 * 2.0f;
        }
    }
}

void RainSystem::render() {
    drops->Draw();
    splashes->Draw();
}

RainSystem::~RainSystem() {
    if (drops != NULL) delete drops;
    if (splashes != NULL) delete splashes;
    drops = NULL;
    splashes = NULL;
}