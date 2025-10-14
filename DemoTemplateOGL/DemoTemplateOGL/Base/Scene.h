#pragma once
#include "Base/model.h"
#include "SkyDome.h"
#include "Terreno.h"
#include "Billboard.h"
#include "BillboardAnimation.h"
#include "Billboard2D.h"
#include "Texto.h"
#include "Water.h"
#include "Animator.h"
#include "Animation.h"
#include "Pincipal.h"
#include "../InputDevices/KeyboardInput.h"

class Scene {
public:
    virtual float getAngulo() = 0;
    virtual void setAngulo(float angulo) = 0;
    virtual Model* getMainModel() = 0;
    virtual void setMainModel(Model* mainModel) = 0;
    virtual Scene* Render() = 0;
    virtual SkyDome* getSky() = 0;
    virtual Terreno* getTerreno() = 0;
    virtual std::vector<Model*> *getLoadedModels() = 0;
    virtual std::vector<Billboard*> *getLoadedBillboards() = 0;
    virtual std::vector<BillboardAnimation*> *getLoadedBillboardsAnimation() = 0;
    virtual std::vector<Billboard2D*> *getLoadedBillboards2D() = 0;
    virtual std::vector<Texto*> *getLoadedText() = 0;
    virtual ~Scene(){ };

    virtual int update() {
        float angulo = getAngulo() + 1.5f * gameTime.deltaTime / 100.0f;
        angulo = angulo >= 360.0f ? angulo - 360.0f : angulo;
        setAngulo(angulo);
        //OLIVER CAMBIOS-----------
        // Guardar llamadas cuando no exista SkyDome
        if (getSky() != nullptr) {
            getSky()->setRotY(angulo);
            Model* camara = getMainModel();
			for (int i = 0; i < getLoadedModels()->size(); i++){
				auto it = getLoadedModels()->begin() + i;
				Model *collider = NULL, *model = *it;
				for (int j = 0; j < model->getModelAttributes()->size(); j++){
					int idxCollider = -1;
					bool objInMovement = (*model->getNextTranslate(j)) != (*model->getTranslate(j));
					glm::vec3 &posM = objInMovement ? *model->getNextTranslate(j) : *model->getTranslate(j);
					glm::vec3 ejeColision = glm::vec3(0);
					bool isPrincipal = model == camara; // Si es personaje principal, activa gravedad
					float terrainY = getTerreno()->Superficie(posM.x, posM.z);
					ModelCollider mcollider = model->update(terrainY, *getLoadedModels(), ejeColision, isPrincipal, j);
					if (mcollider.model != NULL){
						collider = (Model*)mcollider.model;
						idxCollider = mcollider.attrIdx;
					}
					if (collider != NULL && model == camara){
						if (ejeColision.y == 1){
							INFO("APLASTADO!!!! " + collider->name, "JUMP HITBOX_"+to_string(idxCollider));
							if (removeCollideModel(collider, idxCollider))
								i--;
						}
					}
					if (model->name.compare("bullet_9_mm") == 0 && mcollider.hitGround){
						if (mcollider.model != NULL){
							INFO("IMPACTO!!!! " + ((Model*)mcollider.model)->name, "Impact HITBOX_"+to_string(mcollider.attrIdx));
						}else{
//							INFO("IMPACTO!!!! ", "Impact HITBOX_"+to_string(mcollider.attrIdx));
						}
						removeCollideModel(model, j);
						j--;
					}
					if (j < 0) j = 0;
				}
				if (i < 0) i = 0;
			}
			// Actualizamos la camara
            camara->cameraDetails->CamaraUpdate(camara->getRotY(), camara->getTranslate());
            return -1;
        }
        //OLIVER CAMBIOS---------
        Model* camara = getMainModel();
        // Actualizar cooldown de da�o del principal (si aplica)
        if (auto principal = dynamic_cast<Principal*>(camara)) {
            principal->actualizarCooldown(gameTime.deltaTime);
        }
        int tp = -1;
        bool choque = false;
        for (int i = 0; i < getLoadedModels()->size(); i++) {
            auto it = getLoadedModels()->begin() + i;
            Model* collider = NULL, * model = *it;
            int idxCollider = -1;
            for (int j = 0; model->getModelAttributes()->size() > 1 && j < model->getModelAttributes()->size(); j++) {
                bool objInMovement = (*model->getNextTranslate(j)) != (*model->getTranslate(j));
                glm::vec3& posM = objInMovement ? *model->getNextTranslate(j) : *model->getTranslate(j);
                glm::vec3 ejeColision = glm::vec3(0);
                bool isPrincipal = model == camara; // Si es personaje principal, activa gravedad
                float terrainY = getTerreno()->Superficie(posM.x, posM.z);
                ModelCollider mcollider = model->update(terrainY, *getLoadedModels(), ejeColision, isPrincipal, j);
                if (mcollider.model != NULL) {
                    collider = (Model*)mcollider.model;
                    idxCollider = mcollider.attrIdx;
                }
                if (collider != NULL && model == camara) {
                    if (ejeColision.y == 1) {
                        INFO("APLASTADO!!!! " + collider->name, "JUMP HITBOX_" + to_string(idxCollider));
                        if (removeCollideModel(collider, idxCollider))
                            i--;
                    }
                }
                if (model->name.compare("bullet_9_mm") == 0 && mcollider.hitGround) {
                    if (mcollider.model != NULL) {
                        INFO("IMPACTO!!!! " + ((Model*)mcollider.model)->name, "Impact HITBOX_" + to_string(mcollider.attrIdx));
                    }
                    else {
                        //							INFO("IMPACTO!!!! ", "Impact HITBOX_"+to_string(mcollider.attrIdx));
                    }
                    removeCollideModel(model, j);
                    j--;
                }
                if (j < 0) j = 0;
            }
            // --- IA simple: zombies siguen al modelo principal ---
            if (model != camara && (model->name == "ZombieE" || model->name == "ZombieN")) {
                glm::vec3 target = *camara->getTranslate();
                glm::vec3 current = *model->getTranslate();
                glm::vec3 dir = target - current;
                dir.y = 0; // ignorar altura vertical
                float dist2 = glm::dot(dir, dir);
                if (dist2 > 0.25f) { // peque�o umbral para evitar jitter
                    float dist = glm::sqrt(dist2);
                    glm::vec3 step = (dir / dist) * (0.8f * (float)gameTime.deltaTime / 100.0f); // velocidad * dt
                    current += step;
                    current.y = getTerreno()->Superficie(current.x, current.z);
                    model->setNextTranslate(&current);
                    // ROTAR zombie para que "mire" hacia el jugador usando eje Z
                    //float yaw = glm::degrees(glm::atan(dir.x, dir.z));
                    //model->setNextRotX(yaw); // <-- cambio solicitado (antes Y)
                }
            }
            bool objInMovement = (*model->getNextTranslate()) != (*model->getTranslate());
            glm::vec3& posM = objInMovement ? *model->getNextTranslate() : *model->getTranslate();
            glm::vec3 ejeColision = glm::vec3(0);
            bool isPrincipal = model == camara; // Si es personaje principal, activa gravedad
            ModelCollider mcollider = model->update(getTerreno()->Superficie(posM.x, posM.z), *getLoadedModels(), ejeColision, isPrincipal);
            if (mcollider.model != NULL) {
                collider = (Model*)mcollider.model;
                idxCollider = mcollider.attrIdx;
            }
            if (collider != NULL && model == camara) {
                // 1) Gasolina: sumar y eliminar solo ese modelo
                if (collider->name == "Gasolina" || collider->name == "gasRandom1" || collider->name == "gasRandom2") {
                    long valActual = 0;
                    Texto* gasCounter = nullptr;
                    auto textos = getLoadedText();
                    for (Texto* t : *textos) {
                        if (t && t->name == "GasCounter") {
                            gasCounter = t;
                            // Texto esperado formato "N/10"
                            WCHAR* actual = t->getTexto();
                            // Extraer n�mero antes de '/'
                            wchar_t* slash = wcschr(actual, L'/');
                            if (slash) {
                                *slash = L'\0';
                                valActual = wcstol(actual, nullptr, 10);
                                *slash = L'/'; // restaurar
                            } else {
                                // fallback si formato incorrecto
                                valActual = wcstol(actual, nullptr, 10);
                            }
                            break;
                        }
                    }
                    long nuevoValor = std::min(valActual + 1, 10L);
                    if (gasCounter) {
                        WCHAR nuevo[32];
                        swprintf(nuevo, 32, L"%ld/10", nuevoValor);
                        gasCounter->initTexto(nuevo);
                    }
                    if (removeCollideModel(collider, idxCollider)) {
                        i--;
                    } else {
                        auto& models = *getLoadedModels();
                        auto itc = std::find(models.begin(), models.end(), collider);
                        if (itc != models.end()) {
                            models.erase(itc);
                            delete collider;
                            i--;
                        }
                    }
                    // Mostrar mensaje cuando alcanza 10
                    if (nuevoValor == 10) {
                        bool yaExiste = false;
                        for (Texto* t : *getLoadedText()) {
                            if (t && t->name == "GasHint") { yaExiste = true; break; }
                        }
                        if (!yaExiste) {
                            Texto* hint = new Texto((WCHAR*)L"Regresa a tu auto y escapa por 1a p1uma de stop", 10.0f, 0.0f, 50.0f, 200.0f, 0.0f, camara);
                            hint->name = "GasHint";
                            getLoadedText()->emplace_back(hint);
                        }
                    }
                    continue; // no quitar vidas
                }
                // 2) Si hay >= 10 gasolinas y chocamos con el auto, cambiar control/c�mara al auto
                if (collider->name == "AutoChido") {
                    long val = 0;
                    auto textos = getLoadedText();
                    for (Texto* t : *textos) {
                        if (t && t->name == "GasCounter") {
                            WCHAR* actual = t->getTexto();
                            wchar_t* slash = wcschr(actual, L'/');
                            if (slash) {
                                *slash = L'\0';
                                val = wcstol(actual, nullptr, 10);
                                *slash = L'/';
                            } else {
                                val = wcstol(actual, nullptr, 10);
                            }
                            break;
                        }
                    }
                    if (val >= 10) {
                        if (auto principal = dynamic_cast<Principal*>(camara)) {
                            principal->setActive(false);
                        }
                        setMainModel(collider);
                        camara = getMainModel();
                        memset(KEYS, 0, 255);
                        continue;
                    }
                }
                // 3) Victoria: colisi�n con StopFinal usando el auto
                if (collider->name == "StopFinal" && camara->name == "AutoChido") {
                    INFO("HAZ ESCAPADO!", "GAME WIN");
#ifdef _WIN32
                    PostQuitMessage(0);
#else
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
#endif
                    return tp; // Terminar update inmediatamente
                }
                // 4) Enemigos: aplicar da�o (o eliminar si el principal es el cqrro)
                if (collider->name == "ZombieE" || collider->name == "ZombieN") {
                    if (camara->name == "AutoChido") {
                        // Eliminar zombie al chocar con el auto
                        auto &models = *getLoadedModels();
                        auto zit = std::find(models.begin(), models.end(), collider);
                        if (zit != models.end()) {
                            // liberar hitboxes internas
                            for (auto &attr : *collider->getModelAttributes()) {
                                if (attr.hitbox) {
                                    Model* hb = (Model*)attr.hitbox;
                                    delete hb;
                                    attr.hitbox = NULL;
                                }
                            }
                            models.erase(zit);
                            delete collider;
                            i--; // ajustar �ndice tras borrado
                        }
                        continue; // no da�o al auto
                    }
                    if (collider->name == "ZombieE")
                        tp = -1;
                    if (auto principal = dynamic_cast<Principal*>(camara)) {
                        if (principal->puedeRecibirDano()) {
                            choque = true;
                        }
                    }
                }
            }
            /*if (collider != NULL && model == camara){
                if (ejeColision.y == 1){
                    INFO("APLASTADO!!!!", "JUMP HITBOX");
                    if (removeCollideModel(collider, idxCollider))
                        i--;
                }
            }*/
            if (i < 0) i = 0;
        }
        /*if (choque) {
            memset(KEYS, 0, 255);
            Principal* principal = (Principal*)camara;
            if (principal->vidas.size() > 0) {
                delete principal->vidas.back();
                principal->vidas.pop_back();
            }else
                INFO("HAZ MUERTO", "GAME OVER");
        }*/
        if (choque) {
            memset(KEYS, 0, 255);
            Principal* principal = (Principal*)camara; // ideal: dynamic_cast<Principal*>(camara)
            if (!principal->vidas.empty()) {
                Billboard2D* vida = principal->vidas.front();
                auto& ui = *getLoadedBillboards2D();
                auto it = std::find(ui.begin(), ui.end(), vida);
                if (it != ui.end()) {
                    ui.erase(it);
                }
                principal->vidas.erase(principal->vidas.begin());
                delete vida;
                principal->iniciarCooldownDano(3000.0);
            }
            else {
                INFO("HAZ MUERTO", "GAME OVER");
                PostQuitMessage(0);
            }
        }
        getTerreno()->lightPos = *camara->getTranslate();
        camara->cameraDetails->CamaraUpdate(camara->getRotY(), camara->getTranslate());
        return tp;
    }

