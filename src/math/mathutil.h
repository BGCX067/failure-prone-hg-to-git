#ifndef MATHUTIL_H_
#define MATHUTIL_H_

#include "vec3.h"
#include "matrix.h"

float Max(float a, float b);
float Min(float a, float b);
int FCmp(float a, float b);

int RayTriangleIntersection(vec3 ro, vec3 rd, vec3 va, vec3 vb, vec3 vc, float *t);
float DegToRad(float angle);


/* axis-aligned bounding box */
typedef struct _boundingbox {
    vec3 pmin;
    vec3 pmax;
} BBox;

//numvertices aqui é realmente o número de vertices (tamanho do vetor/3, em geral)
void CalcBBox(BBox *b, float *vertices, int numvertices);
void bbcenter(BBox b, vec3 v);
void BBUnion(BBox *res, BBox b1, BBox b2);

#endif
