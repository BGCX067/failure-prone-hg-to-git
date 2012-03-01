#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "vec3.h"
#include "matrix.h"

/*[0] = x, [1] = y, [2] = z, [3] = angle*/
typedef float quaternion[4];

#define QUAT_IDENTITY(q) { q[0] = 0.0; q[1] = 0.0; q[2] = 0.0; q[3] = 1.0;}

float quatLength(quaternion q);
void quatNormalize(quaternion q);
void quatConjugate(quaternion q, quaternion conj);
void quatMult(quaternion q1, quaternion q2, quaternion res);
void quatMultVec(quaternion q, vec3 v, quaternion res);
void fromAxisAngle(vec3 v, float theta, quaternion q);
void quatToMatrix(quaternion q, mat4 m);
void rotateVec(vec3 v, quaternion q, vec3 res);
void quatComputeAngle(quaternion q);
void quatSlerp(quaternion q1, quaternion q2, float t, quaternion res);
float quatDotProduct(quaternion q1, quaternion q2);

#endif
