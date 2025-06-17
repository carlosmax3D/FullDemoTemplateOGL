#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Utilities.h"
#ifdef __linux__
#include <locale>
#include <codecvt>
#include <string>
#define _strcmpi(x,y) strcasecmp(x,y)
#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L
#define MB_USERICON                 0x00000080L
#define MB_ICONWARNING              MB_ICONEXCLAMATION
#define MB_ICONERROR                MB_ICONHAND
#define MB_ICONINFORMATION          MB_ICONASTERISK
#define MB_ICONSTOP                 MB_ICONHAND
#endif

std::wstring s2ws(const std::string& s) {
#ifdef __linux__
    std::wstring wideString = 
        std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(s);
    return wideString;
#else
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
#endif
}

LOGGER::LOG::LOG() {
	name = "output";
	WINDOW = NULL;
}
LOGGER::LOG::LOG(std::string filename) {
	name = filename;
	WINDOW = NULL;
}
LOGGER::LOG::LOG(std::string filename, void* hwnd) {
	name = filename;
	WINDOW = hwnd;
}
std::string LOGGER::LOG::getLogger() {
	return this->name;
}
void LOGGER::LOG::exclamation(const std::string log) {
	info(log.c_str(), "Exclamation");
}
void LOGGER::LOG::exclamation(const std::string log, const std::string title) {
	processLog(log.c_str(), title.c_str(), "EXCL", MB_ICONEXCLAMATION);
}
void LOGGER::LOG::exclamation(const char* log) {
	processLog(log, "Exclamation", "EXCL", MB_ICONEXCLAMATION);
}
void LOGGER::LOG::info(const std::string log) {
	info(log.c_str(), "Information");
}
void LOGGER::LOG::info(const std::string log, const std::string title) {
	processLog(log.c_str(), title.c_str(), "INFO", MB_ICONINFORMATION);
}
void LOGGER::LOG::info(const char* log) {
	processLog(log, "Information", "INFO", MB_ICONINFORMATION);
}
void LOGGER::LOG::warning(const std::string log) {
	warning(log.c_str(), "Warning");
}
void LOGGER::LOG::warning(const std::string log, const std::string title) {
	processLog(log.c_str(), title.c_str(), "WARN", MB_ICONINFORMATION);
}
void LOGGER::LOG::warning(const char* log) {
	processLog(log, "Warning", "WARN", MB_ICONINFORMATION);
}
void LOGGER::LOG::error(const std::string log) {
	error(log.c_str(), "Error");
}
void LOGGER::LOG::error(const std::string log, const std::string title) {
	processLog(log.c_str(), title.c_str(), "ERROR", MB_ICONERROR);
}
void LOGGER::LOG::error(const char* log) {
	processLog(log, "Error", " ERR", MB_ICONERROR);
}
void LOGGER::LOG::question(const std::string log) {
	question(log.c_str(), "Question");
}
void LOGGER::LOG::question(const std::string log, const std::string title) {
	processLog(log.c_str(), title.c_str(), "QUESTION", MB_ICONQUESTION);
}
void LOGGER::LOG::question(const char* log) {
	processLog(log, "Question", "QSTO", MB_ICONQUESTION);
}
void LOGGER::LOG::processLog(const char* log, const char* title, const char* type, unsigned int MB_TYPE) {
#ifdef DEBUGFILE
	std::string filename(this->name);
	filename.append(".log");
	std::ofstream f(filename, std::ios::app);
	if (f.is_open()) {
		f << type << "::" << title << ":: " << log << std::endl;
		f.close();
	}
#endif
	bool loggerMB = false;
#ifdef SHOWLOGGERMB
	loggerMB = true;
#endif
	if (loggerMB) {
		std::string slog(log), stitle(title);
#ifdef _WIN32 
		std::wstring wlog = s2ws(slog), wtitle = s2ws(stitle);
		const wchar_t* buf = wlog.c_str();
		const wchar_t* bufT = wtitle.c_str();
		HWND* window = (HWND*)(this->getWindow() == NULL ? LOGGER::LOGS::WINDOW : this->getWindow());
		// Muestra el messagebox del mensaje de log
		MessageBox(*window, buf, bufT, MB_TYPE);
		// Envia un mensaje a la ventana para que reinice los botones del teclado
		SendMessage(*window, WM_COMMAND, 9999, 0);
#elif __linux__
		std::string command = "xmessage -center -title \"" + stitle + "\" \"" + slog + "\"";
		system(command.c_str());
#else
		std::string command = "/usr/bin/osascript -e \"display dialog \\\"" + slog + "\\\"\"";
		system(command.c_str());
#endif
	}
#ifndef _WIN32 
	std::cout << title << ": " << log << std::endl;
#endif
}
void LOGGER::LOG::setWindow(void* hwnd) {
	LOGGER::LOGS::WINDOW = hwnd;
	this->WINDOW = hwnd;
}
void* LOGGER::LOG::getWindow() {
	return this->WINDOW;
}

