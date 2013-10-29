#ifndef MATHUTIL_H_
#define MATHUTIL_H_

#include "vec3.h"
#include "matrix.h"
#include <stdbool.h>

#ifndef EPS_FLOAT
#define EPS_FLOAT     1E-5
#endif

#ifndef EPS_DOUBLE
#define EPS_DOUBLE    1E-9
#endif

float Max(float a, float b);
float Min(float a, float b);
int FCmp(float a, float b);

bool DAEqual(double a, double b, double epsilon);
bool DEqual(double a, double b, double epsilon);
bool DGreater(double a, double b, double epsilon);
bool DLesser(double a, double b, double epsilon);

int RayTriangleIntersection(vec3 ro, vec3 rd, vec3 va, vec3 vb, vec3 vc, float *t);
float DegToRad(float angle);

void EarClipTrig(float *vertices, int numvertices);

/* axis-aligned bounding box */
typedef struct _boundingbox {
    vec3 pmin;
    vec3 pmax;
} BBox;

//numvertices aqui é realmente o número de vertices (tamanho do vetor/3, em geral)
void CalcBBox(BBox *b, float *vertices, int numvertices);
void BBCenter(BBox b, vec3 v);
void BBUnion(BBox *res, BBox b1, BBox b2);

#endif
