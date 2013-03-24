#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "vec3.h"

typedef float mat4[16];
typedef float mat3[9];

void fpIdentity(mat4 m);
void fpMultMatrix(mat4 res, const mat4 a, const mat4 b);
void fpTranspose(mat4 t, mat4 m);
float fpDeterminant(mat4 m);
void fpInverse(mat4 inv, const mat4 m);

void fptranslatefv(mat4 m, vec3 v);
void fptranslatef(mat4 m, float x, float y, float z);

void fpRotatef(mat4 matrix, float angleInRadians, float x, float y, float z);

void fpLookAt(mat4 result, vec3 position, vec3 look, vec3 up);

void fpperspective(mat4 m, float fovy, float ratio, float znear, float zfar);

void fpOrtho( mat4 m, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);

void fpNormalMatrix(mat3 normalmatrix, mat4 mv);

void fpUnproject(float winx, float winy, float winz, 
                 const float modelMatrix[16],
                 const float projMatrix[16],
                 const int viewport[4],
                 float *x, float *y, float *z);


void fpGetPosFromMatrix(const mat4 matrix, vec3 pos);


void fpMultMatrixVecf(const float matrix[16], const float in[4], float out[4]);
#endif