LOGGER::LOG LOGGER::LOGS::getLOGGER() {
	return getLOGGER("output");
}
LOGGER::LOG LOGGER::LOGS::getLOGGER(std::string filename) {
	if (log.size() > 0) {
		for (LOG l : log) {
			if (_strcmpi(l.getLogger().c_str(), filename.c_str()) == 0)
				return l;
		}
	}
	LOG l(filename, LOGGER::LOGS::WINDOW);
	log.push_back(l);
	return l;
}

std::vector<LOGGER::LOG> LOGGER::LOGS::log;
void* LOGGER::LOGS::WINDOW = NULL;

unsigned int GetSizeOfType(unsigned int type) {
		//This function returns the size of a single element of this type in bytes
		switch (type)
		{
		case GL_FLOAT:          return 4;
		case GL_UNSIGNED_INT:   return 4;
		case GL_UNSIGNED_BYTE:  return 1;
		}
		assert(false);
		return 0;
}

// Global Variables:
struct Vertex;
struct BoneInfo;
struct Texture;
struct GameTime;
struct ModelAttributes;
struct ModelCollider;
struct GameActions;
GameActions::~GameActions() {
	if (angle != NULL) delete angle;
	if (pitch != NULL) delete pitch;
	if (zoom != NULL) delete zoom;
	if (playerZoom != NULL) delete playerZoom;
}
void GameActions::setAngle(float value) {
	if (angle == NULL) angle = new float;
	*angle = value;
}
void GameActions::setPitch(float value) {
	if (pitch == NULL) pitch = new float;
	*pitch = value;
}
void GameActions::setPlayerZoom(float value) {
	if (playerZoom == NULL) playerZoom = new float;
	*playerZoom = value;
}
float* GameActions::getPlayerZoom() {
	return playerZoom;
}
void GameActions::setZoom(float value) {
	if (zoom == NULL) zoom = new float;
	*zoom = value;
}
float* GameActions::getZoom() {
	return zoom;
}
float* GameActions::getPitch() {
	return pitch;
}
float* GameActions::getAngle() {
	return angle;
}
Vertex::Vertex() {}
Vertex::Vertex(glm::vec3 pos, glm::vec2 texCoord, glm::vec3 normal, glm::vec3 color) {
	this->Position = pos;
	this->TexCoords = texCoord;
	this->Normal = normal;
	this->Tangent = color;
	this->Bitangent = color;
}

#ifdef _WIN32 
struct UTILITIES_OGL::ImageDetails;
struct UTILITIES_OGL::Vertices;
struct UTILITIES_OGL::Maya;
struct KeyFrame;
struct AssimpNodeData;
#endif
float UTILITIES_OGL::sinLUT[LUT_SIZE];

glm::mat4 UTILITIES_OGL::aiMatrix4x4ToGlm(aiMatrix4x4& from) {
    glm::mat4 to;
    to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
    to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
    to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
    to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;
    return to;
}

void UTILITIES_OGL::calculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices){
    // Reset normals to zero using memset for efficiency
    for (Vertex& vertex : vertices) {
        vertex.Normal = glm::vec3(0.0f);
    }

    // Calculate normals for each triangle
    size_t numTriangles = indices.size() / 3;
    for (size_t i = 0; i < numTriangles; i++) {
        unsigned int idx0 = indices[i * 3];
        unsigned int idx1 = indices[i * 3 + 1];
        unsigned int idx2 = indices[i * 3 + 2];

        // Validate indices
        if (idx0 >= vertices.size() || idx1 >= vertices.size() || idx2 >= vertices.size()) {
            std::cerr << "Invalid index: " << idx0 << ", " << idx1 << ", " << idx2 << " - Max: " << vertices.size() << std::endl;
            continue;
        }

        // Load vertex positions
        const glm::vec3& v0 = vertices[idx0].Position;
        const glm::vec3& v1 = vertices[idx1].Position;
        const glm::vec3& v2 = vertices[idx2].Position;

        // Compute normal using cross product
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        // Accumulate normals
        vertices[idx0].Normal += normal;
        vertices[idx1].Normal += normal;
        vertices[idx2].Normal += normal;
    }

    // Normalize all vertex normals
    for (Vertex& vertex : vertices) {
        if (glm::length(vertex.Normal) > 1e-6f) { // Prevent NaN issues
            vertex.Normal = glm::normalize(vertex.Normal);
        }
    }
}

//generamos las normales a traves de punteros del vector, es una forma comun de manejarlos
glm::vec3 UTILITIES_OGL::genNormal(float* v1, float* v2, float* v3) {
	glm::vec3 vec1, vec2;

	vec1.x = *v2 - *v1;
	vec1.y = *(v2 + 1) - *(v1 + 1);
	vec1.z = *(v2 + 2) - *(v1 + 2);

	vec2.x = *v3 - *v1;
	vec2.y = *(v3 + 1) - *(v1 + 1);
	vec2.z = *(v3 + 2) - *(v1 + 2);

	return glm::cross(vec1, vec2);
}

void UTILITIES_OGL::sumaNormal(float* v1, float* v2){
	*v1 += *v2;
	*(v1 + 1) += *(v2 + 1);
	*(v1 + 2) += *(v2 + 2);
}

