#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "vec3.h"
#include "matrix.h"

/*[0] = x, [1] = y, [2] = z, [3] = angle*/
typedef float quat[4];

//#define QUAT_IDENTITY(q) { q[0] = 0.0; q[1] = 0.0; q[2] = 0.0; q[3] = 1.0;}

void Normalizeq(quat q);
float Lengthq(quat q);
void Conjugateq(quat q, quat conj);
void Multq(quat q1, quat q2, quat res);
void Multqv(quat q, vec3 v, quat res);
void FromAxisAngle(vec3 v, float theta, quat q);
void ToMatrixq(quat q, mat4 m);
void Rotateq(vec3 v, quat q, vec3 res);
void Angleq(quat q);
void Slerpq(quat q1, quat q2, float t, quat res);
float Dotq(quat q1, quat q2);

void Setq(quat dest, quat src);
void Setqf(quat q, float x, float y, float z, float w);

#endif
