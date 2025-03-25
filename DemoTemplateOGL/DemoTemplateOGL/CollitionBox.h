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
	static int idCollition;
	CollitionBox(float x, float y, float z, float halfWidth, float halfHeight, float halfDepth, Camera* camera);
	CollitionBox(glm::vec3 position, glm::vec3 halfWHD, Camera* camera);

	~CollitionBox();

};

#endif 