void UTILITIES_OGL::normaliza(float* v1){
	float magnitud = sqrt((*v1) * (*v1) + (*(v1 + 1)) * (*(v1 + 1)) + (*(v1 + 2)) * (*(v1 + 2)));
	*v1 /= magnitud;
	*(v1 + 1) /= magnitud;
	*(v1 + 2) /= magnitud;
}

void UTILITIES_OGL::vectoresEsfera(Maya esfera, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, unsigned int iv, unsigned int ii) {
	for (unsigned int i = 0; i < iv || i < ii; i++) {
		if (i < iv) {
			Vertex v;
			v.Position.x = esfera.maya[i].Posx;
			v.Position.y = esfera.maya[i].Posy;
			v.Position.z = esfera.maya[i].Posz;
			v.Normal.x = esfera.maya[i].Normx;
			v.Normal.y = esfera.maya[i].Normy;
			v.Normal.z = esfera.maya[i].Normz;
			v.TexCoords.x = esfera.maya[i].u;
			v.TexCoords.y = esfera.maya[i].v;
			vertices.emplace_back(v);
		}
		if (i < ii)
			indices.emplace_back(esfera.indices[i]);

	}
}

//generamos los vertices a traves de coordenadas esfericas
//conocimiento adquirido en la materia de Fund de las Graficas Computacionales
UTILITIES_OGL::Maya UTILITIES_OGL::Esfera(int stacks, int slices, float radio, float inicio, float final) {
	//Cargamos la estructura con los espacios de memoria necesarios
	Vertices* verticesxyzSD = new Vertices[stacks * slices * 3];
	unsigned int* indices = new unsigned int[(stacks-1) * (slices-1) * 6]{0};
	//generamos un objeto para poder transportar los punteros
	Maya salida;
	//a darle que es mole de olla!
	for (unsigned int i = 0; i < slices; i++)
	{
		for (unsigned int j = 0; j < stacks; j++)
		{
			int indice = (i * stacks + j);
			verticesxyzSD[indice].Posx = radio * cos(((double)j / (stacks - 1)) * (M_PI * (final - inicio)) + M_PI * inicio - M_PI / 2.0) *
				cos(2.0 * M_PI * (double)i / (slices - 1));
			verticesxyzSD[indice].Posy = radio * sin(((double)j / (stacks - 1)) * (M_PI * (final - inicio)) + M_PI * inicio - M_PI / 2.0);
			verticesxyzSD[indice].Posz = radio * cos(((double)j / (stacks - 1)) * (M_PI * (final - inicio)) + M_PI * inicio - M_PI / 2.0) *
				sin(2.0 * M_PI * (double)i / (slices - 1));

			verticesxyzSD[indice].Normx = cos(((double)j / (stacks - 1)) * (M_PI * (final - inicio)) + M_PI * inicio - M_PI / 2.0) *
				cos(2.0 * M_PI * (double)i / (slices - 1));
			verticesxyzSD[indice].Normy = sin(((double)j / (stacks - 1)) * (M_PI * (final - inicio)) + M_PI * inicio - M_PI / 2.0);
			verticesxyzSD[indice].Normz = cos(((double)j / (stacks - 1)) * (M_PI * (final - inicio)) + M_PI * inicio - M_PI / 2.0) *
				sin(2.0 * M_PI * (double)i / (slices - 1));

			verticesxyzSD[indice].u = (float)1 * (1 - (float)i / (stacks - 1));
			verticesxyzSD[indice].v = (float)1 * (1 - (float)j / (slices - 1));
		}
	}

	//ahora la parte mas importante de crear vertices es el algoritmo para unirlos, en este caso sustituiremos
	//a un algoritmo con un un grupo de indices
	unsigned int indice = 0;
	for (unsigned int i = 0; i < slices - 1; i++)
	{
		for (unsigned int j = 0; j < stacks - 1; j++)
		{
			indices[indice++] = i * stacks + j;
			indices[indice++] = (i + 1) * stacks + j + 1;
			indices[indice++] = i * stacks + j + 1;

			indices[indice++] = i * stacks + j;
			indices[indice++] = (i + 1) * stacks + j;
			indices[indice++] = (i + 1) * stacks + j + 1;
		}
	}

	//una vez generados los damos a conocer a traves del objeto "salida"
	salida.maya = verticesxyzSD;
	salida.indices = indices;

	return salida;
}

