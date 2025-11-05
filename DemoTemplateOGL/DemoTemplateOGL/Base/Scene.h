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
#include "Principal.h"
#include <GLFW/glfw3.h>


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
			Principal* principal = (Principal*)getMainModel();                   //Tal vez
			if (principal->enInvulnerable) {									 //Tal vez
				principal->tiempoInvulnerable -= gameTime.deltaTime;			 //Tal vez
				if (principal->tiempoInvulnerable <= 0) {						 //Tal vez
					principal->enInvulnerable = false;                           //Tal vez
				}																 //Tal vez
			}
            float angulo = getAngulo() + 1.5 * gameTime.deltaTime / 100;
            angulo = angulo >= 360 ? angulo - 360.0 : angulo;
            setAngulo(angulo);
            getSky()->setRotY(angulo);
            Model* camara = getMainModel();
			int tp = -1;
			bool choque = false;
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
					//////////////////
					/*if (model->name.compare("Arana") == 0 && j == 0) {
						glm::vec3 posJugador = *camara->getNextTranslate();
						glm::vec3 posArana = posM;

						glm::vec3 dir = posJugador - posArana;					//
						dir.y = 0.0f;											//
						float distancia = glm::length(dir);						//
						if (distancia > 0.1f) dir = glm::normalize(dir);		//
						float rotY = atan2(dir.x, dir.z) * (180.0f / M_PI) + 180.0f;	//
						model->setNextRotY(rotY);								//

						float followSpeed = 1.0f * gameTime.deltaTime / 1000;
						posArana = glm::mix(posArana, posJugador, followSpeed);
						posArana.y = getTerreno()->Superficie(posArana.x, posArana.z) + 1;
						model->setNextTranslate(&posArana);

						if (distancia < 5.0f)									//
							model->setAnimation(2); // atacar					//
						else if (distancia < 25.0f)								//
							model->setAnimation(1); // caminar					//
						//else                                                  //
						//	model->setAnimation(0); // idle						//
						/////////////////	
					}*/
					if (collider != NULL && model == camara){
						if (ejeColision.y == 1 && collider->name == "Arana"){
							INFO("¡¡APLASTADA!!", "Adiós, araña");
							if (removeCollideModel(collider, idxCollider))
								i--;
						} else {
							choque = true;
						}
						if (collider->name == "PuertaCabana") {
							static bool esperandoRespuesta = false;
							static bool yaMostrado = false;

							int respuesta = MessageBoxA(NULL, "¿Refugiarte en la cabaña?", "Entrar", MB_YESNO | MB_ICONQUESTION);
							if (respuesta == IDYES) {
								tp = 1;
								entrarAlEdificio();
							} 
							else {
								INFO("Entonces quédate afuera.", "Entrar");
							}

						}
					}
					if (j < 0) j = 0;
				}	
			}
			if (choque) {
				Principal* principal = (Principal*)camara;
				if (!principal->enInvulnerable) {                               //Tal vez
					if (principal->vidas.size() > 0) {
						delete principal->vidas.back();
						principal->vidas.pop_back();
						principal->enInvulnerable = true;                       //Tal vez
						principal->tiempoInvulnerable = 2000.0f;                //Tal vez
						//INFO("¡Has recibido daño!", "Ouch");				    //Tal vez
					}
					else {
						INFO("TE HAS MUERTO.", "GAMEOVER");
						choque = false;
					}
				}
			}

			// Actualizamos la camara
            camara->cameraDetails->CamaraUpdate(camara->getRotY(), camara->getTranslate());
            return tp;
        }

		void entrarAlEdificio() {
			Model* camara = getMainModel();	
			glm::vec3 posInterior = glm::vec3(0.0f, 0.0f, 25.0f); // interior de la cabaña
			camara->setTranslate(&posInterior);
			camara->setNextTranslate(&posInterior);
			float nuevaRotY = camara->getRotY() + 180.0f;
			if (nuevaRotY >= 360.0f) nuevaRotY -= 360.0f; // eo
			camara->setRotY(nuevaRotY);
			camara->setNextRotY(nuevaRotY);
			camara->cameraDetails->CamaraUpdate(nuevaRotY, camara->getTranslate());
			INFO("Has entrado a la cabaña.", "Entrar");
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