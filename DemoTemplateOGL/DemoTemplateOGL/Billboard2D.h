#ifndef _2DBill_h
#define _2DBill_h
#include "Base/Utilities.h"
#include "Base/Billboard.h"

class Billboard2D : public Billboard {
public:
	Billboard2D(int glTextura, WCHAR textura[], float x, float y, float z, Camera* camera):
        Billboard(glTextura, textura, x, y, z, camera){
            long tLength = wcslen((const wchar_t*)textura);
            char stext[1024];
            Texture t;
            t.id = glTextura;
#ifdef __linux__ 
            wcstombs(stext,(wchar_t*)textura,1024);
            strcpy(t.type, "texture_diffuse");
            strcpy(t.path, stext);
#else
            wcstombs_s(NULL, stext, 1024, (wchar_t*)textura, 1024);
            strcpy_s(t.type, 255, "texture_diffuse");
            strcpy_s(t.path, 1024, stext);
#endif
            initBillboard(t, 0, 0, x, y, z, camera, GL_DYNAMIC_DRAW);
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
            auto attributes = this->getModelAttributes();
            for (int i = 0 ; i < 1 ; i++){
                Model::prepShader(*gpuDemo, attributes->at(i));
                prepShader(*gpuDemo, i);
                gpuDemo->setInt("texture_diffuse1", 0);
                Model::Draw(*gpuDemo, i);
            }
            gpuDemo->desuse();
        } else Draw(*gpuDemo);
    }

    void Draw(Shader &shader){
        Billboard::Draw(shader);
    }

    void prepShader(Shader& shader, int idx = 0){
    //	glm::mat4 projection = cameraDetails->cameraDetails->getProjection();
        glm::mat4 projection = glm::ortho(0.0f, (SCR_WIDTH+0.0f), (SCR_HEIGHT+0.0f), 0.0f, -1.0f, 1.0f);
        glm::mat4 view = cameraDetails->getView();
        ModelAttributes &attr = this->getModelAttributes()->at(idx);
        if (this->getModelAttributes()->size() == 1){
            glm::mat4 model = glm::mat4(1.0f);
        //	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
            model = glm::translate(model, attr.translate);  // translate it down so it's at the center of the scene
            if (this->getRotX() != 0) 
                model = glm::rotate(model, glm::radians(attr.rotX), glm::vec3(1, 0, 0)); 
            if (this->getRotY() != 0)
                model = glm::rotate(model, glm::radians(attr.rotY), glm::vec3(0, 1, 0)); 
            if (this->getRotZ() != 0) 
                model = glm::rotate(model, glm::radians(attr.rotZ), glm::vec3(0, 0, 1)); 
            if (this->getScale() != NULL) 
                model = glm::scale(model, attr.scale);	// it's a bit too big for our scene, so scale it down 
            shader.setMat4("model", model);
        }
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
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

    void initBillboard(Texture &texture, float ancho, float alto, float x, float y, float z, Camera* camera, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW){
        cameraDetails = camera;
        vector<unsigned int> indices;
        vector<Vertex>	vertices;
        vector<Texture>	textures;

        glm::vec3 origin = glm::vec3(x, y, z);
        setTranslate(&origin);
        textures.emplace_back(texture);
        vertices.reserve(4);
        for (int i = 0; i < 4; i++)
            vertices.emplace_back();
        reloadData(&vertices);
        indices.reserve(6);
        indices = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };
        gpuDemo = NULL;
        meshes.emplace_back(new Mesh(vertices, indices, textures, VBOGLDrawType, EBOGLDrawType));
        textures_loaded.emplace_back(&this->meshes[0]->textures.data()[0]);
    }
};

#endif 