UTILITIES_OGL::Maya UTILITIES_OGL::Plano(int vertx, int vertz, float anchof, float profz){
	//Cargamos la estructura con los espacios de memoria necesarios
	Vertices* verticesxyzSD = new Vertices[vertx * vertz * 3];
	unsigned int* indices = new unsigned int[(vertx-1) * (vertz-1) * 6]{0};

	//es la separacion entre vertices, se le resta 1 para que el lado correcto
	//imagine que el ancho es de 10 y tiene 10 vertices, entonces le daria un deltax
	//de 1, si los vertices van de 0 a 9 entonces la posicion del ultimo vertice
	//seria 9, si le divide entre vertx -1 le dara 1.1111, y el ultimo vertice sera 10
	float deltax = anchof / (vertx - 1);
	float deltaz = profz / (vertz - 1);

	//crea los vertices
	for (unsigned int z = 0; z < vertz; z++)
	{
		for (unsigned int x = 0; x < vertx; x++)
		{
			verticesxyzSD[z * vertx + x].Posx = (float)x * deltax;
			verticesxyzSD[z * vertx + x].Posy = 0.0;
			verticesxyzSD[z * vertx + x].Posz = (float)z * deltaz;

			//carga las normales con cero
			verticesxyzSD[z * vertx + x].Normx = 0.0;
			verticesxyzSD[z * vertx + x].Normy = 0.0;
			verticesxyzSD[z * vertx + x].Normz = 0.0;
		}
	}

	//calcula los uv's
	for (unsigned int z = 0; z < vertz; z++)
	{
		for (unsigned int x = 0; x < vertx; x++)
		{
			verticesxyzSD[z * vertx + x].u = (float)x / (vertx - 1);
			verticesxyzSD[z * vertx + x].v = (float)z / (vertz - 1);
		}
	}

	glm::vec3 aux;
	//crea las normales
	for (unsigned int z = 0; z < (vertz - 1); z++)
	{
		for (unsigned int x = 0; x < (vertx - 1); x++)
		{
			aux = genNormal(&verticesxyzSD[z * vertx + x].Posx, &verticesxyzSD[(z + 1) * vertx + (x + 1)].Posx,
				&verticesxyzSD[z * vertx + (x + 1)].Posx);

			sumaNormal(&verticesxyzSD[z * vertx + x].Normx, &aux.x);
			sumaNormal(&verticesxyzSD[(z + 1) * vertx + (x + 1)].Normx, &aux.x);
			sumaNormal(&verticesxyzSD[z * vertx + (x + 1)].Normx, &aux.x);

			aux = genNormal(&verticesxyzSD[z * vertx + x].Posx, &verticesxyzSD[(z + 1) * vertx + x].Posx,
				&verticesxyzSD[(z + 1) * vertx + (x + 1)].Posx);

			sumaNormal(&verticesxyzSD[z * vertx + x].Normx, &aux.x);
			sumaNormal(&verticesxyzSD[(z + 1) * vertx + x].Normx, &aux.x);
			sumaNormal(&verticesxyzSD[(z + 1) * vertx + (x + 1)].Normx, &aux.x);
		}
	}

	//Normaliza las normales
	for (unsigned int z = 0; z < vertz; z++)
	{
		for (unsigned int x = 0; x < vertx; x++)
		{
			normaliza(&verticesxyzSD[z * vertx + x].Normx);
		}
	}

	//ahora la parte mas importante de crear vertices es el algoritmo para unirlos, en este caso sustituiremos
	//a un algoritmo con un un grupo de indices
	unsigned int indice = 0;
	for (unsigned int i = 0; i < vertz - 1; i++)
	{
		for (unsigned int j = 0; j < vertx - 1; j++)
		{
			indices[indice++] = i * vertz + j;
			indices[indice++] = (i + 1) * vertz + j + 1;
			indices[indice++] = i * vertz + j + 1;

			indices[indice++] = i * vertz + j;
			indices[indice++] = (i + 1) * vertz + j;
			indices[indice++] = (i + 1) * vertz + j + 1;
		}
	}

	//generamos un objeto para poder transportar los punteros

	Maya salida;

	salida.maya = verticesxyzSD;
	salida.indices = indices;

	return salida;
}

