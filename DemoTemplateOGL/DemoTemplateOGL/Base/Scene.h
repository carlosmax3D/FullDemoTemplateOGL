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
		virtual ~Scene(){
		};

		virtual int update(){
            float angulo = getAngulo() + 1.5 * gameTime.deltaTime / 100;
            angulo = angulo >= 360 ? angulo - 360.0 : angulo;
            setAngulo(angulo);
            getSky()->setRotY(angulo);
            Model* camara = getMainModel();

			//creamos una variable para hacer el tp
			int tp = -1;

			//creamos unas variables para el acumulo de objetos
			bool objetos[5] = { false,false,false,false,false };
			//el 1 es la pelota, el 2 es el triciclo, el tercero es el libro, el cuarto es el oso y el quinto es el coche

			for (int i = 0; i < getLoadedModels()->size(); i++){
				auto it = getLoadedModels()->begin() + i;
				Model *collider = NULL, *model = *it;
				for (int j = 0; j < model->getModelAttributes()->size(); j++){
					int idxCollider = -1;
					bool objInMovement = (*model->getNextTranslate(j)) != (*model->getTranslate(j));
					glm::vec3 &posM = objInMovement ? *model->getNextTranslate(j) : *model->getTranslate(j);
					glm::vec3 ejeColision = glm::vec3(0);
					bool isPrincipal = model == camara; // Si es personaje principal, activa gravedad
					if (isPrincipal && model->name.compare("Walking") == 0) {
						isPrincipal = model->name.compare("Walking") == 0;
					}
					ModelCollider mcollider = model->update(getTerreno()->Superficie(posM.x, posM.z), *getLoadedModels(), ejeColision, isPrincipal, j);
					if (mcollider.model != NULL){
						collider = (Model*)mcollider.model;
						idxCollider = mcollider.attrIdx;
					}

					//seguimiento del modelo

					if (model->name.compare("monstruo") == 0) {
						glm::vec3 positionA = *camara->getNextTranslate();
						glm::vec3 positionB = *model->getNextTranslate();


						glm::vec3 dir = positionA - positionB;
						dir.y = 0.0f;
						float distancia = glm::length(dir);
						if (distancia > 0.1f) dir = glm::normalize(dir);
						float rotY = atan2(dir.x, dir.z) * (180.0f * M_PI) + 180.0f;
						model->setNextRotZ(rotY);

						float followSpeed = 0.5f * gameTime.deltaTime / 1000;
						//positionB = glm::vec3(foatamat[3]);

						positionB = glm::mix(positionB, positionA, followSpeed);

						//para que respete la altura del terreno debemos mover en y
						positionB.y = getTerreno()->Superficie(positionB.x, positionB.z);
						model->setNextTranslate(&positionB);
						//model->setTranslate(&positionB);
					}
					
					//sacar el collider del monstruo 
					//preguntar por el nombre del modelo para sacar la colision y hacer el tp al otro escenario
					if (collider != NULL && model == camara){
						if (collider->name.compare("casa") == 0) {
							tp = 1;
						}
						if (ejeColision.y == 1){
							INFO("APLASTADO!!!! " + collider->name, "JUMP HITBOX_"+to_string(idxCollider));
							if (removeCollideModel(collider, idxCollider))
								i--;
						}
						
					}
					else if (collider != NULL) {
						if (model->name.compare("monstruo") == 0 && collider == camara) {
							bool agregar = true;
							for (Texto* a : *getLoadedText()) {
								if (a->name.compare("GAMEOVER") == 0) {
									agregar = false;
									break;
								}
							}
							if (agregar) {
								Texto* coordenadas = new Texto((WCHAR*)L"PERDISTE", 40, 0, 0, 44, 0, model);
								coordenadas->name = "GAMEOVER";
								getLoadedText()->emplace_back(coordenadas);
							}
						}
				}
					if (j < 0) j = 0;
			}
				if (i < 0) i = 0;
			}
			// Actualizamos la camara
            //camara->cameraDetails->CamaraUpdate(camara->getRotZ(), camara->getTranslate());
			camara->cameraDetails->CamaraUpdate(camara->getRotY(), camara->getTranslate());
            return tp;
        }

		virtual bool removeCollideModel(Model* collider, int idxCollider){
			auto it = std::find(getLoadedModels()->begin(), getLoadedModels()->end(), collider);
			if (idxCollider == 0){
				collider->setActive(false);
				Model* AABB = (Model*)collider->getModelAttributes()->at(0).hitbox;
				if (AABB)
					delete AABB;
				collider->getModelAttributes()->at(0).hitbox = NULL;
			} else {
				ModelAttributes &attr = collider->getModelAttributes()->at(idxCollider);
				Model *AABB = (Model*)attr.hitbox;
				if (AABB)
					delete AABB;
				collider->getModelAttributes()->erase(collider->getModelAttributes()->begin() + idxCollider);
			}
			if (collider->getModelAttributes()->size() == 1 && !collider->getActive()){
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