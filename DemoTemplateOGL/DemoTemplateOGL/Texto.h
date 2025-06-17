#ifndef _textoP_h
#define _textoP_h
#include "Base/Utilities.h"
#include "Billboard2D.h"
#include "Base/font_atlas.h"

class Texto {

private:
    std::vector<Billboard2D*> textBillboard;
	Model* cameraDetails = NULL;
    Shader* gpuDemo = NULL;
	bool defaultShader = false;
	float scale = 0;
	WCHAR texto[512];
	float rotacion;
	float x;
	float y; 
	float z;
public:
	std::string name = "";
	Texto(std::wstring &texto, float escala, float rotacion, float x, float y, float z, Model* camera);

	Texto(WCHAR *texto, float escala, float rotacion, float x, float y, float z, Model* camera);

	~Texto();

	void initTexto(WCHAR *texto);
	void initTexto(wstring &texto);
    void initTexto();

    // Usa el shader default para poder imprimir el billboard
	void Draw();

	void Draw(Shader &shader);

	void prepShader(Shader& shader, glm::vec3 &pos);

    glm::vec2 rotate_pt(glm::vec2& rotate_about, glm::vec2 pt, float& rotation_angle);
	void setDefaultShader(bool defaultShader);
	float getScale();
	void setScale(float scale);
	WCHAR* getTexto();
};

#endif 