    virtual bool removeCollideModel(Model* collider, int idxCollider) {
        auto it = std::find(getLoadedModels()->begin(), getLoadedModels()->end(), collider);
        if (idxCollider == 0) {
            collider->setActive(false);
            Model* AABB = (Model*)collider->getModelAttributes()->at(0).hitbox;
            delete AABB;
            collider->getModelAttributes()->at(0).hitbox = NULL;
        } else {
            ModelAttributes &attr = collider->getModelAttributes()->at(idxCollider);
            Model *AABB = (Model*)attr.hitbox;
            delete AABB;
            collider->getModelAttributes()->erase(collider->getModelAttributes()->begin() + idxCollider);
        }
        if (collider->getModelAttributes()->size() == 1 && !collider->getActive()) {
            it = std::find(getLoadedModels()->begin(), getLoadedModels()->end(), collider);
            it = getLoadedModels()->erase(it);
            delete collider;
            return true;
        }
        return false;
    }

    virtual Model* lookForCollition(glm::vec3 &yPos, bool collitionMovement = false) {
        std::vector<Model*> *ourModel = getLoadedModels();
        Model *camara = getMainModel();
        for (int i = 0; i < ourModel->size(); i++) {
            Model *model = (*ourModel)[i];
            if (model != camara) {
                if (camara->colisionaCon(model->getModelAttributes()->at(0), yPos, collitionMovement)) {
                    return model;
                }
            }
        }
        return NULL;
    }
};