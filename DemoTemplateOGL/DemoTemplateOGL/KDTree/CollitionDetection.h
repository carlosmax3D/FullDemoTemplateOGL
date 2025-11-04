#include "../Base/Utilities.h"
#include "KDTree.h"
#include <iostream>
// https://github.com/Meirshomron/MeshCollision
#ifndef COLLITION_DETECTION_H
#define COLLITION_DETECTION_H

extern bool SolveEquision(Node& A, Node& B, glm::vec3& L, glm::mat4& TRTB, glm::mat4& TRTA);
extern bool checkCollision(Node& A, Node& B, glm::mat4& TRTB, glm::mat4& TRTA);
extern void print_queue(std::queue<std::pair<Node*, Node*>> q);
extern bool findCollision(std::pair<Node*, Node*>& collistionNodes, Node& firstShape, glm::mat4 firstTransScale, Node& secondShape, glm::mat4 secondTransScale);
extern std::vector<glm::vec3> obtenerEjesSeparacion(const glm::mat4& M1, const glm::mat4& M2);
extern void obtenerEjesSeparacion(glm::vec3* ejes, const glm::mat4& M1, const glm::mat4& M2);
extern bool proyectarYComprobarSolapamiento(Vertex* verticesCubo1, 
                                     Vertex* verticesCubo2, 
                                     const glm::vec3& eje);
extern bool rayIntersectsOBB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const vector<Vertex>& verticesCubo1, const glm::mat4& M1, float& tMin, float& tMax);
#endif