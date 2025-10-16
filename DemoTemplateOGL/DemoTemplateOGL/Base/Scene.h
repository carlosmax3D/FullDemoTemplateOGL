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
					if (j < 0) j = 0;
				}
				if (i < 0) i = 0;
			}
			// Actualizamos la camara
            camara->cameraDetails->CamaraUpdate(camara->getRotY(), camara->getTranslate());
            return -1;
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