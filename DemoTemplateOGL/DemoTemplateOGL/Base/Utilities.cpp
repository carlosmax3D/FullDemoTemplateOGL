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
#include <ctime>
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
#define sprintf_s(a,b,c,d,e,f,g,h,i) sprintf(a,c,d,e,f,g,h,i)
#endif
#if defined(_WIN32) && defined(ENGINE_DIRECTX)
ID3D11Device* device = NULL;
ID3D11DeviceContext* ctx = NULL;
IDXGISwapChain* swapChain = NULL;
ID3D11RenderTargetView* backBufferRTV = NULL;
ID3D11BlendState* blendState = NULL;
ID3D11SamplerState* samplerState = NULL;
ID3D11SamplerState* samplerAlpha = NULL;
ID3D11SamplerState* samplerFont = NULL;
ID3D11DepthStencilView* depthView = NULL;
ID3D11DepthStencilState* depthState = NULL;
ID3D11RasterizerState* rsBackState = NULL;
ID3D11RasterizerState* rsFrontState = NULL;
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
void formatDate(char* result, tm* timeptr) {
	sprintf_s(result, 26, "%02d/%02d/%04d %.2d:%.2d:%.2d|",
		timeptr->tm_mday, timeptr->tm_mon,
		1900 + timeptr->tm_year, timeptr->tm_hour,
		timeptr->tm_min, timeptr->tm_sec);
}
void LOGGER::LOG::processLog(const char* log, const char* title, const char* type, unsigned int MB_TYPE) {
#ifdef DEBUGFILE
	time_t now = time(nullptr);
#ifdef _WIN32 
	tm timeptr1;
	localtime_s(&timeptr1, &now);
	tm *timeptr = &timeptr1;
#else
	tm *timeptr = localtime(&now);
#endif
	char datetime[26] = { 0 };
	formatDate(datetime, timeptr);
	std::string filename(this->name);
	filename.append(".log");
	std::ofstream f(filename, std::ios::app);
	if (f.is_open()) {
		f << datetime << type << "::" << title << ":: " << log << std::endl;
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

void UTILITIES_OGL::calculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
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

void UTILITIES_OGL::sumaNormal(float* v1, float* v2) {
	*v1 += *v2;
	*(v1 + 1) += *(v2 + 1);
	*(v1 + 2) += *(v2 + 2);
}

void UTILITIES_OGL::normaliza(float* v1) {
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
	unsigned int* indices = new unsigned int[(stacks - 1) * (slices - 1) * 6] {0};
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

UTILITIES_OGL::Maya UTILITIES_OGL::Plano(int vertx, int vertz, float anchof, float profz) {
	//Cargamos la estructura con los espacios de memoria necesarios
	Vertices* verticesxyzSD = new Vertices[vertx * vertz * 3];
	unsigned int* indices = new unsigned int[(vertx - 1) * (vertz - 1) * 6] {0};

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
	Vertices* verticesxyzSD = new Vertices[vertx * vertz * nrComponents];
	unsigned int* indices = new unsigned int[(vertx - 1) * (vertz - 1) * 6] {0};

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
			verticesxyzSD[z * vertx + x].Posy = (float)altura[(z * vertx + x) * nrComponents] / 10.0; // nrComponents -> 4
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
			indices[indice++] = i * vertx + j;
			indices[indice++] = (i + 1) * vertx + j;
			indices[indice++] = i * vertx + j + 1;

			indices[indice++] = (i + 1) * vertx + j;
			indices[indice++] = (i + 1) * vertx + j + 1;
			indices[indice++] = i * vertx + j + 1;
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
	const char* filename = fileName;
#ifdef __linux__ 
	if (FreeImage_IsPluginEnabled(FIF_BMP) == -1 || FreeImage_IsPluginEnabled(FIF_BMP) == FALSE)
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
			for (int j = 0; j < (*x) * (*y); j++) {
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

unsigned char* loadMemory(const aiTexture* tex, int* x, int* y, int* comp, int req_comp, bool rotateX, bool rotateY) {
	unsigned char* data = NULL, * tmp = NULL;
#ifdef __linux__ 
	if (FreeImage_IsPluginEnabled(FIF_BMP) == -1 || FreeImage_IsPluginEnabled(FIF_BMP) == FALSE)
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

bool TextureFromMemory(Texture& text, const aiTexture* texture, bool rotateX, bool rotateY, bool* alpha, struct UTILITIES_OGL::ImageDetails* img) {
	bool textureID = false;

	int width, height, nrComponents;
	unsigned char* data = loadMemory(texture, &width, &height, &nrComponents, 0, rotateX, rotateY);
	if (data) {
#ifdef ENGINE_DIRECTX
		textureID = DXLoadTexture(text, data, width, height, nrComponents, alpha, img);
#else
		textureID = GLLoadTexture(text, data, width, height, nrComponents, alpha, img);
#endif
		delete[] data;
		if (!textureID) {
			std::string name = texture->mFilename.C_Str();
			INFO("Texture failed to load at path: " + name, "ERROR LOAD OBJ");
		}
	} else {
		std::string name = texture->mFilename.C_Str();
		INFO("Texture failed to load texture : " + name, "ERROR LOAD OBJ");
	}
	return textureID;
}

bool TextureFromFile(Texture& text, const char* path, const std::string& directory, bool rotateX, bool rotateY, bool* alpha, struct UTILITIES_OGL::ImageDetails* img) {
	std::string filename = std::string(path);
	if (!directory.empty())
		filename = directory + '/' + filename;

	bool textureID = false;

	int width, height, nrComponents;
	unsigned char* data = loadFile(filename.c_str(), &width, &height, &nrComponents, 0, rotateX, rotateY);
	if (data) {
#ifdef ENGINE_DIRECTX
		textureID = DXLoadTexture(text, data, width, height, nrComponents, alpha, img);
#else
		textureID = GLLoadTexture(text, data, width, height, nrComponents, alpha, img);
#endif
		delete[] data;
		if (!textureID) INFO("Texture failed to load at path: " + filename, "ERROR LOAD OBJ");
	}else
		INFO("Texture failed to load at path: " + filename, "ERROR LOAD OBJ");
	return textureID;
}

#ifdef ENGINE_DIRECTX
ID3D11RasterizerState* createCullRasterizer(bool front) {
	if (front && rsFrontState != NULL) return rsFrontState;
	if (!front && rsBackState != NULL) return rsBackState;

	D3D11_RASTERIZER_DESC rs = {};
	rs.FillMode = D3D11_FILL_SOLID;
	rs.DepthClipEnable = TRUE;
	rs.CullMode = front ? D3D11_CULL_FRONT : D3D11_CULL_BACK;
	rs.FrontCounterClockwise = TRUE;
	device->CreateRasterizerState(&rs, front ? &rsFrontState : &rsBackState);
	return front ? rsFrontState : rsBackState;
}

ID3D11DepthStencilState* createDefaultDepthState() {
	if (depthState != NULL) return depthState;
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	device->CreateDepthStencilState(&dsDesc, &depthState);
	return depthState;
}
ID3D11BlendState* createAlphaBlend() {
	if (blendState != NULL) return blendState;
	D3D11_BLEND_DESC bd = {};
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&bd, &blendState);
	return blendState;
}
ID3D11SamplerState* createFontSampler() {
	if (samplerFont) return samplerFont;
	D3D11_SAMPLER_DESC samp = {};
	samp.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; // NO mipmap
//	samp.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samp.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samp.MinLOD = 0;
	samp.MaxLOD = 0;

	device->CreateSamplerState(&samp, &samplerFont);
	return samplerFont;
}
ID3D11SamplerState* createDefaultAlphaSampler() {
	if (samplerAlpha != NULL) return samplerAlpha;
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// MUY IMPORTANTE PARA ALPHA
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	device->CreateSamplerState(&sampDesc, &samplerAlpha);
	return samplerAlpha;
}
ID3D11SamplerState* createDefaultSampler() {
	if (samplerState != NULL) return samplerState;
	D3D11_SAMPLER_DESC samp = {};
	samp.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samp.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samp.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samp.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samp.MinLOD = 0;
	samp.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samp, &samplerState);
	return samplerState;
}
bool DXLoadTexture(Texture& text, unsigned char* data, int width, int height, int nrComponents, bool* alpha, struct UTILITIES_OGL::ImageDetails* img) {
	if (!data) return 0;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	unsigned char* fData = NULL;
	switch (nrComponents) {
		case 1: format = DXGI_FORMAT_R8_UNORM;
			break;
		case 3:	format = DXGI_FORMAT_R8G8B8A8_UNORM;
				fData = expandToRGBA(data, width, height, nrComponents);
			break;
		case 4: format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		default:format = DXGI_FORMAT_R8G8B8A8_UNORM;
				fData = expandToRGBA(data, width, height, nrComponents);
	}
	if (fData != NULL) data = fData;
	if (alpha && *alpha)
		format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (alpha && (nrComponents == 4))
		*alpha = true;
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	ID3D11Texture2D* tex = nullptr;
	HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, &tex);
	if (FAILED(hr)) {
		if (fData != NULL) delete[] fData;
		return false;
	}
	ctx->UpdateSubresource(tex, 0, nullptr, data, width * (nrComponents == 1 ? 1 : 4), 0);
	ID3D11ShaderResourceView* srv = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(-1);
	hr = device->CreateShaderResourceView(tex, &srvDesc, &srv);
	tex->Release();
	if (FAILED(hr)) {
		ERRORL(std::to_string(hr), "ERROR");
		if (fData != NULL) delete[] fData;
		return false;
	}
	if (srv == NULL) return false;
	ctx->GenerateMips(srv);
	// ---------------------------------------
	//  Info de salida (igual que OpenGL)
	// ---------------------------------------
	if (img) {
		img->width = width;
		img->height = height;
		img->nrComponents = nrComponents;
		img->format = format; // ahora DXGI_FORMAT
	}
	text.id = 0;
	text.idDX11 = srv;
	if (fData != NULL) delete[] fData;
	return true;
}
void cleanDXPipeline() {
	// Input Assembler
	ID3D11Buffer* nullVB[8] = {};
	UINT strides[8] = {};
	UINT offsets[8] = {};
	ctx->IASetVertexBuffers(0, 8, nullVB, strides, offsets);
	ctx->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	ctx->IASetInputLayout(nullptr);
	// Shaders
	ctx->VSSetShader(nullptr, nullptr, 0);
	ctx->PSSetShader(nullptr, nullptr, 0);
	ctx->GSSetShader(nullptr, nullptr, 0);
	ctx->HSSetShader(nullptr, nullptr, 0);
	ctx->DSSetShader(nullptr, nullptr, 0);
	ctx->CSSetShader(nullptr, nullptr, 0);
	// Shader resources
	ID3D11ShaderResourceView* nullSRV[16] = {};
	ctx->VSSetShaderResources(0, 16, nullSRV);
	ctx->PSSetShaderResources(0, 16, nullSRV);
	ctx->CSSetShaderResources(0, 16, nullSRV);
	// Samplers
	ID3D11SamplerState* nullSamplers[16] = {};
	ctx->VSSetSamplers(0, 16, nullSamplers);
	ctx->PSSetSamplers(0, 16, nullSamplers);
	ctx->CSSetSamplers(0, 16, nullSamplers);
	// States
	ctx->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	ctx->OMSetDepthStencilState(nullptr, 0);
	ctx->RSSetState(nullptr);
	// Render targets
	ctx->OMSetRenderTargets(0, nullptr, nullptr);
	ctx->PSSetShaderResources(0, 16, nullSRV);
	ctx->ClearState();
	ctx->Flush();
	if (blendState != NULL) blendState->Release();
	if (rsBackState != NULL) rsBackState->Release();
	if (rsFrontState != NULL) rsFrontState->Release();
	if (samplerAlpha != NULL) samplerAlpha->Release();
	if (samplerState != NULL) samplerState->Release();
	if (samplerFont != NULL) samplerFont->Release();
	if (depthState != NULL) depthState->Release();
	if (depthView != NULL) depthView->Release();
	if (backBufferRTV != NULL) backBufferRTV->Release();
	if (swapChain != NULL) swapChain->Release();
	if (ctx != NULL) ctx->Release();
#if defined(ENGINE_DEBUG)
	ID3D11Debug* d3dDebug;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&d3dDebug)))) {
		// 2. Call ReportLiveDeviceObjects
		d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_IGNORE_INTERNAL);
		d3dDebug->Release();
	}
