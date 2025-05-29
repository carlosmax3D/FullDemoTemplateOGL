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
			for (auto it = getLoadedModels()->begin(); it != getLoadedModels()->end(); ){
				Model* collider = NULL, *model = *it;
				bool objInMovement = (*model->getNextTranslate()) != (*model->getTranslate());
				glm::vec3 &posM = objInMovement ? *model->getNextTranslate() : *model->getTranslate();
				glm::vec3 ejeColision = glm::vec3(0);
				if (model == camara) // Si es personaje principal, activa gravedad
					collider = model->update(getTerreno()->Superficie(posM.x, posM.z), *getLoadedModels(), ejeColision, true);
				else 
					collider = model->update(getTerreno()->Superficie(posM.x, posM.z), *getLoadedModels(), ejeColision);
				if (collider != NULL && model == camara && ejeColision.y == 1){
					INFO("APLASTADO!!!!", "JUMP HITBOX");
					it = std::find(getLoadedModels()->begin(), getLoadedModels()->end(), collider);
					Model* eliminado = *it;
					it = getLoadedModels()->erase(it);
					delete eliminado;
				} else it++;
			}
			// Actualizamos la camara
			camara->cameraDetails->CamaraUpdate(camara->getRotY(), camara->getTranslate());
			return -1;
		}

		virtual Model* lookForCollition(glm::vec3 &yPos, bool collitionMovement = false) {
			std::vector<Model*> *ourModel = getLoadedModels();
			Model *camara = getMainModel();
			for (int i = 0; i < ourModel->size(); i++) {
				Model *model = (*ourModel)[i];
				if (model != camara) {
					if (camara->colisionaCon(*model, yPos, collitionMovement)) {
						return model;
					}
				}
			}
			return NULL;
		}
};