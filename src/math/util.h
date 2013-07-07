#ifndef MATHUTIL_H_
#define MATHUTIL_H_

#include "vec3.h"
#include "renderer/mesh.h"
#include "matrix.h"

float max(float a, float b);
float min(float a, float b);

int rayTriangleIntersection(vec3 ro, vec3 rd, vec3 va, vec3 vb, vec3 vc, float *t);
int rayMeshIntersection(vec3 ro, vec3 rd, Mesh *m, int *indices);
int picking(int mouseX, int mouseY, mat4 modelviewMatrix, mat4 projectionMatrix, int viewPort[4], Mesh *m, int *indices);

#endif
