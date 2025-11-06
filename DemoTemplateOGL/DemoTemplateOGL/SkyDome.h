#ifndef _Sky
#define _Sky
#include "Base/Utilities.h"
#include "Base/model.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>  // Para std::setprecision y std::fixed
#include <map>
#include <vector>

class SkyDome : public Model {
	//El constructor llama al metodo Esfera de la clase geometrias que generara los vertices
	//normales y uvs de la misma, nos regresa la estructura Maya.
private:
    float timeOfDay; // 0-24 horas virtuales
    float daySpeed;  // Velocidad del ciclo
    GLuint textureDay, textureAfternoon, textureNight;
    bool useMultipleTextures;
    int debugCounter;
public:
	SkyDome(int stacks, int slices, float radio, WCHAR *nombre, Camera* camera) {
		cameraDetails = camera;
		vector<unsigned int> indices;
		vector<Texture> textures;
		vector<Vertex>	vertices;
		vector<Material> materials;
		unsigned int esferaTextura;
		if (this->getModelAttributes()->size() == 0){
			ModelAttributes attr{0};
			this->getModelAttributes()->push_back(attr);
		}
		UTILITIES_OGL::Maya cuadro = UTILITIES_OGL::Esfera(stacks, slices, radio, 0.5, 1);
		UTILITIES_OGL::vectoresEsfera(cuadro, vertices, indices, stacks * slices * 3, (stacks - 1) * (slices - 1) * 6);
		delete[] cuadro.maya;
		delete[] cuadro.indices;
		// cargamos la textura de la figura
		char stext[1024];
		Texture t;
#ifdef _WIN32
		wcstombs_s(NULL, stext, 1024, (wchar_t*)nombre, 1024);
		strcpy_s(t.type, 255, "texture_diffuse");
		strcpy_s(t.path, 1024, stext);
#else
		wcstombs(stext, (wchar_t*)nombre, 1024);
		strcpy(t.type, "texture_diffuse");
		strcpy(t.path, stext);
#endif
		esferaTextura = TextureFromFile(stext, this->directory);
		t.id = esferaTextura;
		textures.emplace_back(t);
		gpuDemo = NULL;
		meshes.emplace_back(new Mesh(vertices, indices, textures, materials));
		textures_loaded.emplace_back(&this->meshes[0]->textures.data()[0]);
		setDefaultShader(false);
	}
    // Constructor para múltiples texturas (ciclo día/noche)
    SkyDome(int stacks, int slices, float radio,
        WCHAR* dayTex, WCHAR* afternoonTex, WCHAR* nightTex,
        Camera* camera, float initialTime) {
        cameraDetails = camera;
        timeOfDay = initialTime;
        // 6 minutos = 360 segundos para ciclo completo de 24 horas
        daySpeed = 24.0f / 360.0f; // = 0.0667
        useMultipleTextures = true;
        debugCounter = 0;
        gpuDemo = NULL;

        vector<unsigned int> indices;
        vector<Texture> textures;
        vector<Vertex> vertices;
        vector<Material> materials;
        ModelAttributes attr{ 0 };
		if (this->getModelAttributes()->size() < 1)
			this->getModelAttributes()->push_back(attr);

        UTILITIES_OGL::Maya cuadro = UTILITIES_OGL::Esfera(stacks, slices, radio, 0.5, 1);
        UTILITIES_OGL::vectoresEsfera(cuadro, vertices, indices, stacks * slices * 3, (stacks - 1) * (slices - 1) * 6);
        delete[] cuadro.maya;
        delete[] cuadro.indices;

        // Cargar las tres texturas
        char stext[1024];

        // Textura de la noche
#ifdef _WIN32
        wcstombs_s(NULL, stext, 1024, (wchar_t*)nightTex, 1024);
#else
        wcstombs(stext, (wchar_t*)nightTex, 1024);
#endif
        textureNight = TextureFromFile(stext, this->directory);

        // Textura del día
#ifdef _WIN32
        wcstombs_s(NULL, stext, 1024, (wchar_t*)dayTex, 1024);
#else
        wcstombs(stext, (wchar_t*)dayTex, 1024);
#endif
        textureDay = TextureFromFile(stext, this->directory);

        // Textura de la tarde
#ifdef _WIN32
        wcstombs_s(NULL, stext, 1024, (wchar_t*)afternoonTex, 1024);
#else
        wcstombs(stext, (wchar_t*)afternoonTex, 1024);
#endif
        textureAfternoon = TextureFromFile(stext, this->directory);

        // Configurar textura inicial
        Texture t;
#ifdef _WIN32
        strcpy_s(t.type, 255, "texture_diffuse");
        strcpy_s(t.path, 1024, stext);
#else
        strcpy(t.type, "texture_diffuse");
        strcpy(t.path, stext);
#endif
        t.id = textureNight;
//        textures.emplace_back(t);

        gpuDemo = NULL;
        meshes.emplace_back(new Mesh(vertices, indices, textures, materials));
//        textures_loaded.emplace_back(&this->meshes[0]->textures.data()[0]);

	}

