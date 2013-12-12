#include "mathutil.h"
#include <math.h>
#include <stdlib.h>
#include "utlist.h"

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



double TriangleArea2D(double a[2], double b[2], double c[2]) {
    return (b[0] - a[0])*(c[1] - a[1]) - (c[0] - a[0])*(b[1] - a[1]);
}

bool LeftPt2D(double a[2], double b[2], double c[2]) {
    return DGreater(TriangleArea2D(a, b, c), 0.0, EPS_FLOAT);
}

bool LeftOnPt2D(double a[2], double b[2], double c[2]) {
    double area = TriangleArea2D(a, b, c);
    return DAEqual(area, 0.0, EPS_FLOAT) || DGreater(area, 0.0, EPS_FLOAT);
}

bool CollinearPt2D(double a[2], double b[2], double c[2]) {
    return DEqual(TriangleArea2D(a, b, c), 0.0, EPS_FLOAT);
}

bool IntersectPropLine2D(double a[2], double b[2], double c[2], double d[2]) {
    //Checks collinearity
    if(CollinearPt2D(a, b, c) || CollinearPt2D(a, b, d) ||
       CollinearPt2D(c, d, a) || CollinearPt2D(c, d, b) )
        return false;

    return (!LeftPt2D(a, b, c) != !LeftPt2D(a, b, d)) && (!LeftPt2D(c, d, a) != !LeftPt2D(c, d, b));
}

bool BetweenPt2D(double a[2], double b[2], double c[2]) {
    if(!CollinearPt2D(a, b, c))
        return false;

    if(!DEqual(a[0], b[0], EPS_FLOAT)) {
        bool a0lec0 = DEqual(a[0], c[0], EPS_FLOAT) || DLesser(a[0], c[0], EPS_FLOAT);
        bool c0leb0 = DEqual(c[0], b[0], EPS_FLOAT) || DLesser(c[0], b[0], EPS_FLOAT);
        bool a0gec0 = DEqual(a[0], c[0], EPS_FLOAT) || DGreater(a[0], c[0], EPS_FLOAT);
        bool c0geb0 = DEqual(c[0], b[0], EPS_FLOAT) || DGreater(c[0], b[0], EPS_FLOAT);
        return (a0lec0 && c0leb0) || (a0gec0 && c0geb0);
    } else {
        bool a1lec1 = DEqual(a[1], c[1], EPS_FLOAT) || DLesser(a[1], c[1], EPS_FLOAT);
        bool c1leb1 = DEqual(c[1], b[1], EPS_FLOAT) || DLesser(c[1], b[1], EPS_FLOAT);
        bool a1gec1 = DEqual(a[1], c[1], EPS_FLOAT) || DGreater(a[1], c[1], EPS_FLOAT);
        bool c1geb1 = DEqual(c[1], b[1], EPS_FLOAT) || DGreater(c[1], b[1], EPS_FLOAT);
        return (a1lec1 && c1leb1) || (a1gec1 && c1geb1);
    }
//    if(a[0] != b[0])
//        return ((a[0] <= c[0]) && (c[0] <= b[0])) ||
//               ((a[0] >= c[0]) && (c[0] >= b[0]));
//    else 
//        return ((a[1] <= c[1]) && (c[1] <= b[1])) ||
//               ((a[1] >= c[1]) && (c[1] >= b[1]));
}

bool IntersectLine2D(double a[2], double b[2], double c[2], double d[2]) {
    if(IntersectPropLine2D(a, b, c, d))
        return true;
    else if(BetweenPt2D(a, b, c) || BetweenPt2D(a, b, d) ||
            BetweenPt2D(c, d, a) || BetweenPt2D(c, d, b))
        return true;
    return false;
}

//Polygon Triangulation Related
typedef struct vert {
    int id;
    double coord[2];
    bool ear;
    struct vert *next, *prev;
} VertexNode2D;

static bool diagonalie2D(VertexNode2D *a, VertexNode2D *b) {
    VertexNode2D *c = a->next, *c1;

    do {
        c1 = c->next;
        //TODO comparar os ponteiros apenas
        if((c != a) && (c1 != a) &&
           (c != b) && (c1 != b) &&
           IntersectLine2D(a->coord, b->coord, c->coord, c1->coord))
            return false;
        c = c->next;
    } while(c != a->prev);
    return true;
}

static bool inCone2D(VertexNode2D *a, VertexNode2D *b) {
    VertexNode2D *a0 = a->prev, *a1 = a->next;
    if(LeftOnPt2D(a->coord, a1->coord, a0->coord)) 
        return LeftPt2D(a->coord, b->coord, a0->coord) && 
               LeftPt2D(b->coord, a->coord, a1->coord);

    return !(LeftOnPt2D(a->coord, b->coord, a1->coord) && 
             LeftOnPt2D(b->coord, a->coord, a0->coord));
}

static bool diagonal(VertexNode2D *a, VertexNode2D *b) {
    return inCone2D(a, b) && inCone2D(b, a) && diagonalie2D(a, b);
}

static void earInit(VertexNode2D *verts) {
    VertexNode2D *v0, *v1, *v2;
    v1 = verts;
    do {
        v0 = v1->prev;
        v2 = v1->next;
        v1->ear = diagonal(v0, v2);
        v1 = v1->next;
    } while(v1 != verts);
}

