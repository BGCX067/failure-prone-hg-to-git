#ifndef _VEC3_H_
#define _VEC3_H_

typedef float vec3[3];

void addvec(vec3 v1, vec3 v2, vec3 res);
void subvec(vec3 v1, vec3 v2, vec3 res);
float dot(vec3 v1, vec3 v2);
void cross(vec3 v1, vec3 v2, vec3 res);
void normalize(vec3 v);
float length(vec3 v);

#endif