UTILITIES_OGL::Maya UTILITIES_OGL::Plano(int vertx, int vertz, float anchof, float profz, unsigned char* altura, int nrComponents, float tile) {
	//Cargamos la estructura con los espacios de memoria necesarios
	Vertices* verticesxyzSD = new Vertices[vertx * vertz * 3];
	unsigned int* indices = new unsigned int[(vertx-1) * (vertz-1) * 6]{0};

	//es la separacion entre vertices, se le resta 1 para que el lado correcto
	//imagine que el ancho es de 10 y tiene 10 vertices, entonces le daria un deltax
	//de 1, si los vertices van de 0 a 9 entonces la posicion del ultimo vertice
	//seria 9, si le divide entre vertx -1 le dara 1.1111, y el ultimo vertice sera 10
	float deltax = anchof / (vertx - 1);
	float deltaz = profz / (vertz - 1);

	//crea los vertices
	for (unsigned int z = 0; z < vertz; z++)
	{
		for (unsigned int x = 0; x < vertx; x++)
		{
			verticesxyzSD[z * vertx + x].Posx = (float)x * deltax - anchof / 2.0;
			verticesxyzSD[z * vertx + x].Posy = (float)altura[z * vertx * nrComponents + x * nrComponents] / 10.0; // nrComponents -> 4
			verticesxyzSD[z * vertx + x].Posz = (float)z * deltaz - profz / 2.0;

			//carga las normales con cero
			verticesxyzSD[z * vertx + x].Normx = 0.0;
			verticesxyzSD[z * vertx + x].Normy = 1.0;
			verticesxyzSD[z * vertx + x].Normz = 0.0;
		}
	}

	//calcula los uv's
	for (unsigned int z = 0; z < vertz; z++)
	{
		for (unsigned int x = 0; x < vertx; x++)
		{
			verticesxyzSD[z * vertx + x].u = (float)(x * tile) / (vertx - 1);
			verticesxyzSD[z * vertx + x].v = (float)(z * tile) / (vertz - 1);
		}
	}

	glm::vec3 aux;
	//crea las normales
	for (unsigned int z = 0; z < (vertz - 1); z++)
	{
		for (unsigned int x = 0; x < (vertx - 1); x++)
		{
			aux = genNormal(&verticesxyzSD[z * vertx + x].Posx, &verticesxyzSD[z * vertx + (x + 1)].Posx,
				&verticesxyzSD[(z + 1) * vertx + (x + 1)].Posx);

			sumaNormal(&verticesxyzSD[z * vertx + x].Normx, &aux.x);
			sumaNormal(&verticesxyzSD[(z + 1) * vertx + (x + 1)].Normx, &aux.x);
			sumaNormal(&verticesxyzSD[z * vertx + (x + 1)].Normx, &aux.x);

			aux = genNormal(&verticesxyzSD[z * vertx + x].Posx, &verticesxyzSD[(z + 1) * vertx + x + 1].Posx,
				&verticesxyzSD[(z + 1) * vertx + x].Posx);

			sumaNormal(&verticesxyzSD[z * vertx + x].Normx, &aux.x);
			sumaNormal(&verticesxyzSD[(z + 1) * vertx + x].Normx, &aux.x);
			sumaNormal(&verticesxyzSD[(z + 1) * vertx + (x + 1)].Normx, &aux.x);
		}
	}

	//Normaliza las normales
	for (unsigned int z = 0; z < vertz; z++)
	{
		for (unsigned int x = 0; x < vertx; x++)
		{
			normaliza(&verticesxyzSD[z * vertx + x].Normx);

		}
	}


	//ahora la parte mas importante de crear vertices es el algoritmo para unirlos, en este caso sustituiremos
	//a un algoritmo con un un grupo de indices
	unsigned int indice = 0;
	for (unsigned int i = 0; i < vertz - 1; i++)
	{
		for (unsigned int j = 0; j < vertx - 1; j++)
		{
			indices[indice++] = i * vertz + j;
			indices[indice++] = (i + 1) * vertz + j + 1;
			indices[indice++] = i * vertz + j + 1;


			indices[indice++] = (i + 1) * vertz + j;
			indices[indice++] = (i + 1) * vertz + j + 1; 
			indices[indice++] = i * vertz + j;
		}
	}

	//generamos un objeto para poder transportar los punteros

	Maya salida;

	salida.maya = verticesxyzSD;
	salida.indices = indices;

	return salida;
}

unsigned char* loadFile(char const* fileName, int* x, int* y, int* comp, int req_comp, bool rotateX, bool rotateY) {
	unsigned char* data = NULL, * tmp = NULL;
	const char *filename = fileName;
#ifdef __linux__ 
	if (FreeImage_IsPluginEnabled(FIF_BMP) == -1 || FreeImage_IsPluginEnabled(FIF_BMP) == FALSE )
		FreeImage_Initialise();
	std::string sfilename(fileName);
	for (int i = 0; i < sfilename.length(); i++)
		if (sfilename[i] == '\\')
			sfilename[i] = '/';
	filename = sfilename.c_str();
#endif
	FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(filename, 0);
	FIBITMAP* imagen = FreeImage_Load(formato, filename);
	if (imagen != NULL) {
		if (rotateY) FreeImage_FlipVertical(imagen);
		if (rotateX) FreeImage_FlipHorizontal(imagen);
		*x = FreeImage_GetWidth(imagen);
		*y = FreeImage_GetHeight(imagen);
		// calculate the number of bytes per pixel
		*comp = FreeImage_GetLine(imagen) / FreeImage_GetWidth(imagen);
		// calculate the number of samples per pixel
		unsigned samples = *comp / sizeof(FreeImage_GetImageType(imagen));
		tmp = new unsigned char[(*x) * (*y) * (*comp)];
		memcpy(tmp, (unsigned char*)FreeImage_GetBits(imagen), (*x) * (*y) * (*comp));
		if ((*comp) == 2)
			for (int j = 0; j < (*x) * (*y); j++)  {
				unsigned char c = tmp[j * (*comp) + 0];
				tmp[j * (*comp) + 0] = tmp[j * (*comp) + 1];
				tmp[j * (*comp) + 1] = c;
			}
		if ((*comp) >= 3)
			for (int j = 0; j < (*x) * (*y); j++) {
				unsigned char c = tmp[j * (*comp) + 0];
				tmp[j * (*comp) + 0] = tmp[j * (*comp) + 2];
				tmp[j * (*comp) + 2] = c;
			}
		FreeImage_Unload(imagen);
	}
	else {
		if (rotateY) stbi_set_flip_vertically_on_load(false);
		data = stbi_load(filename, x, y, comp, 0);
		if (data) {
			tmp = new unsigned char[(*x) * (*y) * (*comp)];
			memcpy(tmp, data, (*x) * (*y) * (*comp));
			stbi_image_free(data);
		}
		stbi_set_flip_vertically_on_load(false);
	}
	return tmp;
}

