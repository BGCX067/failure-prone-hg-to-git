#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "vec3.h"

typedef float mat4[16];

void fpIdentity(mat4 m);
void fpMultMatrix(mat4 res, mat4 a, mat4 b);

void fptranslatefv(mat4 m, vec3 v);
void fptranslatef(mat4 m, float x, float y, float z);

void fpperspective(mat4 m, float fovy, float ratio, float znear, float zfar);


#endif
