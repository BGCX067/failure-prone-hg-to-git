#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "vec3.h"

typedef float mat4[16];
typedef float mat3[9];

//Funções abaixo só funcionam pra mat4
void Identity(mat4 m);
void Multm(mat4 res, const mat4 a, const mat4 b);
void Transpose(mat4 t, mat4 m);
float Determinant(mat4 m);
void Inverse(mat4 inv, const mat4 m);
//FIXME fora do padrão, recebe um vec4
void Multmv(const float matrix[16], const float in[4], float out[4]);


//Transformations related
void Translatefv(mat4 m, vec3 v);
void Translatef(mat4 m, float x, float y, float z);
void Rotatef(mat4 matrix, float angleInRadians, float x, float y, float z);
void LookAt(mat4 result, vec3 position, vec3 look, vec3 up);
void Perspective(mat4 m, float fovy, float ratio, float znear, float zfar);
void Ortho( mat4 m, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
void NormalMatrix(mat3 normalmatrix, mat4 mv);
void Unproject(float winx, float winy, float winz, 
               const float modelMatrix[16],
               const float projMatrix[16],
               const int viewport[4],
               float *x, float *y, float *z);
void PosFromMatrix(const mat4 matrix, vec3 pos);

#endif
