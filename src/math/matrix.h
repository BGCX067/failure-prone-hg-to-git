#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "vec3.h"

typedef float mat4[16];
typedef float mat3[9];

/**
 *  Matriz column-major
 *
 *  0   4   8   12
 *  1   5   9   13
 *  2   6   10  14
 *  3   7   11  15
 *
 *
 *  Orientação a partir da matrix:
 *  X -> [0, 1, 2]
 *  Y -> [4, 5, 6]
 *  Z -> [8, 9, 10]
 *  pos -> [12, 13, 14]
 *
 *
 *  Camera - representa o inverso da transformação
 *  aplicada no mundo. Assumindo que não tem escala
 *  nessa matriz, a transposta da modelview é igual à
 *  sua inversa
 *      - OBS1: Função look at, por exemplo, escreve a
 *      transposta da matriz acima
 *  
 *
 **/

//Funções abaixo só funcionam pra mat4
void Identity(mat4 m);
void Multm(mat4 res, const mat4 a, const mat4 b);
void Transpose(mat4 t, mat4 m);
float Determinant(mat4 m);
void Inverse(mat4 inv, const mat4 m);
//FIXME fora do padrão, recebe um vec4
void Multmv(const float matrix[16], const float in[4], float out[4]);


//Transformations related
//Translate, Rotate and Scale multiplicam a matriz m (arg)
//a direita (post-multiply - igual ao padrão do OpenGL)
//Obs: talvez seja interessante fazer multiplicação a esquerda
//(pre-multiply) para ter uma API mais simples e intuitiva
void Translatefv(mat4 m, vec3 v);
void Translatef(mat4 m, float x, float y, float z);
void Rotatef(mat4 m, float angInRad, float x, float y, float z);
void Scalef(mat4 m, float x, float y, float z);

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