unsigned char* loadMemory(const aiTexture *tex, int* x, int* y, int* comp, int req_comp, bool rotateX, bool rotateY) {
	unsigned char* data = NULL, * tmp = NULL;
#ifdef __linux__ 
	if (FreeImage_IsPluginEnabled(FIF_BMP) == -1 || FreeImage_IsPluginEnabled(FIF_BMP) == FALSE )
		FreeImage_Initialise();
#endif
    // Create a memory stream from the data buffer
    FIMEMORY* memStream = FreeImage_OpenMemory((BYTE*)tex->pcData, tex->mWidth);
    if (!memStream) {
        ERRORL("Failed to create memory stream for embbebed texture.", "Error at loading model");
        return data;
    }
	FREE_IMAGE_FORMAT formato = tex->mHeight == 0 ? FreeImage_GetFileTypeFromMemory(memStream) : FIF_UNKNOWN;
	formato = formato == FIF_UNKNOWN ? FreeImage_GetFIFFromFormat(tex->achFormatHint) : formato;
	FIBITMAP* imagen = formato == FIF_UNKNOWN ? NULL : FreeImage_LoadFromMemory(formato, memStream);
	if (imagen != NULL) {
		if (rotateY) FreeImage_FlipVertical(imagen);
		if (rotateX) FreeImage_FlipHorizontal(imagen);
		*x = FreeImage_GetWidth(imagen);
		*y = FreeImage_GetHeight(imagen);
		// calculate the number of bytes per pixel
		*comp = FreeImage_GetLine(imagen) / FreeImage_GetWidth(imagen);
		// calculate the number of samples per pixel
		unsigned samples = *comp / sizeof(FreeImage_GetImageType(imagen));
		tmp = new unsigned char[(*x) * (*y) * (*comp)];
		memcpy(tmp, (unsigned char*)FreeImage_GetBits(imagen), (*x) * (*y) * (*comp));
		for (int j = 0; j < (*x) * (*y) && (*comp) >= 3; j++) {
			unsigned char c = tmp[j * (*comp) + 0];
			tmp[j * (*comp) + 0] = tmp[j * (*comp) + 2];
			//tmp[j * (*comp) + 1] = tmp[j * (*comp) + 1];
			tmp[j * (*comp) + 2] = c;
			//tmp[j * (*comp) + 3] = tmp[j * (*comp) + 3];
		}
		FreeImage_Unload(imagen);
	    FreeImage_CloseMemory(memStream);
	}
	else {
		if (rotateY) stbi_set_flip_vertically_on_load(false);
		data = stbi_load_from_memory((const stbi_uc*)tex->pcData, tex->mWidth, x, y, comp, 0);
		if (data) {
			tmp = new unsigned char[(*x) * (*y) * (*comp)];
			memcpy(tmp, data, (*x) * (*y) * (*comp));
			stbi_image_free(data);
		}
		stbi_set_flip_vertically_on_load(false);
	}
	return tmp;
}

unsigned int TextureFromMemory(const aiTexture *texture, bool rotateX, bool rotateY, bool *alpha, struct UTILITIES_OGL::ImageDetails* img) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = loadMemory(texture, &width, &height, &nrComponents, 0, rotateX, rotateY);
	GLenum format = GL_RGBA;
	if (data) {
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3) {
			format = GL_RGB;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		}
		else if (nrComponents == 4) {
			format = GL_RGBA;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		}
		if (alpha != NULL && *alpha) {
			format = GL_RGBA;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		}
		if (format == GL_RGBA && alpha != NULL)
			*alpha = true;
		glBindTexture(GL_TEXTURE_2D, textureID);
		//        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);   //Requires GL 1.4. Removed from GL 3.1 and above.
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		delete[] data;
	}
	else {
		std::string name = texture->mFilename.C_Str();
		INFO("Texture failed to load texture : " + name, "ERROR LOAD OBJ");
	}
	if (img != NULL) {
		img->format = format;
		img->height = height;
		img->nrComponents = nrComponents;
		img->width = width;
	}
	return textureID;
}

unsigned int TextureFromFile(const char* path, const std::string& directory, bool rotateX, bool rotateY, bool *alpha, struct UTILITIES_OGL::ImageDetails* img) {
	std::string filename = std::string(path);
	if (!directory.empty())
		filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = loadFile(filename.c_str(), &width, &height, &nrComponents, 0, rotateX, rotateY);
	GLenum format = GL_RGBA;
	if (data) {
		if (nrComponents == 1 || nrComponents == 2){
			format = nrComponents == 2 ? GL_RG : GL_RED;
			if (nrComponents == 2)
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		} else if (nrComponents == 3) {
			format = GL_RGB;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		}
		else if (nrComponents == 4) {
			format = GL_RGBA;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		}
		if (alpha != NULL && *alpha) {
			format = GL_RGBA;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		}
		if (format == GL_RGBA && alpha != NULL)
			*alpha = true;
		glBindTexture(GL_TEXTURE_2D, textureID);
		//        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);   //Requires GL 1.4. Removed from GL 3.1 and above.
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		delete[] data;
	}
	else {
		INFO("Texture failed to load at path: " + filename, "ERROR LOAD OBJ");
	}
	if (img != NULL) {
		img->format = format;
		img->height = height;
		img->nrComponents = nrComponents;
		img->width = width;
	}
	return textureID;
}

