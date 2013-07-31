#include "vec3.h"
#include <math.h>
#include <string.h>

void Addv(vec3 v1, vec3 v2, vec3 res) {
    res[0] = v1[0] + v2[0];
    res[1] = v1[1] + v2[1];
    res[2] = v1[2] + v2[2];
}

void Subv(vec3 v1, vec3 v2, vec3 res) {
    res[0] = v1[0] - v2[0];
    res[1] = v1[1] - v2[1];
    res[2] = v1[2] - v2[2];
}

void Multv(vec3 v, float s, vec3 res) {
    res[0] = v[0]*s;
    res[1] = v[1]*s;
    res[2] = v[2]*s;
}

float Dot(vec3 v1, vec3 v2) {
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void Cross(vec3 v1, vec3 v2, vec3 res) {
    vec3 tmp;
    tmp[0] = v1[1]*v2[2] - v1[2]*v2[1];
    tmp[1] = v1[2]*v2[0] - v1[0]*v2[2];
    tmp[2] = v1[0]*v2[1] - v1[1]*v2[0];
    memcpy(res, tmp, sizeof(vec3));
}

float Lengthv(vec3 v) {
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

void Normalizev(vec3 v) {
    float len = Lengthv(v);
    if (len == 0)
	    return;
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

float Anglev(vec3 v1, vec3 v2) {
    return acos(Dot(v1, v2)/(Lengthv(v1)*Lengthv(v2)));
}


void Setv(vec3 dest, vec3 src) {
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
}

void Setvf(vec3 dest, float x, float y, float z) {
    dest[0] = x;
    dest[1] = y;
    dest[2] = z;
}

