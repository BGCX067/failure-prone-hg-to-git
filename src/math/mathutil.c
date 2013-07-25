#include "mathutil.h"
#include <math.h>

/* Por algum motivo M_PI nao aparece definido em math.h */
#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#ifndef EPS
#define EPS	    0.00001
#endif


float Max(float a, float b){
	if (a>b)
		return a;
	else
		return b;
}

float Min(float a, float b){
	if (a<b)
		return a;
	else
		return b;
}

int FCmp(float a, float b) {
    if(fabs(a - b) < EPS)
        return 0;
    else
        return 1;
}

int RayTriangleIntersection(vec3 ro, vec3 rd, vec3 va, vec3 vb, vec3 vc, float *t) {
    vec3 edge1, edge2, tvec, pvec, qvec;

    Subv(vb, va, edge1);
    Subv(vc, va, edge2);

    Cross(rd, edge2, pvec);

    float det = Dot(edge1, pvec);
    float invdet = 1.0f/det;

    //TODO considerar culling?
    Subv(ro, va, tvec);
    float u = Dot(tvec, pvec)*invdet;

    if(u < 0.0f || u > 1.0f)
        return 0;

    Cross(tvec, edge1, qvec);
    float v = Dot(rd, qvec)*invdet;
    if(v < 0.0f || u + v > 1.0f)
        return 0;

    *t =  Dot(edge2, qvec)*invdet;

    return 1;
}

float DegToRad(float angle) {
    return M_PI*angle/180.0f;
}

void CalcBBox(BBox *b, float *vertices, int numvertices) {
    b->pmin[0] = 99999999;
    b->pmin[1] = 99999999;
    b->pmin[2] = 99999999;

    b->pmax[0] = -999999999;
    b->pmax[1] = -999999999;
    b->pmax[2] = -999999999;

    for(int j = 0; j < numvertices; j++) {
        //X
        if(vertices[3*j] < b->pmin[0])
            b->pmin[0] = vertices[3*j];
        if (vertices[3*j] > b->pmax[0])
            b->pmax[0] = vertices[3*j];
        //Y
        if(vertices[3*j + 1] < b->pmin[1])
            b->pmin[1] = vertices[3*j + 1];
        if (vertices[3*j + 1] > b->pmax[1])
            b->pmax[1] = vertices[3*j + 1];
        //Z
        if(vertices[3*j + 2] < b->pmin[2])
            b->pmin[2] = vertices[3*j + 2];
        if (vertices[3*j + 2] > b->pmax[2])
            b->pmax[2] = vertices[3*j + 2];
    }
}

void BBCenter(BBox b, vec3 v) {
    v[0] = 0.5*(b.pmin[0] + b.pmax[0]);
    v[1] = 0.5*(b.pmin[1] + b.pmax[1]);
    v[2] = 0.5*(b.pmin[2] + b.pmax[2]);
}

void BBUnion(BBox *res, BBox b1, BBox b2) {
    res->pmin[0] = b1.pmin[0] < b2.pmin[0] ? b1.pmin[0] : b2.pmin[0];
    res->pmin[1] = b1.pmin[1] < b2.pmin[1] ? b1.pmin[1] : b2.pmin[1];
    res->pmin[2] = b1.pmin[2] < b2.pmin[2] ? b1.pmin[2] : b2.pmin[2];

    res->pmax[0] = b1.pmax[0] > b2.pmax[0] ? b1.pmax[0] : b2.pmax[0];
    res->pmax[1] = b1.pmax[1] > b2.pmax[1] ? b1.pmax[1] : b2.pmax[1];
    res->pmax[2] = b1.pmax[2] > b2.pmax[2] ? b1.pmax[2] : b2.pmax[2];
}

