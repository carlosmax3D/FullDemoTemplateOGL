#ifndef _2DBill_h
#define _2DBill_h
#include "Base/Utilities.h"
#include "Base/Billboard.h"

class Billboard2D : public Billboard {
public:
	Billboard2D(int glTextura, WCHAR textura[], float x, float y, float z, Camera* camera):
        Billboard(glTextura, textura, x, y, z, camera){
            glm::vec3 pos(x,y,0);
            this->setTranslate(&pos);
            ModelAttributes &attr = this->getModelAttributes()->at(0);
            if (attr.hitbox)
                delete (Model*)attr.hitbox;
            attr.hitbox = NULL;
    }

	Billboard2D(WCHAR textura[], float ancho, float alto, float x, float y, float z, Camera* camera):
        Billboard(textura, ancho, alto, x, y, z, camera){
            glm::vec3 pos(x,y,0);
            this->setTranslate(&pos);
            ModelAttributes &attr = this->getModelAttributes()->at(0);
            if (attr.hitbox)
                delete (Model*)attr.hitbox;
            attr.hitbox = NULL;
    }

    ~Billboard2D(){
    }

    // Usa el shader default para poder imprimir el billboard
    void Draw(){
        if (gpuDemo == NULL) {
            // build and compile our shader zprogram
            // ------------------------------------
            gpuDemo = new Shader("shaders/text_shader.vs", "shaders/billboard.fs");
    //		gpuDemo = new Shader("shaders/models/1.model_material_loading.vs", "shaders/models/1.model_material_loading.fs");
            setDefaultShader(true);
        }
        if (getDefaultShader()) {
            gpuDemo->use();
            Model::prepShader(*gpuDemo,(*getModelAttributes())[0]);
            prepShader(*gpuDemo);
            gpuDemo->setInt("texture_diffuse1", 0);
            Draw(*gpuDemo);
            gpuDemo->desuse();
        } else Draw(*gpuDemo);
    }

    void Draw(Shader &shader){
        Billboard::Draw(shader);
    }

    void prepShader(Shader& shader){
    //	glm::mat4 projection = cameraDetails->cameraDetails->getProjection();
        glm::mat4 projection = glm::ortho(0.0f, (SCR_WIDTH+0.0f), (SCR_HEIGHT+0.0f), 0.0f, -1.0f, 1.0f);
        glm::mat4 view = cameraDetails->getView();

        glm::mat4 model = glm::mat4(1.0f);
    //	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::translate(model, *this->getTranslate()); // translate it down so it's at the center of the scene
        if (this->getScale() != NULL)
            model = glm::scale(model, *this->getScale());	// it's a bit too big for our scene, so scale it down

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
    }

    glm::vec2 rotate_pt(glm::vec2& rotate_about, glm::vec2 pt, float& rotation_angle){
        // Return the rotation point
        glm::vec2 translated_pt = pt - rotate_about;

        // Apply rotation
        float radians = (rotation_angle  * 3.14159365f) / 180.0f; // convert degrees to radians
        float cos_theta = cos(radians);
        float sin_theta = sin(radians);

        // Rotated point of the corners
        glm::vec2 rotated_pt = glm::vec2((translated_pt.x * cos_theta) - (translated_pt.y * sin_theta),
            (translated_pt.x * sin_theta) + (translated_pt.y * cos_theta));

        return (rotated_pt + rotate_about);
    }
};

#endif 