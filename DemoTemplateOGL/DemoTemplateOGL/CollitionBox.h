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
	static Model* GenerateAABB(glm::vec3 &position, Node& node, Camera* camera);
	CollitionBox(float x, float y, float z, float halfWidth, float halfHeight, float halfDepth, Camera* camera);
	CollitionBox(glm::vec3 &position, Node& node, Camera* camera);
	CollitionBox(glm::vec3 &position, glm::vec3& halfWHD, Camera* camera);
	void initCollitionBox(float x, float y, float z, float centerx, float centery, float centerz, float halfWidth, float halfHeight, float halfDepth, Camera* camera);

	~CollitionBox();

};

#endif 