#include "vec3.h"
#include <math.h>

void addvec(vec3 v1, vec3 v2, vec3 res) {
    res[0] = v1[0] + v2[0];
    res[1] = v1[1] + v2[1];
    res[2] = v1[2] + v2[2];
}

void subvec(vec3 v1, vec3 v2, vec3 res) {
    res[0] = v1[0] - v2[0];
    res[1] = v1[1] - v2[1];
    res[2] = v1[2] - v2[2];
}

float dot(vec3 v1, vec3 v2) {
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void cross(vec3 v1, vec3 v2, vec3 res) {
    res[0] = v1[1]*v2[2] - v2[1]*v1[2];
    res[1] = v1[2]*v2[0] - v1[0]*v2[2];
    res[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

float length(vec3 v) {
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

void normalize(vec3 v) {
    float len = length(v);
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}
