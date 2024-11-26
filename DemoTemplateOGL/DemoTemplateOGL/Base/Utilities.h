#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>
#include <map>
#include <vector>
#include "../WinAPIHeaders/framework.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <glad/glad.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glext.h"
#include "wglext.h"
#include <FreeImage.h>
#include <assimp/matrix4x4.h>

#ifndef UTILITIES_OGL_H
// FLAG TO DISPLAY ERRORS ON MessageBox
#define SHOWLOGGERMB
#define DEBUGFILE
#define MAX_BONE_INFLUENCE 4

#define UTILITIES_OGL_H
// ejemplo de uso de enumeracion para definir cual eje se utilizara
enum Ejes { EjeX, EjeY, EjeZ };

// Tamanio de la ventana
extern long get_nanos();
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;

struct GameTime {
	long lastTick = 0;
	long deltaTime = 0;
};
extern GameTime gameTime;

struct GameActions {
	// float /// x = 0 -> quiet; x > 0 -> move positive; x < 0 -> move negative
	float advance = 0;
	float hAdvance = 0;
	float sideAdvance = 0;
	bool firstPerson = false;
	bool jump = false;
	bool action = false;
	void setZoom(float value);
	float* getZoom();
	void setPlayerZoom(float value);
	float* getPlayerZoom();
	void setAngle(float value);
	float* getAngle();
	void setPitch(float value);
	float* getPitch();
	~GameActions();
	private:
		float* zoom = NULL;
		float* playerZoom = NULL;
		float* angle = NULL;
		float* pitch = NULL;
};

// Estructura Vertice que contiene las propiedades del mesh
struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
	Vertex();
	Vertex(glm::vec3 pos, glm::vec2 texCoord, glm::vec3 normal, glm::vec3 color);

    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct BoneInfo {
    /*id is index in finalBoneMatrices*/
    int id;

    /*offset matrix transforms vertex from model space to bone space*/
    glm::mat4 offset;

};
// Estructura de Texture para guardar el ID de la textura y su tipo
struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

extern std::wstring s2ws(const std::string& s);

extern unsigned int GetSizeOfType(unsigned int type);

namespace UTILITIES_OGL {

	struct ImageDetails {
		int width;
		int height;
		int nrComponents;
		GLenum format;
	};

	//estructura para manejar primitivos con posicion, normal y uv's
	struct Vertices {
		float Posx, Posy, Posz;
		float Normx, Normy, Normz;
		float u, v;
	};

struct AssimpNodeData {
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

struct KeyPosition {
    glm::vec3 position;
    double timeStamp;
};

struct KeyRotation {
    glm::quat orientation;
    double timeStamp;
};

struct KeyScale {
    glm::vec3 scale;
    double timeStamp;
};

	//En honor a nuestros ancestros llamaremos "Maya" a la malla
	//estructura que contiene datos de los vertices y sus indices
	struct Maya {
		Vertices* maya;
		unsigned int* indices;
	};

	extern glm::mat4 aiMatrix4x4ToGlm(aiMatrix4x4& from);
	extern void calculateNormals(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
	extern void sumaNormal(float* v1, float* v2);
	extern void normaliza(float* v1);
	extern void vectoresEsfera(Maya esfera, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, unsigned int iv, unsigned int ii);
	extern Maya Esfera(int stacks, int slices, float radio, float inicio, float final);
	extern Maya Plano(int vertx, int vertz, float anchof, float profz);
	extern Maya Plano(int vertx, int vertz, float anchof, float profz, unsigned char* altura, int nrComponents = 4, float tile = 1);
	extern glm::vec3 genNormal(float* v1, float* v2, float* v3);
}
extern unsigned char* loadFile(char const* filename, int* x, int* y, int* comp, int req_comp, bool rotateX = false, bool rotateY = true);
extern unsigned int TextureFromFile(const char* path, const std::string& directory, bool rotateX = false, bool rotateY = true, bool *alpha = NULL, struct UTILITIES_OGL::ImageDetails* img = NULL);

#pragma once
#ifndef LOGGER_H
#define LOGGER_H

namespace LOGGER {
	class LOG {
	private:
		void* WINDOW;
		std::string name;
	public:
		LOG();
		LOG(std::string filename);
		LOG(std::string filename, void* hwnd);
		std::string getLogger();
		void info(const std::string log, const std::string title);
		void info(const std::string log);
		void info(const char* log);
		void warning(const std::string log);
		void warning(const std::string log, const std::string title);
		void warning(const char* log);
		void error(const std::string log);
		void error(const std::string log, const std::string title);
		void error(const char* log);
		void question(const std::string log);
		void question(const std::string log, const std::string title);
		void question(const char* log);
		void exclamation(const std::string log);
		void exclamation(const std::string log, const std::string title);
		void exclamation(const char* log);
		void processLog(const char* log, const char* title, const char* type, unsigned int MB_TYPE);
		void setWindow(void* hwnd);
		void* getWindow();
	};

	class LOGS {
	private:
		static std::vector<LOG> log;
	public:
		static void* WINDOW;
		static LOG getLOGGER();
		static LOG getLOGGER(std::string filename);
	};
}
#endif

//void * operator new(size_t size);

#endif