double get_nanos() {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

// Comparator function
bool compareKeyframes(UTILITIES_OGL::KeyFrame& A, UTILITIES_OGL::KeyFrame& b) {
    return A.timeStamp < b.timeStamp;
}

glm::vec3 lerpVec3(const glm::vec3& a, const glm::vec3& b, float t) {
    return a + t * (b - a); // More numerically stable than glm::mix
}

void ModelAttributes::setTranslate(glm::vec3* translate) {
    if (translate == NULL) {
        this->translate = glm::vec3(0);
        this->hasTranslate = false;
    }
    else {
        this->translate = *translate;
        this->hasTranslate = true;
    }
}
void ModelAttributes::setNextTranslate(glm::vec3* translate) {
    if (translate == NULL) {
        this->nextTranslate = glm::vec3(0);
    } else {
        this->nextTranslate = *translate;
    }
}
void ModelAttributes::setScale(glm::vec3* scale) {
    if (scale == NULL) {
        this->scale = glm::vec3(0);
        this->hasScale = false;
    }
    else {
        this->scale = *scale;
        this->hasScale = true;
    }
}

void ModelAttributes::setRotX(float rotationAngle) {
    this->rotX = rotationAngle;
    this->rotation.x = rotationAngle == 0 ? 0 : 1;
}
void ModelAttributes::setRotY(float rotationAngle) {
    this->rotY = rotationAngle;
    this->rotation.y = rotationAngle == 0 ? 0 : 1;
}
void ModelAttributes::setRotZ(float rotationAngle) {
    this->rotZ = rotationAngle;
    this->rotation.z = rotationAngle == 0 ? 0 : 1;
}
void ModelAttributes::setNextRotX(float rotationAngle) {
    this->nextRotX = rotationAngle;
    this->nextRotation.x = rotationAngle == 0 ? 0 : 1;
}
void ModelAttributes::setNextRotY(float rotationAngle) {
    this->nextRotY = rotationAngle;
    this->nextRotation.y = rotationAngle == 0 ? 0 : 1;
}
void ModelAttributes::setNextRotZ(float rotationAngle) {
    this->nextRotZ = rotationAngle;
    this->nextRotation.z = rotationAngle == 0 ? 0 : 1;
}

std::vector<Vertex> init_cube(float x, float y, float z, float width, float height, float depth){
    //Vertex* myVertex = (Vertex*)malloc(sizeof(Vertex) * 24 * 44);
    std::vector<Vertex> myVertex;
    myVertex.reserve(24);
//    Vertex t = Vertex(glm::vec3(-width + x, -height + y, -depth + z), glm::vec2(1, 0), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0));			//yellow
    myVertex.emplace_back(glm::vec3(-width + x, -height + y, -depth + z), glm::vec2(1, 0), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0));			//yellow
//    t = Vertex(glm::vec3(-width + x, height + y, -depth + z), glm::vec2(0, 0), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0));
    myVertex.emplace_back(glm::vec3(-width + x, height + y, -depth + z), glm::vec2(0, 0), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0));
//    t = Vertex(glm::vec3(width + x, height + y, -depth + z), glm::vec2(0, 1), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0));
    myVertex.emplace_back(glm::vec3(width + x, height + y, -depth + z), glm::vec2(0, 1), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0));
//    t = Vertex(glm::vec3(width + x, -height + y, -depth + z), glm::vec2(1, 1), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0));
    myVertex.emplace_back(glm::vec3(width + x, -height + y, -depth + z), glm::vec2(1, 1), glm::vec3(0, 0, -1), glm::vec3(1, 1, 0));

//    t = Vertex(glm::vec3(-width + x, -height + y, depth + z), glm::vec2(1, 0), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));			//white
    myVertex.emplace_back(glm::vec3(-width + x, -height + y, depth + z), glm::vec2(1, 0), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));			//white
//    t = Vertex(glm::vec3(-width + x, height + y, depth + z), glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));
    myVertex.emplace_back(glm::vec3(-width + x, height + y, depth + z), glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));
//    t = Vertex(glm::vec3(width + x, height + y, depth + z), glm::vec2(0, 1), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));
    myVertex.emplace_back(glm::vec3(width + x, height + y, depth + z), glm::vec2(0, 1), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));
//    t = Vertex(glm::vec3(width + x, -height + y, depth + z), glm::vec2(1, 1), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));
    myVertex.emplace_back(glm::vec3(width + x, -height + y, depth + z), glm::vec2(1, 1), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1));