#endif
	if (device != NULL) device->Release();
}
#else
bool GLLoadTexture(Texture& text, unsigned char* data, int width, int height, int nrComponents, bool *alpha, struct UTILITIES_OGL::ImageDetails* img) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	GLenum format = GL_RGBA;
	if (data) {
		if (nrComponents == 1 || nrComponents == 2) {
			format = nrComponents == 2 ? GL_RG : GL_RED;
			if (nrComponents == 2)
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		}
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
	}
	if (img != NULL) {
		img->format = format;
		img->height = height;
		img->nrComponents = nrComponents;
		img->width = width;
	}
	text.id = textureID;
	return true;
}
#endif

double get_nanos() {
	struct timespec ts;
	int r = timespec_get(&ts, TIME_UTC);
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
	}
	else {
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

std::vector<Vertex> init_cube(float x, float y, float z, float width, float height, float depth) {
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

void clearScreen() {
	//borramos el biffer de color y el z para el control de profundidad a la 
	//hora del render a nivel pixel.
	const float clearColor[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
#ifdef ENGINE_DIRECTX
	if (depthView != NULL)ctx->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	ctx->ClearRenderTargetView(backBufferRTV, clearColor);
#else
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
	//	glClearColor(255.0f, 255.0f, 255.0f, 255.0f);
#endif
}

void freeTexture(Texture& texture) {
#ifdef ENGINE_DIRECTX
	if (texture.idDX11 != NULL) {
		texture.idDX11->Release();
		texture.idDX11 = NULL;
	}
#else
	glDeleteTextures(1, &(texture.id));
#endif
}

void setDepthTest(bool enable) {
#ifdef ENGINE_DIRECTX
	ctx->OMSetDepthStencilState(enable ? createDefaultDepthState() : NULL, 0);
	if (enable && depthView != NULL) ctx->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
#else
	if (enable) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
#endif
}

unsigned char* expandToRGBA(unsigned char* data, int width, int height, int nrComponents) {
	long size = width * height * nrComponents, iEx = 0;
	unsigned char* expanded = new unsigned char[width * height * 4];
	for (long i = 0; i < size; i += nrComponents) {
		int j = 0;
		for (j = 0; j < 4 && j < nrComponents; j++)
			expanded[iEx + j] = data[i + j];
		unsigned char v = nrComponents % 4 == 0 ? 255 : (nrComponents < 3 ? data[i] : 255);
		while (j < 3){
			expanded[iEx + j] = v;
			j++;
		}
		if (nrComponents % 4 != 0 || nrComponents < 4)
			expanded[iEx + 3] = (unsigned char)255;
		iEx += 4;
	}
	return expanded;
}

unsigned char* expandGlyphToRGBA(const unsigned char* src, int w, int h, int pitch){
	unsigned char* dst = new unsigned char[w * h * 4];

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			unsigned char a = src[y * pitch + x];
			int i = (y * w + x) * 4;

			dst[i + 0] = 255; // R
			dst[i + 1] = 255; // G
			dst[i + 2] = 255; // B
			dst[i + 3] = a;   // A
		}
	}
	return dst;
}

//void * operator new(size_t size){
//    void * p = malloc(size);
//    return p;
//}