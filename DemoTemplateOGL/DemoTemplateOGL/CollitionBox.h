#pragma once
#ifndef _CollitionBox
#define _CollitionBox
#include "Base/Utilities.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Base/glext.h"
#include "Base/wglext.h"
#include "Base/model.h"

class CollitionBox : public Model {
private:
public:
	CollitionBox(float x, float y, float z, float halfWidth, float halfHeight, float halfDepth, Camera* camera) {
        buildCollider(x, y, z, halfWidth, halfHeight, halfDepth);
        this->cameraDetails = camera;
	}
	CollitionBox(glm::vec3 position, glm::vec3 halfWHD, Camera* camera) {
        buildCollider(position.x, position.y, position.z, halfWHD.x, halfWHD.y, halfWHD.z);
        this->cameraDetails = camera;
	}

	~CollitionBox() {
		//nos aseguramos de disponer de los recursos previamente reservados
	}

};

#endif 