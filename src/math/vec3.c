#include "vec3.h"
#include <math.h>

void vecAdd(vec3 v1, vec3 v2, vec3 res) {
    res[0] = v1[0] + v2[0];
    res[1] = v1[1] + v2[1];
    res[2] = v1[2] + v2[2];
}

void vecSub(vec3 v1, vec3 v2, vec3 res) {
    res[0] = v1[0] - v2[0];
    res[1] = v1[1] - v2[1];
    res[2] = v1[2] - v2[2];
}

void vecMult(vec3 v, float s, vec3 res) {
    res[0] = v[0]*s;
    res[1] = v[1]*s;
    res[2] = v[2]*s;
}

float dot(vec3 v1, vec3 v2) {
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void cross(vec3 v1, vec3 v2, vec3 res) {
    res[0] = v1[1]*v2[2] - v1[2]*v2[1];
    res[1] = v1[2]*v2[0] - v1[0]*v2[2];
    res[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

float vecLength(vec3 v) {
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

void vecNormalize(vec3 v) {
    float len = vecLength(v);
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

//TODO otimizar, acos é lento
float vecAngle(vec3 v1, vec3 v2) {
    return acos(dot(v1, v2)/(vecLength(v1)*vecLength(v2)));
}