static int* earClipTriangulate(VertexNode2D *vertices, int n) {
    int ntriangles = n - 2;
    int tindex = 0;
    int *triangles = malloc(sizeof(int)*ntriangles*3);
    earInit(vertices);
    while(n > 3) {
        VertexNode2D *v2 = vertices;
        do  {
            if(v2->ear) {
                VertexNode2D *v0, *v1, *v3, *v4;
                v3 = v2->next;
                v4 = v3->next;
                v1 = v2->prev;
                v0 = v1->prev;

                //printf("diagonal: (%d, %d)\n", v1->id, v3->id);
                v1->ear = diagonal(v0, v3);
                v3->ear = diagonal(v1, v4);
                triangles[tindex] = v1->id;
                triangles[tindex + 1] = v2->id;
                triangles[tindex + 2] = v3->id;
                tindex += 3;

                //remove v2
                v1->next = v3;
                v3->prev = v1;
                vertices = v3;
                n--;
                break;
            }
            v2 = v2->next;
        }while(v2 != vertices);
    }
    //printf("ultimo triangle\n");
    //os 3 últimos vertices representam o utlimo triangulo
    triangles[tindex] = vertices->id;
    triangles[tindex + 1] = vertices->next->id;
    triangles[tindex + 2] = vertices->next->next->id;
    return triangles;
}

int* PolygonTriangulation(double *vertices, int nverts) {
    //Projeta os vertices em um plano
    //1. encontrar 2 vetores linearmente independentes no plano
    double iv[3] = {0.0, 0.0, 0.0}, jv[3] = {0.0, 0.0, 0.0}, o[3] = {0.0, 0.0, 0.0};
    for(int i = 1; i < nverts; i++) {
        int prev = i - 1;
        int next = (i + 1)%nverts;
        double v0[3] = { vertices[3*prev], vertices[3*prev + 1], vertices[3*prev + 2]};
        double v1[3] = { vertices[3*i], vertices[3*i + 1], vertices[3*i + 2]};
        double v2[3] = { vertices[3*next], vertices[3*next + 1], vertices[3*next + 2]};

        iv[0] = v2[0] - v1[0]; iv[1] = v2[1] - v1[1]; iv[2] = v2[2] - v1[2];
        jv[0] = v0[0] - v1[0]; jv[1] = v0[1] - v1[1]; jv[2] = v0[2] - v1[2];

        double ivlen = sqrt(iv[0]*iv[0] + iv[1]*iv[1] + iv[2]*iv[2]);
        double jvlen = sqrt(jv[0]*jv[0] + jv[1]*jv[1] + jv[2]*jv[2]);
        iv[0] /= ivlen; iv[1] /= ivlen; iv[2] /= ivlen;
        jv[0] /= jvlen; jv[1] /= jvlen; jv[2] /= jvlen;
        double dotiv = iv[0]*jv[0] + iv[1]*jv[1] + iv[2]*jv[2];

        //verifica se os vetores são linearmente independentes
        if(1.0 - fabs(dotiv) > EPS_FLOAT) {
            o[0] = v1[0]; o[1] = v1[1]; o[2] = v1[2];
            break;
        }

        //verifica se os vetores são linearmente independentes
        /*if((iv[0]/jv[0] != iv[1]/jv[1]) || (iv[0]/jv[0] != iv[2]/jv[2])) {
            double ivlen = sqrt(iv[0]*iv[0] + iv[1]*iv[1] + iv[2]*iv[2]);
            iv[0] /= ivlen; iv[1] /= ivlen; iv[2] /= ivlen;
            double jvlen = sqrt(jv[0]*jv[0] + jv[1]*jv[1] + jv[2]*jv[2]);
            jv[0] /= jvlen; jv[1] /= jvlen; jv[2] /= jvlen;
            o[0] = v1[0]; o[1] = v1[1]; o[2] = v1[2];
            //printf("iv: (%f, %f, %f), jv: (%f, %f, %f)\n", iv[0], iv[1], iv[2], jv[0], jv[1], jv[2]);
            //printf("o: (%f, %f, %f)\n", o[0], o[1], o[2]);
            break;
        }*/
    }

    //2. escrever todos os pontos do poligono em função destes vetores
    double vertices2D[2*nverts];
    for(int i = 0; i < nverts; i++) {
        double vec[3] = { vertices[3*i] - o[0], vertices[3*i + 1] - o[1], vertices[3*i + 2] - o[2]};
        vertices2D[2*i] = iv[0]*vec[0] + iv[1]*vec[1] + iv[2]*vec[2];
        vertices2D[2*i + 1] = jv[0]*vec[0] + jv[1]*vec[1] + jv[2]*vec[2];
    }

    //Constroi lista de vertices
    VertexNode2D *vlist = NULL;
    for(int i = nverts - 1; i  >= 0; i--) {
        VertexNode2D *v = malloc(sizeof(VertexNode2D));
        v->id = i;
        v->coord[0] = vertices2D[2*i];
        v->coord[1] = vertices2D[2*i + 1];
        v->ear = false;
        CDL_PREPEND(vlist, v); 
    }
    //VertexNode2D *v;
    //printf("poligono:\n");
    //CDL_FOREACH(vlist, v) {
    //    printf("%d: (%f, %f), prev: %d, next: %d\n", v->id, v->coord[0], v->coord[1], v->prev->id, v->next->id);
    //}
        
    return earClipTriangulate(vlist, nverts);
}

