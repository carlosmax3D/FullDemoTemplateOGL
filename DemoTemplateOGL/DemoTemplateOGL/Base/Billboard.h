#ifndef _billb
#define _billb
#include "Utilities.h"
#include "model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glext.h"
#include "wglext.h"

class Billboard : public Model{

private:
	float alto, ancho;

	void reloadData(vector<Vertex> *vertices, glm::vec3 origin);
public:
	Billboard(int glTextura, WCHAR textura[], float x, float y, float z, Camera* camera);

	Billboard(WCHAR textura[], float ancho, float alto, float x, float y, float z, Camera* camera);

	~Billboard();

	virtual void reloadData();
	void reloadData(vector<Vertex> *vertices);

	virtual void initBillboard(Texture &texture, float ancho, float alto, float x, float y, float z, Camera* camera, int VBOGLDrawType = GL_STATIC_DRAW, int EBOGLDrawType = GL_STATIC_DRAW);
	// Usa el shader default para poder imprimir el billboard
	virtual void Draw();

	virtual void Draw(Shader &shader);

	virtual void prepShader(Shader& shader, int idx = 0);

	void setTextureCoords(float *tCoords);
};

#endif 