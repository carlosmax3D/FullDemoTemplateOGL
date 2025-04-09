#pragma once
#ifndef _wata
#define _wata
#include "Base/Utilities.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Base/glext.h"
#include "Base/wglext.h"
#include "Terreno.h"

class Water : public Terreno {

private:
double WATER_PI_FACTOR = 2.0f * M_PI;
size_t mapSizeX;
static constexpr float waveSpeed = 0.1f;
static constexpr float waveAmplitude = 0.5f;
static constexpr float waveFrequency = 0.5f;
void reloadData(vector<Vertex>& vertices) {
    static float time = 0.f;
    time += waveSpeed;
    
    const float waveTime = waveFrequency * time;

    Vertex* vData = vertices.data();  // Direct pointer access for fast memory operations
    size_t numVerts = vertices.size();

    for (size_t i = 0; i < numVerts; i += 4) {  // Loop unrolling: Process 4 vertices per iteration
        for (int j = 0; j < 4 && (i + j) < numVerts; j++) {
            Vertex &v = vData[i + j];  // Access vertex directly using pointer arithmetic

            float xPos = v.Position.x;
            float zPos = v.Position.z;

            // Fast sine lookup (LUT) instead of costly sin() calls
            int xIndex = ((int)((waveFrequency * xPos + waveTime) * LUT_SIZE / (WATER_PI_FACTOR))) % LUT_SIZE;
            int zIndex = ((int)((waveFrequency * zPos + waveTime) * LUT_SIZE / (WATER_PI_FACTOR))) % LUT_SIZE;
            
            v.Position.y = waveAmplitude * (UTILITIES_OGL::sinLUT[xIndex] + UTILITIES_OGL::sinLUT[zIndex]);
        }
    }
    // If needed, recalculate normals and rebuild KD tree
//    UTILITIES_OGL::calculateNormals(vertices, meshes[0]->indices);
//    buildKDtree();
}
public:
	Water(WCHAR alturas[], WCHAR textura[], float ancho, float prof, Camera* camera)
		: Terreno(alturas, textura, ancho, prof, camera, GL_DYNAMIC_DRAW) {
		gpuDemo = NULL;
//		buildKDtree();
		mapSizeX = getMapAlturaX() * 3;
		for (int i = 0; i < LUT_SIZE; i++) {
			UTILITIES_OGL::sinLUT[i] = sin((float)i / LUT_SIZE * 2.0f * M_PI);
		}
	}

	~Water() {
		//nos aseguramos de disponer de los recursos previamente reservados
	}

	virtual void Draw() {
		if (gpuDemo == NULL) {
			gpuDemo = new Shader("shaders/water.vs", "shaders/water.fs");
			setDefaultShader(true);
		}
		if (getDefaultShader()) {
			gpuDemo->use();
			prepShader(*gpuDemo);
			Draw(*gpuDemo);
			gpuDemo->desuse();
		}
		else Draw(*gpuDemo);
	}

	virtual void Draw(Shader& shader) {
//		reloadData(meshes[0]->vertices);
		static float time = 0.f;
		time += waveSpeed;
		shader.setFloat("time", time);
		Model::Draw(shader,0);
	}

};
#endif 