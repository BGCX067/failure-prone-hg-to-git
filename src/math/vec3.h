#ifndef _VEC3_H_
#define _VEC3_H_

typedef float vec3[3];

#define VEC3_ZERO(v) {v[0] = 0.0; v[1] = 0.0; v[2] = 0.0;}

void vecAdd(vec3 v1, vec3 v2, vec3 res);
void vecSub(vec3 v1, vec3 v2, vec3 res);
void vecMult(vec3 v, float s, vec3 res);

float dot(vec3 v1, vec3 v2);
void cross(vec3 v1, vec3 v2, vec3 res);

float vecLength(vec3 v);
void vecNormalize(vec3 v);

float vecAngle(vec3 v1, vec3 v2);

#endif
