#include "mathutil.h"
#include <math.h>

/* Por algum motivo M_PI nao aparece definido em math.h */
#ifndef M_PI
#define M_PI	3.14159265358979323846
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

//Returns 0 if a == b 1 otherwise
int FCmp(float a, float b) {
    if(fabs(a - b) < EPS_FLOAT)
        return 0;
    else if(a > b)
        return 1;
    else
        return -1;
}

//almost equal
bool DAEqual(double a, double b, double epsilon) {
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

//essentially equal
bool DEqual(double a, double b, double epsilon) {
    return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool DGreater(double a, double b, double epsilon) {
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool DLesser(double a, double b, double epsilon) {
    return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
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

//bool pointInTriangle(float *p, float *v0, float *v1, float *v2) {  
//}

//Supõe que seja 2D
//void EarClipTrig(float *vertices, int numvertices) {
//    //Algoritmo:
//    //for each Vi and the corresponding triangle V(i-1)ViV(i + 1)
//    //  test if any other vertice is inside the triangle
//    //TODO construir lista encadeada de vertices ?
//    struct vert {
//        int index;
//        float coord[3];
//        struct vert *next, *prev;
//    };
//
//    struct vert* head = malloc(sizeof(struct vert));
//    head->next = head;
//    head->prev = head;
//    head->index = 0;
//    head->x = vertices[0];
//    head->y = vertices[1];
//    struct vert* last = head;
//    for(int i = 1; i < numvertices; i++) {
//        struct vert *v = malloc(sizeof())
//        v->index = i;
//        v->coord[0] = vertices[2*i];
//        v->coord[1] = vertices[2*i + 1];
//        v->coord[2] = 0.0;
//        v->prev = last;
//        v->next = head;
//        last->next = v;
//        head->prev = v;
//        last = v;
//    }
//
//    int nvert = numvertices;
//    struct vert* currv = head;
//    while (nvert > 3) {
//        struct vert* v0 = currv;
//        struct vert* v1 = v0->next;
//        struct vert* v2 = v1->next;
//        bool isEar = true;
//        for(struct vert* it = v1->next; it != v0; it = it->next) {
//            if(pointInTriangle(it->coord, v0->coord, v1->coord, v2->coord)) {
//                isEar = false;
//                break;
//            }
//        }
//        if(isEar) {
//            //faz triangulo v0v1v2
//            tris[currtri] = v0->coord[0];
//            tris[currtri + 1] = v0->coord[1];
//            tris[currtri + 2] = v1->coord[0];
//            tris[currtri + 3] = v1->coord[1];
//            tris[currtri + 4] = v2->coord[0];
//            tris[currtri + 5] = v2->coord[1];
//            //remove v1 da lista
//            v1->prev->next = v1->next;
//            v1->next->prev = v1->prev;
//            free(v1);
//            nvert--;
//        } 
//        currv = currv->next;
//    }
//    //os 3 vertices restantes formam o ultimo triangulo
//    tris[currtri] = currv->coord[0];
//    tris[currtri + 1] = currv->coord[1];
//    tris[currtri + 2] = (currv->next)->coord[0];
//    tris[currtri + 3] = (currv->next)->coord[1];
//    tris[currtri + 4] = (currv->next->next)->coord[0];
//    tris[currtri + 5] = (currv->next->next)->coord[1];
//
//
//    /*bool clipped[numvertices] { false };
//    for(int i = 0; i < numvertices; i++) {
//        bool isEar = true;
//        //vertices do triangulo: i, i+1, i+2
//        float v0[3] = { vertices[2*i], vertices[2*i + 1], 0.0};
//        float v1[3] = { vertices[2*(i + 1)], vertices[2*(i + 1) + 1], 0.0};
//        float v2[3] = { vertices[2*(i + 2)], vertices[2*(i + 2) + 1], 0.0};
//        for(int j = (i + 3)%numvertices; j != i; j = (j + 1)%numvertices) {
//            //ponto pra testar interseção: j
//            float p[3] = {vertices[2*j], vertices[2*j +1], 0.0};
//            if(pointInTriangle(p, v0, v1, v2)) {
//                isEar = false;
//                break;
//            }
//        }
//        if(isEar) {
//            //v0, v1, v2 é um triangulo válido.
//            //remover v1 da lista de vertices e testar de novo com a lista menor 
//            clipped[vertices[i + 1] = true;
//        }
//
//    }*/
//}

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