	~SkyDome() {
		//nos aseguramos de disponer de los recursos previamente reservados
	}
    void update(float deltaTime) {
        timeOfDay += daySpeed * deltaTime;
        if (timeOfDay >= 24.0f) {
            timeOfDay = 0.0f;
        }

        debugCounter++;
        if (debugCounter % 60 == 0) {
            printDebugInfo();
        }
    }

    float getTimeOfDay() const { return timeOfDay; }
    void setTimeOfDay(float time) { timeOfDay = time; }
    float getDaySpeed() const { return daySpeed; }
    void setDaySpeed(float speed) { daySpeed = speed; }

    // Obtener factor de mezcla para transiciones suaves
    float getBlendFactor() {
        if (!useMultipleTextures) return 0.0f;

        // Cada período dura 8 horas (24/3 = 8)
        float periodDuration = 8.0f;
        float transitionDuration = 2.0f; // 2 horas de transición suave

        // NOCHE: 0-8
        if (timeOfDay >= 0.0f && timeOfDay < 8.0f) {
            // Transición hacia DÍA en las últimas 2 horas
            if (timeOfDay >= 6.0f) {
                return (timeOfDay - 6.0f) / transitionDuration; // 0.0 a 1.0
            }
            return 0.0f;
        }
        // DÍA: 8-16
        else if (timeOfDay >= 8.0f && timeOfDay < 16.0f) {
            // Transición hacia TARDE en las últimas 2 horas
            if (timeOfDay >= 14.0f) {
                return (timeOfDay - 14.0f) / transitionDuration; // 0.0 a 1.0
            }
            return 0.0f;
        }
        // TARDE: 16-24
        else {
            // Transición hacia NOCHE en las últimas 2 horas
            if (timeOfDay >= 22.0f) {
                return (timeOfDay - 22.0f) / transitionDuration; // 0.0 a 1.0
            }
            return 0.0f;
        }
    }

    // Determinar textura principal y siguiente para mezcla
    void getCurrentTextures(GLuint& tex1, GLuint& tex2) {
        if (!useMultipleTextures) {
            tex1 = textureDay;
            tex2 = textureDay;
            return;
        }

        // NOCHE: 0-8
        if (timeOfDay >= 0.0f && timeOfDay < 8.0f) {
            tex1 = textureNight;
            tex2 = textureDay; // Transición a día
        }
        // DÍA: 8-16
        else if (timeOfDay >= 8.0f && timeOfDay < 16.0f) {
            tex1 = textureDay;
            tex2 = textureAfternoon; // Transición a tarde
        }
        // TARDE: 16-24
        else {
            tex1 = textureAfternoon;
            tex2 = textureNight; // Transición a noche
        }
    }

    // Obtener color de luz ambiental con transiciones suaves
    glm::vec3 getAmbientLight() {
        glm::vec3 nightAmbient(0.08f, 0.08f, 0.15f);   // Azul oscuro noche
        glm::vec3 dayAmbient(0.75f, 0.75f, 0.80f);     // Blanco/azul día
        glm::vec3 afternoonAmbient(0.60f, 0.45f, 0.35f); // Naranja/dorado tarde

        float blend = getBlendFactor();

        // NOCHE (0-8)
        if (timeOfDay >= 0.0f && timeOfDay < 8.0f) {
            return glm::mix(nightAmbient, dayAmbient, blend);
        }
        // DÍA (8-16)
        else if (timeOfDay >= 8.0f && timeOfDay < 16.0f) {
            return glm::mix(dayAmbient, afternoonAmbient, blend);
        }
        // TARDE (16-24)
        else {
            return glm::mix(afternoonAmbient, nightAmbient, blend);
        }
    }

    // Obtener color de luz difusa con transiciones suaves
    glm::vec3 getDiffuseLight() {
        glm::vec3 nightDiffuse(0.25f, 0.25f, 0.45f);    // Azul luna
        glm::vec3 dayDiffuse(1.0f, 1.0f, 0.95f);        // Amarillo sol
        glm::vec3 afternoonDiffuse(1.0f, 0.65f, 0.45f); // Naranja atardecer

        float blend = getBlendFactor();

        // NOCHE (0-8)
        if (timeOfDay >= 0.0f && timeOfDay < 8.0f) {
            return glm::mix(nightDiffuse, dayDiffuse, blend);
        }
        // DÍA (8-16)
        else if (timeOfDay >= 8.0f && timeOfDay < 16.0f) {
            return glm::mix(dayDiffuse, afternoonDiffuse, blend);
        }
        // TARDE (16-24)
        else {
            return glm::mix(afternoonDiffuse, nightDiffuse, blend);
        }
    }

