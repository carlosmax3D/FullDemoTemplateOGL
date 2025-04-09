#include "Billboard.h"
#ifdef __linux__ 
#define ZeroMemory(x,y) memset(x,0,y)
#define wcscpy_s(x,y,z) wcscpy(x,z)
#define strcpy_s(x,y,z) strcpy(x,z)
#define wcstombs_s(x, y, z, w, r) wcstombs(y,w,r)
#endif

void Billboard::reloadData(){
	reloadData(&(meshes[0]->vertices));
}

void Billboard::reloadData(vector<Vertex> *vertices){
    float verts[] = {
	// positions        // texture coords
		1.0f, 1.0f, 0.0f, textCoords[0], textCoords[1], //bottom left
		0.0f, 1.0f, 0.0f, textCoords[2], textCoords[3],  //bottom right
		0.0f, 0.0f, 0.0f, textCoords[4], textCoords[5],   //top right
		1.0f, 0.0f, 0.0f, textCoords[6], textCoords[7],  //top left
	};
//	vertices->clear();
	for (int i = 0; i < 20; i=i+5) {
		Vertex &v = vertices->at(i/5);
		v.Position = glm::vec3(*(verts + i), *(verts + i + 1), *(verts + i + 2));
		v.TexCoords = glm::vec2(*(verts + i + 3), *(verts + i + 4));
//		vertices->emplace_back(v);
	}
}

void Billboard::reloadData(vector<Vertex> *vertices, glm::vec3 origin){
	glm::vec3 billcam = glm::vec3(cameraDetails->getPosition().x - origin.x,
		cameraDetails->getPosition().y - origin.y,
		cameraDetails->getPosition().z - origin.z);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 right = glm::normalize(glm::cross(up, billcam));
	right.x *= this->ancho / 2.0;
	right.z *= this->ancho / 2.0;

	float verts[] = {
		// positions        // texture coords
		origin.x + right.x, origin.y       , origin.z + right.z, textCoords[0], textCoords[1], //bottom left
		origin.x - right.x, origin.y       , origin.z - right.z, textCoords[2], textCoords[3], //bottom right

		origin.x - right.x, origin.y + alto, origin.z - right.z, textCoords[4], textCoords[5], //top right
		origin.x + right.x, origin.y + alto, origin.z + right.z, textCoords[6], textCoords[7], //top left
	};
//	vertices->clear();
	for (int i = 0; i < 20; i = i + 5) {
		Vertex &v = vertices->at(i/5);
		v.Position = glm::vec3(*(verts + i), *(verts + i + 1), *(verts + i + 2));
		v.TexCoords = glm::vec2(*(verts + i + 3), *(verts + i + 4));
//		vertices->emplace_back(v);
	}
//	buildKDtree();
}

Billboard::Billboard(int glTextura, WCHAR textura[], float x, float y, float z, Camera* camera) {
    long tLength = wcslen((const wchar_t*)textura);
	char stext[1024];
//    std::string stext(texto.begin(), texto.end());
	ModelAttributes attr{0};
	this->getModelAttributes()->push_back(attr);
	wcstombs_s(NULL, stext, 1024, (wchar_t*)textura, 1024);
	Texture t;
	t.id = glTextura;
	strcpy_s(t.type, 255, "texture_diffuse");
	strcpy_s(t.path, 1024, stext);
	initBillboard(t, ancho, alto, x, y, z, camera, GL_DYNAMIC_DRAW);
}

Billboard::Billboard(WCHAR textura[], float ancho, float alto, float x, float y, float z, Camera* camera) {
	unsigned int texturaB;
	bool alpha = true;
	char stext[1024];
//    std::string stext(texto.begin(), texto.end());
	ModelAttributes attr{0};
	this->getModelAttributes()->push_back(attr);
	wcstombs_s(NULL, stext, 1024, (wchar_t*)textura, 1024);
	texturaB = TextureFromFile(stext, this->directory, false, true, &alpha);
	Texture t;
	t.id = texturaB;
	strcpy_s(t.type, 255, "texture_diffuse");
	strcpy_s(t.type, 1024, stext);
	initBillboard(t, ancho, alto, x, y, z, camera, GL_DYNAMIC_DRAW);
}

void Billboard::initBillboard(Texture &texture, float ancho, float alto, float x, float y, float z, Camera* camera,  int VBOGLDrawType, int EBOGLDrawType){
	cameraDetails = camera;
	vector<unsigned int> indices;
	vector<Vertex>	vertices;
	vector<Texture>	textures;

	this->alto = alto;
	this->ancho= ancho;
//		this->ancho = ancho * 2;
//		this->alto = alto * 2;
	glm::vec3 origin = glm::vec3(x, y, z);
	setTranslate(&origin);
	textures.emplace_back(texture);
	vertices.reserve(4);
	for (int i = 0; i < 4; i++)
		vertices.emplace_back();
	reloadData(&vertices);
	indices.reserve(6);
	indices = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	gpuDemo = NULL;
	meshes.emplace_back(new Mesh(vertices, indices, textures, VBOGLDrawType, EBOGLDrawType));
	textures_loaded.emplace_back(&this->meshes[0]->textures.data()[0]);
}

Billboard::~Billboard(){
	//nos aseguramos de disponer de los recursos previamente reservados
}

// Usa el shader default para poder imprimir el billboard
void Billboard::Draw() {
	reloadData(&(meshes[0]->vertices), *getTranslate());
	if (gpuDemo == NULL) {
		// build and compile our shader zprogram
		// ------------------------------------
		gpuDemo = new Shader("shaders/billboard.vs", "shaders/billboard.fs");
//		gpuDemo = new Shader("shaders/models/1.model_material_loading.vs", "shaders/models/1.model_material_loading.fs");
		setDefaultShader(true);
	}
	if (getDefaultShader()) {
		gpuDemo->use();
		Model::prepShader(*gpuDemo, (*this->getModelAttributes())[0]);
		prepShader(*gpuDemo);
		gpuDemo->setInt("texture_diffuse1", 0);
		Draw(*gpuDemo);
		gpuDemo->desuse();
	} else Draw(*gpuDemo);
}

void Billboard::Draw(Shader &shader) {
//	glDisable(GL_DEPTH_TEST);
	Model::Draw(shader,0);
//	glEnable(GL_DEPTH_TEST);
}

void Billboard::prepShader(Shader& shader) {
	glm::mat4 projection = cameraDetails->getProjection();
	glm::mat4 view = cameraDetails->getView();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f)); // translate it down so it's at the center of the scene
//	model = glm::scale(model, glm::vec3(3.0f,3.0f,3.0f));

	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);
}

void Billboard::setTextureCoords(float *tCoords){
	for (int i = 0; i < 8; i++)
		textCoords[i] = tCoords[i];
}
