#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "vec3.h"
#include "matrix.h"

/*[0] = x, [1] = y, [2] = z, [3] = angle*/
typedef float quaternion[4];


float quatLength(quaternion q);
void quatNormalize(quaternion q);
void conjugate(quaternion q, quaternion conj);
void mult(quaternion q1, quaternion q2, quaternion res);
void fromAxisAngle(vec3 v, float theta, quaternion q);
void quatToMatrix(quaternion q, mat4 m);

#endif
