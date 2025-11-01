#include "RainSystem.h"
#include "Base/shader.h"
#include <glad/glad.h>
#include <random>

Splash::Splash(int maxDrops, Camera* cameraDetails) 
    : Billboard(0, (WCHAR*)"SPLASH", 0, 0, 0, cameraDetails)
{
    vector<Vertex> splash;
    vector<unsigned int> indices;
    vector<Texture> texture;
    getModelAttributes()->reserve(maxDrops);
    splash.push_back(Vertex());
    // Load textures
    bool alpha = true;
    unsigned int splashTexture = TextureFromFile("textures/rainSplash.png", "", false, true, &alpha);
    Texture t;
    t.id = splashTexture;
    texture.push_back(t);
    meshes.push_back(new Mesh(splash, indices, texture, GL_STATIC_DRAW, GL_STATIC_DRAW, GL_POINTS));
    // Load shaders
    this->gpuDemo = new Shader("shaders/rainSplash.vs", "shaders/rainSplash.fs");
}

void Splash::respawn(glm::vec3& pos, int idx) {
    ModelAttributes& s = getModelAttributes()->at(idx);
    s.translate = pos;
    life[idx] = 0.05f;   // half a second
    s.scale = glm::vec3(2.1f);
    s.active = true;
}

void Splash::Draw() {
    glm::mat4 proj = cameraDetails->getProjection();
    glm::mat4 view = cameraDetails->getView();
    // Render splashes
    gpuDemo->use();
    glUniformMatrix4fv(glGetUniformLocation(gpuDemo->ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(gpuDemo->ID, "proj"), 1, GL_FALSE, &proj[0][0]);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, meshes.at(0)->textures.at(0).id);
    glUniform1i(glGetUniformLocation(gpuDemo->ID, "splashTex"), 0);
    Model::Draw(*gpuDemo, 0);
    glDisable(GL_PROGRAM_POINT_SIZE);
    gpuDemo->desuse();
}

RainDrop::RainDrop(int maxDrops, int areaSize, int height, Camera *cameraDetails)
 : Billboard(0, (WCHAR*)"DROPS", 0, 0, 0, cameraDetails)
{
    // Load textures
    bool alpha = true;
    unsigned int dropTexture = TextureFromFile("textures/rainDrop.png", "", false, true, &alpha);
    this->areaSize = areaSize;
    this->height = height;
    this->velocity.reserve(maxDrops);
    vector<Vertex> drops;
    vector<unsigned int> indices;
    vector<Texture> texture;
    getModelAttributes()->reserve(maxDrops);
    drops.push_back(Vertex());
    for (int i = 0; i < maxDrops; i++){
        auto &drop = getModelAttributes()->at(i);
        respawn(drop, i);
    }
    Texture t;
    t.id = dropTexture;
    texture.push_back(t);
    meshes.push_back(new Mesh(drops, indices, texture, GL_STATIC_DRAW, GL_STATIC_DRAW, GL_POINTS));
    // Load shaders
    gpuDemo = new Shader("shaders/rain.vs", "shaders/rain.fs", "shaders/rain.gs");
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
    glm::mat4 proj = cameraDetails->getProjection();
    glm::mat4 view = cameraDetails->getView();
    gpuDemo->use();
    glUniformMatrix4fv(glGetUniformLocation(gpuDemo->ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(gpuDemo->ID, "proj"), 1, GL_FALSE, &proj[0][0]);
    glUniform3fv(glGetUniformLocation(gpuDemo->ID, "cameraPos"), 1, &cameraDetails->getPosition()[0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, meshes.at(0)->textures.at(0).id);
    glUniform1i(glGetUniformLocation(gpuDemo->ID, "dropTex"), 0);
    Model::Draw(*gpuDemo, 0);
    gpuDemo->desuse();
}

RainSystem::RainSystem(int maxDrops, float areaSize, float height, Camera* cameraDetails)
    : splashes(maxDrops, cameraDetails), drops(maxDrops, areaSize, height, cameraDetails)
{
    this->maxDrops = maxDrops;
    this->areaSize = areaSize;
    this->height = height;
    cam = cameraDetails;
    drops.getModelAttributes()->resize(maxDrops);
    splashes.getModelAttributes()->resize(maxDrops);
    splashes.life.resize(maxDrops);
    for (int i = 0; i < maxDrops; i++){
        splashes.getModelAttributes()->at(i).active = false;
    }
}

void RainSystem::update(Terreno *t) {
    // Update drops
    auto& aDrops = *drops.getModelAttributes();
    for (int i = 0; i < aDrops.size(); i++) {
        auto &d = aDrops[i];
        d.translate -= drops.velocity[i];
        float dy = t->Superficie(d.translate.x, d.translate.z);
        if (d.translate.y <= dy) {
            glm::vec3 sp = glm::vec3(d.translate.x, dy, d.translate.z);
            splashes.respawn(sp, i);
            drops.respawn(d, i);
        }
    }
    // Update splashes
    for (int i = 0; i < splashes.getModelAttributes()->size(); i++) {
        auto& s = splashes.getModelAttributes()->at(i);
        if (splashes.life[i] <= 0.00f)
            s.active = false;
        else {
            splashes.life[i] -= gameTime.deltaTime / 1000;
            s.scale.x += gameTime.deltaTime / 1000 * 2.0f;
        }
    }
}

void RainSystem::render() {
    drops.Draw();
    splashes.Draw();
}
