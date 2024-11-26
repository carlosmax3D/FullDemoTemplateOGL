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
    void reloadData(vector<Vertex>& vertices) {
        // Increment time for animation
        static float waveSpeed = 0.1f;
        static float time = 0.f;
        static float waveAmplitude = 0.5f;
        static float waveFrequency = 0.5f;
        time += waveSpeed;

        // Define the size of the water surface
        static const float water_size = 200;

        // Define the number of segments for the water surface
        unsigned int x = 0; // Number of segments in X direction
        unsigned int z = 0; // Number of segments in Z direction

        for (unsigned int segments = 0; segments < vertices.size(); segments++) {
            // Calculate position based on wave function
			x = segments / (getMapAlturaX() * 3);
			z = segments % (getMapAlturaX() * 3);
			Vertex &vertex = vertices.at(segments);
			float xPos = vertex.Position.x;
            float zPos = vertex.Position.z;
            float yPos = waveAmplitude * sin(waveFrequency * (xPos + time)) + waveAmplitude * sin(waveFrequency * (zPos + time));

            // Add vertex with updated position and texture coordinates
			vertex.Position.y = yPos;
//          vertex.TexCoords = glm::vec2((float)x / (numSegmentsX - 1), (float)z / (numSegmentsZ - 1));
        }
		// Recalculate normals based on updated vertices
//		UTILITIES_OGL::calculateNormals(vertices, meshes[0]->indices);
//		buildKDtree();
    }
public:
	Water(WCHAR alturas[], WCHAR textura[], float ancho, float prof, Camera* camera)
		: Terreno(alturas, textura, ancho, prof, camera, GL_DYNAMIC_DRAW) {
		gpuDemo = NULL;
		buildKDtree();
	}

	~Water() {
		//nos aseguramos de disponer de los recursos previamente reservados
	}

	void Draw() {
		if (gpuDemo == NULL) {
			gpuDemo = new Shader("shaders/models/1.model_loading.vs", "shaders/models/1.model_loading.fs");
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

	void Draw(Shader& shader) {
		reloadData(meshes[0]->vertices);
		Model::Draw(shader);
	}

};

#endif 