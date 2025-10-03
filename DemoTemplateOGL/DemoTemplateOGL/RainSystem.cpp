#include "RainSystem.h"
#include "Base/shader.h"
#include <glad/glad.h>
#include <random>

RainSystem::RainSystem(int maxDrops, float areaSize, float height, Camera *cameraDetails)
    : maxDrops(maxDrops), areaSize(areaSize), height(height) {
    cam = cameraDetails;
    drops.resize(maxDrops);
    splashes.resize(maxDrops);
    for (int i = 0; i < maxDrops; i++){
        auto& d = drops[i];
        respawn(d);
        splashes[i].actived = false;
    }

    initGL();
}

void RainSystem::respawn(RainDrop& drop) {
    static std::default_random_engine eng;
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    float x = static_cast<float>(rand() % (int)areaSize) - areaSize / 2;
    float z = static_cast<float>(rand() % (int)areaSize) - areaSize / 2;
    float y = static_cast<float>(rand() % (int)height);

    drop.position = glm::vec3(x, y, z);

    // downward velocity with small random wind
    drop.velocity = glm::vec3(dist(eng) * 2.0f, 0.75f + static_cast<float>(rand() % 100) / 500.0f * (gameTime.deltaTime / 10000), dist(eng) * 2.0f);
}

void RainSystem::spawnSplash(const glm::vec3& pos, int idx) {
    Splash &s = splashes[idx];
    s.position = pos;
    s.life = 0.05f;   // half a second
    s.size = 2.1f;
    s.actived = true;
}

void RainSystem::initGL() {
    // Drops VBO
    glGenVertexArrays(1, &dropVAO);
    glGenBuffers(1, &dropVBO);
    glBindVertexArray(dropVAO);
    glBindBuffer(GL_ARRAY_BUFFER, dropVBO);
    glBufferData(GL_ARRAY_BUFFER, drops.size() * sizeof(RainDrop), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RainDrop), (void*)0);

    glEnableVertexAttribArray(1); // vel
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RainDrop), (void*)offsetof(RainDrop, velocity));
    glBindVertexArray(0);

    // Splashes VBO
    glGenVertexArrays(1, &splashVAO);
    glGenBuffers(1, &splashVBO);
    glBindVertexArray(splashVAO);
    glBindBuffer(GL_ARRAY_BUFFER, splashVBO);
    glBufferData(GL_ARRAY_BUFFER, maxDrops * sizeof(Splash), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Splash), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Splash), (void*)offsetof(Splash, size));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Splash), (void*)offsetof(Splash, actived));
    glBindVertexArray(0);

    // Load shaders
    dropShader = new Shader("shaders/rain.vs", "shaders/rain.fs", "shaders/rain.gs");
    splashShader = new Shader("shaders/rainSplash.vs", "shaders/rainSplash.fs");

    // Load textures
    bool alpha = true;
    dropTexture = TextureFromFile("textures/rainDrop.png", "", false, true, &alpha);
    splashTexture = TextureFromFile("textures/rainSplash.png", "", false, true, &alpha);
}

void RainSystem::update(Terreno *t) {
    // Update drops
    for (int i = 0; i < drops.size(); i++) {
        auto &d = drops[i];
        d.position -= d.velocity;
        float dy = t->Superficie(d.position.x, d.position.z);
        if (d.position.y <= dy) {
            spawnSplash(glm::vec3(d.position.x, dy, d.position.z), i);
            respawn(d);
        }
    }
    // Update splashes
    for (auto& s : splashes) {
        if (s.life <= 0.00f)
            s.actived = false;
        else {
            s.life -= gameTime.deltaTime / 1000;
            s.size += gameTime.deltaTime / 1000 * 2.0f;
        }
    }
    // Upload drops to GPU
    glBindBuffer(GL_ARRAY_BUFFER, dropVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, drops.size() * sizeof(RainDrop), drops.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Upload splashes to GPU
    if (!splashes.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, splashVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, splashes.size() * sizeof(Splash), splashes.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void RainSystem::render() {
    // Render raindrops
	glm::mat4 proj = cam->getProjection();
	glm::mat4 view = cam->getView();
    glUseProgram(dropShader->ID);
    glUniformMatrix4fv(glGetUniformLocation(dropShader->ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(dropShader->ID, "proj"), 1, GL_FALSE, &proj[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dropTexture);
    glUniform1i(glGetUniformLocation(dropShader->ID, "dropTex"), 0);

    glBindVertexArray(dropVAO);
    glDrawArrays(GL_POINTS, 0, drops.size());
    dropShader->desuse();
    // Render splashes
    if (!splashes.empty()) {
        glUseProgram(splashShader->ID);
        glUniformMatrix4fv(glGetUniformLocation(splashShader->ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(splashShader->ID, "proj"), 1, GL_FALSE, &proj[0][0]);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, splashTexture);
        glUniform1i(glGetUniformLocation(splashShader->ID, "splashTex"), 0);

        glBindVertexArray(splashVAO);
        glDrawArrays(GL_POINTS, 0, splashes.size());
        glDisable(GL_PROGRAM_POINT_SIZE);
        splashShader->desuse();
    }
}
