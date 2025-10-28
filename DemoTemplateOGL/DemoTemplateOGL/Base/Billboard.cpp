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
	float textCoords[8] = { 1.0f,1.0f,0.0f,1.0f,0.0f,0.0f,1.0f,0.0f };
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
	float textCoords[8] = { 1.0f,1.0f,0.0f,1.0f,0.0f,0.0f,1.0f,0.0f };
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
//    std::string stext(texto.begin(), texto.end());
	if (this->getModelAttributes()->size() == 0){
		ModelAttributes attr{0};
		this->getModelAttributes()->push_back(attr);
	}
    this->alto = alto;
    this->ancho = ancho;
}

Billboard::Billboard(WCHAR textura[], float ancho, float alto, float x, float y, float z, Camera* camera) {
	unsigned int texturaB;
	bool alpha = true;
	char stext[1024];
//    std::string stext(texto.begin(), texto.end());
	if (this->getModelAttributes()->size() == 0){
		ModelAttributes attr{0};
		this->getModelAttributes()->push_back(attr);
	}
	wcstombs_s(NULL, stext, 1024, (wchar_t*)textura, 1024);
	texturaB = TextureFromFile(stext, this->directory, false, true, &alpha);
	Texture t;
	t.id = texturaB;
	strcpy_s(t.type, 255, "texture_diffuse");
	strcpy_s(t.type, 1024, stext);
	initBillboard(t, ancho, alto, x, y, z, camera, GL_DYNAMIC_DRAW);
}

void Billboard::initBillboard(Texture &texture, float ancho, float alto, float x, float y, float z, Camera* camera, int VBOGLDrawType, int EBOGLDrawType){
    cameraDetails = camera;
    vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };
	vector<Vertex> vertices(4);
    vector<Texture> textures;
    this->alto = alto;
    this->ancho = ancho;
//		this->ancho = ancho * 2;
//		this->alto = alto * 2;
    glm::vec3 origin = glm::vec3(x, y, z);
    setTranslate(&origin);
	vertices[0].Position = glm::vec3(-0.5f, -0.5f, 0.0f);
	vertices[0].TexCoords = glm::vec2(0.0f, 0.0f);
	vertices[1].Position = glm::vec3(0.5f, -0.5f, 0.0f);
	vertices[1].TexCoords = glm::vec2(1.0f, 0.0f);
	vertices[2].Position = glm::vec3(0.5f, 0.5f, 0.0f);
	vertices[2].TexCoords = glm::vec2(1.0f, 1.0f);
	vertices[3].Position = glm::vec3(-0.5f, 0.5f, 0.0f);
	vertices[3].TexCoords = glm::vec2(0.0f, 1.0f);
	textures.emplace_back(texture);
	gpuDemo = NULL;
    meshes.emplace_back(new Mesh(vertices, indices, textures, VBOGLDrawType, EBOGLDrawType));
	textures_loaded.emplace_back(&this->meshes[0]->textures.data()[0]);
	for (Mesh *m : meshes)
		m->modelAttributes = getModelAttributes();
}

Billboard::~Billboard(){
	//nos aseguramos de disponer de los recursos previamente reservados
}

// Usa el shader default para poder imprimir el billboard
void Billboard::Draw() {
	if (gpuDemo == NULL) {
		// build and compile our shader zprogram
		// ------------------------------------
		gpuDemo = new Shader("shaders/billboard.vs", "shaders/billboard.fs");
//		gpuDemo = new Shader("shaders/models/1.model_material_loading.vs", "shaders/models/1.model_material_loading.fs");
		setDefaultShader(true);
	}
	if (getDefaultShader()) {
		gpuDemo->use();
        auto attributes = this->getModelAttributes();
        for (int i = 0 ; i < 1 ; i++){
            Model::prepShader(*gpuDemo, attributes->at(i));
            prepShader(*gpuDemo, i);
            gpuDemo->setInt("texture_diffuse1", 0);
            Model::Draw(*gpuDemo, i);
        }
        gpuDemo->desuse();
	} else Draw(*gpuDemo);
}

void Billboard::Draw(Shader &shader) {
//	glDisable(GL_DEPTH_TEST);
	Model::Draw(shader,0);
//	glEnable(GL_DEPTH_TEST);
}

void Billboard::prepShader(Shader& shader, int idx) {
    glm::mat4 projection = cameraDetails->getProjection();
    glm::mat4 view = cameraDetails->getView();

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    shader.setVec3("billboardPos", *getTranslate(idx));
    shader.setFloat("width", ancho);
    shader.setFloat("height", alto);
    shader.setVec3("cameraPos", cameraDetails->getPosition());
    shader.setVec3("camRight", cameraDetails->getRight());
}

void Billboard::setTextureCoords(float *tCoords){
	auto &vertices = this->meshes.at(0)->vertices;
	for (int i = 0; i < 8; i = i+2){
		vertices.at(i/2).TexCoords.x = tCoords[i];
		vertices.at(i/2).TexCoords.y = tCoords[i+1];
	}
}
