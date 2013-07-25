#ifndef _VEC3_H_
#define _VEC3_H_

typedef float vec3[3];

void Addv(vec3 v1, vec3 v2, vec3 res);
void Subv(vec3 v1, vec3 v2, vec3 res);
void Multv(vec3 v, float s, vec3 res);
float Dot(vec3 v1, vec3 v2);
void Cross(vec3 v1, vec3 v2, vec3 res);
float Lengthv(vec3 v);
void Normalizev(vec3 v);
float Anglev(vec3 v1, vec3 v2);

void Setv(vec3 dest, vec3 src);
void Setvf(vec3 dest, float x, float y, float z);

#endif