    // Obtener dirección del sol/luna
    glm::vec3 getSunDirection() {
        // El sol se mueve en círculo completo durante 24 horas
        float sunAngle = (timeOfDay / 24.0f) * 2.0f * 3.14159265f;
        float sunHeight = sin(sunAngle - 3.14159265f / 2.0f);

        return glm::normalize(glm::vec3(
            cos(sunAngle),
            sunHeight,
            sin(sunAngle)
        ));
    }

    // Obtener nombre del período actual
    std::string getCurrentPeriod() {
        if (timeOfDay >= 0.0f && timeOfDay < 8.0f) {
            return "NOCHE";
        }
        else if (timeOfDay >= 8.0f && timeOfDay < 16.0f) {
            return "DIA";
        }
        else {
            return "TARDE";
        }
    }

    // Método de debug mejorado
    void printDebugInfo() {
        glm::vec3 ambient = getAmbientLight();
        glm::vec3 diffuse = getDiffuseLight();
        float blend = getBlendFactor();

        // Calcular tiempo real transcurrido
        float cycleProgress = (timeOfDay / 24.0f) * 100.0f;
        int totalSeconds = (int)((timeOfDay / 24.0f) * 360.0f);
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        /*std::cout << "[SKYDOME] Hora: " << std::fixed << std::setprecision(2) << timeOfDay
            << " | Periodo: " << getCurrentPeriod()
            << " | Blend: " << std::setprecision(3) << blend
            << " | Tiempo: " << minutes << "m " << seconds << "s"
            << " | Progreso: " << (int)cycleProgress << "%"
            << " | Ambient: (" << ambient.r << "," << ambient.g << "," << ambient.b << ")"
            << std::endl;*/
	}

	// Usa el shader default para poder imprimir el skydome
	void Draw() {
		if (gpuDemo == NULL) {
			gpuDemo = new Shader("shaders/sky.vert", "shaders/sky.frag");
			setDefaultShader(true);
		}
		if (getDefaultShader()) {
			gpuDemo->use();
			prepShader(*gpuDemo);
			Draw(*gpuDemo);
			gpuDemo->desuse();
		} else Draw(*gpuDemo);
	}

	void Draw(Shader& shader) {
		glDisable(GL_DEPTH_TEST);
        // SISTEMA DE MÚLTIPLES TEXTURAS CON BLEND
        if (useMultipleTextures) {
            GLuint tex1, tex2;
            getCurrentTextures(tex1, tex2);
            float blendFactor = getBlendFactor();

            // Activar y enlazar ambas texturas
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex1);
            shader.setInt("texture1", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, tex2);
            shader.setInt("texture2", 1);

            shader.setFloat("blendFactor", blendFactor);
            shader.setBool("useBlending", true);
        }
        else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureDay);
            shader.setInt("texture1", 0);
            shader.setBool("useBlending", false);
        }
		Model::Draw(shader,0);
		glEnable(GL_DEPTH_TEST);
	}

	void prepShader(Shader& shader) {
		glm::vec3 lightColor = getDiffuseLight();
		glm::vec3 ambientColor = getAmbientLight();
		shader.setVec3("light.ambient", ambientColor);
		shader.setVec3("light.diffuse", lightColor);
		shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		glm::vec3 sunDir = getSunDirection();
		glm::vec3 lightPos = sunDir * 1000.0f;
		shader.setVec3("light.position", lightPos);
		shader.setVec3("light.direction", -sunDir);
		shader.setVec3("viewPos", this->cameraDetails->getPosition());
		shader.setVec3("cameraPos", this->cameraDetails->getPosition());

		// view/projection transformations
		shader.setMat4("projection", cameraDetails->getProjection());
		shader.setMat4("view", cameraDetails->getView());

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		//		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::translate(model, glm::vec3(cameraDetails->getPosition().x, cameraDetails->getPosition().y - 5, cameraDetails->getPosition().z)); // translate it down so it's at the center of the scene
		if (getRotationVector()->x != 0)
			model = glm::rotate(model, glm::radians(getRotX()), glm::vec3(1, 0, 0));
		if (getRotationVector()->y != 0)
			model = glm::rotate(model, glm::radians(getRotY()), glm::vec3(0, 1, 0));
		if (getRotationVector()->z != 0)
			model = glm::rotate(model, glm::radians(getRotZ()), glm::vec3(0, 0, 1));
		//model = glm::scale(model, glm::vec3(0.0025f, 0.0025f, 0.0025f));	// it's a bit too big for our scene, so scale it down
//			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		shader.setMat4("model", model);
        shader.setFloat("timeOfDay", timeOfDay);
	}
};
#endif