//    t = Vertex(glm::vec3(-width + x, -height + y, -depth + z), glm::vec2(0, 1), glm::vec3(0, -1, 0), glm::vec3(1, 0.5, 0));		//orange
    myVertex.emplace_back(glm::vec3(-width + x, -height + y, -depth + z), glm::vec2(0, 1), glm::vec3(0, -1, 0), glm::vec3(1, 0.5, 0));		//orange
//    t = Vertex(glm::vec3(-width + x, -height + y, depth + z), glm::vec2(1, 1), glm::vec3(0, -1, 0), glm::vec3(1, 0.5, 0));
    myVertex.emplace_back(glm::vec3(-width + x, -height + y, depth + z), glm::vec2(1, 1), glm::vec3(0, -1, 0), glm::vec3(1, 0.5, 0));
//    t = Vertex(glm::vec3(width + x, -height + y, depth + z), glm::vec2(1, 0), glm::vec3(0, -1, 0), glm::vec3(1, 0.5, 0));
    myVertex.emplace_back(glm::vec3(width + x, -height + y, depth + z), glm::vec2(1, 0), glm::vec3(0, -1, 0), glm::vec3(1, 0.5, 0));
//    t = Vertex(glm::vec3(width + x, -height + y, -depth + z), glm::vec2(0, 0), glm::vec3(0, -1, 0), glm::vec3(1, 0.5, 0));
    myVertex.emplace_back(glm::vec3(width + x, -height + y, -depth + z), glm::vec2(0, 0), glm::vec3(0, -1, 0), glm::vec3(1, 0.5, 0));

//    t = Vertex(glm::vec3(-width + x, height + y, -depth + z), glm::vec2(0, 1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));			//red
    myVertex.emplace_back(glm::vec3(-width + x, height + y, -depth + z), glm::vec2(0, 1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));			//red
//    t = Vertex(glm::vec3(-width + x, height + y, depth + z), glm::vec2(1, 1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
    myVertex.emplace_back(glm::vec3(-width + x, height + y, depth + z), glm::vec2(1, 1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
//    t = Vertex(glm::vec3(width + x, height + y, depth + z), glm::vec2(1, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
    myVertex.emplace_back(glm::vec3(width + x, height + y, depth + z), glm::vec2(1, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
//    t = Vertex(glm::vec3(width + x, height + y, -depth + z), glm::vec2(0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
    myVertex.emplace_back(glm::vec3(width + x, height + y, -depth + z), glm::vec2(0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));

//    t = Vertex(glm::vec3(-width + x, -height + y, -depth + z), glm::vec2(1, 1), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));			//blue
    myVertex.emplace_back(glm::vec3(-width + x, -height + y, -depth + z), glm::vec2(1, 1), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));			//blue
//    t = Vertex(glm::vec3(-width + x, -height + y, depth + z), glm::vec2(1, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));
    myVertex.emplace_back(glm::vec3(-width + x, -height + y, depth + z), glm::vec2(1, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));
//    t = Vertex(glm::vec3(-width + x, height + y, depth + z), glm::vec2(0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));
    myVertex.emplace_back(glm::vec3(-width + x, height + y, depth + z), glm::vec2(0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));
//    t = Vertex(glm::vec3(-width + x, height + y, -depth + z), glm::vec2(0, 1), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));
    myVertex.emplace_back(glm::vec3(-width + x, height + y, -depth + z), glm::vec2(0, 1), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1));

//    t = Vertex(glm::vec3(width + x, -height + y, -depth + z), glm::vec2(1, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));			//green
    myVertex.emplace_back(glm::vec3(width + x, -height + y, -depth + z), glm::vec2(1, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));			//green
//    t = Vertex(glm::vec3(width + x, -height + y, depth + z), glm::vec2(1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
    myVertex.emplace_back(glm::vec3(width + x, -height + y, depth + z), glm::vec2(1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
//    t = Vertex(glm::vec3(width + x, height + y, depth + z), glm::vec2(0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
    myVertex.emplace_back(glm::vec3(width + x, height + y, depth + z), glm::vec2(0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
//    t = Vertex(glm::vec3(width + x, height + y, -depth + z), glm::vec2(0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
    myVertex.emplace_back(glm::vec3(width + x, height + y, -depth + z), glm::vec2(0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));

    return myVertex;
}
std::vector<unsigned int> getCubeIndex() {
    std::vector<unsigned int> indices;
    int cubeIndexSize = 36;
    unsigned int cubeIndex[] = { 0, 1, 2,
    0, 2, 3,

    6, 5, 4,
    7, 6, 4,

    10, 9, 8,
    11, 10, 8,

    12, 13, 14,
    12, 14, 15,

    16, 17, 18,
    16, 18, 19,

    22, 21, 20,
    23, 22, 20
    };
    indices.reserve(cubeIndexSize);
    for (unsigned int i = 0; i < cubeIndexSize; i++)
        indices.emplace_back(cubeIndex[i]);
    return indices;
}

//void * operator new(size_t size){
//    void * p = malloc(size);
//    return p;
//}