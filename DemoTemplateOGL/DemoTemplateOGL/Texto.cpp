#include "Texto.h"
#ifdef ENGINE_DIRECTX
#include "Base/MeshDX11.h"
#endif
#ifdef __linux__ 
#define ZeroMemory(x,y) memset(x,0,y)
#define wcscpy_s(x,y,z) wcscpy(x,z)
#define wcstombs_s(x, y, z, w, r) wcstombs(y,w,r)
#endif

Texto::Texto(wstring &texto, float escala, float rotacion, float x, float y, float z, Model* camera){
	this->scale = escala;
	this->rotacion = rotacion;
	this->x = x;
	this->y = y;
	this->z = z;
	cameraDetails = camera;
	wcscpy_s((wchar_t*)this->texto, 512, texto.c_str());
    initTexto();
}

Texto::Texto(WCHAR *texto, float escala, float rotacion, float x, float y, float z, Model* camera){
	this->scale = escala;
	this->rotacion = rotacion;
	this->x = x;
	this->y = y;
	this->z = z;
	cameraDetails = camera;
	wcscpy_s((wchar_t*)this->texto, 512, (const wchar_t*)texto);
    initTexto();
}

void Texto::initTexto(wstring &texto){
	wcscpy_s((wchar_t*)this->texto, 512, texto.c_str());
	initTexto();
}

void Texto::initTexto(WCHAR *texto){
	wcscpy_s((wchar_t*)this->texto, 512, (const wchar_t*)texto);
	initTexto();
}

void Texto::initTexto(){
    long tLength = wcslen((const wchar_t*)texto);
	if (this->textBillboard.capacity() < tLength)
	    this->textBillboard.reserve(tLength);
	font_atlas &fontTexture = font_atlas::getInstance();
	char stext[1024];
//    std::string stext(texto.begin(), texto.end());
	wcstombs_s(NULL, stext, 1024, (wchar_t*)texto, 512);
	// Store the x,y location
	glm::vec2 loc = glm::vec2(x,y);
	glm::vec2 rotated_pt;

	float xpos = x;
	float ypos = y;
	for (int i = this->textBillboard.size(); i < tLength; i++){
        textBillboard.emplace_back(new Billboard2D(0, (WCHAR*)L"TEXTO", 0, 0, 0, cameraDetails->cameraDetails, GL_DYNAMIC_DRAW, GL_DYNAMIC_DRAW));
		textBillboard.back()->setCleanTextures(false);
	}
	for (int i = 0; i < textBillboard.size(); i++) {
		if (i >= tLength){
			textBillboard[i]->setActive(false);
			continue;
		}
		Billboard2D &letra = *textBillboard[i];
		// get the atlas information
		char ch = stext[i];

		Character &ch_data = fontTexture.ch_atlas[ch];

//		float xpos = x + (ch_data.Bearing.x * scale);
//		float ypos = y - (ch_data.Size.y - ch_data.Bearing.y) * scale;

		float w = ch_data.Size.x * scale;
		float h = ch_data.Size.y * scale;
		glm::vec3 scaleV(w, h, 1);
//		rotated_pt = rotate_pt(loc, glm::vec2(xpos, ypos + h), rotacion);
		glm::vec3 pos = glm::vec3(xpos, ypos, 0);
		letra.setTranslate(&pos);
		letra.setScale(&scaleV);
		letra.setActive(true);
#ifdef ENGINE_DIRECTX
		letra.textures_loaded[0]->idDX11 = fontTexture.texture.idDX11;
		letra.meshes[0]->textures[0].idDX11 = fontTexture.texture.idDX11;
		MeshDX11* m = (MeshDX11*)letra.meshes[0];
		m->fontType = true;
#else
		letra.textures_loaded[0]->id = fontTexture.texture.id;
		letra.meshes[0]->textures[0].id = fontTexture.texture.id;
#endif
		float margin = 0.00002; // This value prevents the minor overlap with the next char when rendering
		float texCoords[] = { ch_data.bot_right.x - margin, ch_data.bot_right.y,
							  ch_data.top_left.x + margin,  ch_data.bot_right.y,
							  ch_data.top_left.x + margin, ch_data.top_left.y,
							  ch_data.bot_right.x - margin, ch_data.top_left.y };
		letra.setTextureCoords(texCoords);
		xpos += scale;
	}
}

Texto::~Texto(){
    if (gpuDemo != NULL)
        delete gpuDemo;
	for (int i = 0; i < textBillboard.size(); i++)
		delete textBillboard[i];
	textBillboard.clear();
}

// Usa el shader default para poder imprimir el billboard
void Texto::Draw(){
	if (gpuDemo == NULL) {
		// build and compile our shader zprogram
		// ------------------------------------
		gpuDemo = Shader::createShader("shaders/text_shader.vs", "shaders/text_shader.fs");
//		gpuDemo = Shader::createShader("shaders/models/1.model_material_loading.vs", "shaders/models/1.model_material_loading.fs");
		setDefaultShader(true);
	}
	if (defaultShader) {
		gpuDemo->use();
		gpuDemo->setInt("texture_diffuse1", 0);
		Draw(*gpuDemo);
		gpuDemo->desuse();
	} else Draw(*gpuDemo);
}

void Texto::Draw(Shader &shader){
    for (int i = 0; showStats && i < textBillboard.size(); i ++){
		prepShader(shader, *textBillboard[i]);
	    textBillboard[i]->Draw(shader);
    }
}

void Texto::prepShader(Shader& shader, Billboard2D &bill){
	glm::vec3& pos = *bill.getTranslate();
//	glm::mat4 projection = cameraDetails->cameraDetails->getProjection();
    glm::mat4 projection = glm::ortho(0.0f, (SCR_WIDTH+0.0f), (SCR_HEIGHT+0.0f), 0.0f, -1.0f, 1.0f);
	glm::mat4 view = cameraDetails->cameraDetails->getView();

	glm::mat4 model = glm::mat4(1.0f);
//	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::translate(model, pos); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(scale, scale, 0.0f));

	shader.setVec3("color", glm::vec3(128,128,128));
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);
}

glm::vec2 Texto::rotate_pt(glm::vec2& rotate_about, glm::vec2 pt, float& rotation_angle){
	// Return the rotation point
	glm::vec2 translated_pt = pt - rotate_about;

	// Apply rotation
	float radians = (rotation_angle  * 3.14159365f) / 180.0f; // convert degrees to radians
	float cos_theta = cos(radians);
	float sin_theta = sin(radians);

	// Rotated point of the corners
	glm::vec2 rotated_pt = glm::vec2((translated_pt.x * cos_theta) - (translated_pt.y * sin_theta),
		(translated_pt.x * sin_theta) + (translated_pt.y * cos_theta));

	return (rotated_pt + rotate_about);
}

void Texto::setDefaultShader(bool defaultShader){
    this->defaultShader = defaultShader;
}

void Texto::setScale(float scale){
	this->scale = scale;
}

float Texto::getScale(){
	return this->scale;
}

WCHAR *Texto::getTexto(){
	return this->texto;
}