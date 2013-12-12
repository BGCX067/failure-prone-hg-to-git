#include "halfedge.h"
#include <stdlib.h>
#include <stdbool.h>
#include "util/utlist.h"

static int nhe = 0;

//Low level functions prototypes
static HEMesh* mvfs(int mid, float x, float y, float z);
static void lmev(HEHalfEdge *he1, HEHalfEdge *he2, float x, float y, float z);
static int lmef(HEHalfEdge *he1, HEHalfEdge *he2);

static bool smev(HEMesh *m, int fid, int vid, float x, float y, float z);
static bool smef(HEMesh *m, int fid, int vid1, int vid2);
//static void sweep(HEFace *f, float dx, float dy, float dz);
//static HEMesh* getHEMesh(HEMesh *head, int mid); 
static HEFace* getHEFace(HEMesh *m, int fid); 
static HEEdge* getHEEdge(HEMesh *m, int eid);
//static HEHalfEdge* getHEd(HEFace *f, int vid1, int vid2); 
static HEHalfEdge* sgetHEd(HEFace *f, int vid); 

//Highest Level functions
HEMesh *HECreateTriangle() {
    HEMesh *t = mvfs(1, -0.5, -0.5, 0.0);
    smev(t, 0, 0, 0.5, -0.5, 0.0);
    smev(t, 0, 1, 0.0, 0.5, 0.0);
    smef(t, 0, 2, 0);

    return t;
}

HEMesh *HECreateQuad() {
    HEMesh *q = mvfs(1, -1.0, -1.0, 0.0);
    smev(q, 0, 0, 1.0, -1.0, 0.0);
    smev(q, 0, 1, 1.0, 1.0, 0.0);
    smev(q, 0, 2, -1.0, 1.0, 0.0);
    smef(q, 0, 3, 0);

    return q;
}

HEMesh *HECreateQuadXZ(float xmin, float zmin, float xmax, float zmax) {
    HEMesh *q = mvfs(1, xmin, 0.0, zmin);
    smev(q, 0, 0, xmin, 0.0, zmax);
    smev(q, 0, 1, xmax, 0.0, zmax);
    smev(q, 0, 2, xmax, 0.0, zmin);
    smef(q, 0, 3, 0);
    return q;
}

HEMesh *HECreateCube() {
    HEMesh *c = mvfs(1, -1.0, -1.0, 0.0);
    smev(c, 0, 0, 1.0, -1.0, 0.0);
    smev(c, 0, 1, 1.0, 1.0, 0.0);
    smev(c, 0, 2, -1.0, 1.0, 0.0);
    smef(c, 0, 3, 0);

    smev(c, 1, 0, -1.0, -1.0, -1.0);
    smev(c, 1, 1, 1.0, -1.0, -1.0);
    smev(c, 1, 2, 1.0, 1.0, -1.0);
    smev(c, 1, 3, -1.0, 1.0, -1.0);

    smef(c, 1, 4, 5);
    smef(c, 2, 5, 6);
    smef(c, 3, 6, 7);
    smef(c, 4, 7, 4);

    return c;
}

//Ao criar um QUAD, duas faces são geradas, uma ccw e uma cw (front and back,
//respectivamente). Aplicar um sweeping na face 0 com valor negativo ou um
//sweeping na face 1 com valor positivo leva a resultados estranhos (cubo com
//faces na orientação errada).
HEMesh *HECreateCubeExtrude() {
    HEMesh *c = mvfs(1, -1.0, -1.0, 0.0);
    smev(c, 0, 0, 1.0, -1.0, 0.0);
    smev(c, 0, 1, 1.0, 1.0, 0.0);
    smev(c, 0, 2, -1.0, 1.0, 0.0);
    smef(c, 0, 3, 0);
    
    //sweep(getHEFace(c, 0), 0.0, 0.0, 3.0);
    ExtrudeFace(c, 0, 0.0, 0.0, 3.0);
    return c;
}

void SplitEdge(HEMesh *m, int eid, float a) {
    HEEdge *e = getHEEdge(m, eid);
    HEVertex *v1 = e->he1->vertex;
    HEVertex *v2 = e->he2->vertex;
    double x = v1->coord[0] + a*(v2->coord[0] - v1->coord[0]);
    double y = v1->coord[1] + a*(v2->coord[1] - v1->coord[1]);
    double z = v1->coord[2] + a*(v2->coord[2] - v1->coord[2]);
    lmev(e->he1, e->he2->next, x, y, z);
}

int SplitFace(HEMesh *m, int fid, int v1, int v2) {
    HEFace *f = getHEFace(m, fid);
    HEHalfEdge *he1 = sgetHEd(f, v1);
    HEHalfEdge *he2 = sgetHEd(f, v2);

    return lmef(he1, he2);
}

typedef enum { PLUS, MINUS }HESign;

extern inline HEHalfEdge* HEMate(HEHalfEdge *he);
static HEMesh *newHEMesh();
static HEFace *newHEFace(HEMesh *m);
static HELoop *newHELoop(HEFace *f);
static HEEdge *newHEEdge(HEMesh *m);
static HEVertex *newHEVertex(HEMesh *m, float x, float y, float z);

static HEHalfEdge* addhe(HEEdge *e, HEVertex *v, HEHalfEdge *h, HESign sign) {
    HEHalfEdge *he;
    if (h->edge == NULL)
        he = h;
    else {
        //Manipulação das listas para colocar a nova halfedge he
        //na frente (antes) da halfedge h na lista duplamente encadeada
        he = malloc(sizeof(HEHalfEdge));
        h->prev->next = he;
        he->prev = h->prev;
        h->prev = he;
        he->next = h;
        he->id = nhe;
        //printf("--criado half edge numero %d\n", nhe++);
    }
    //Ajusta os ponteiros
    he->edge = e;
    he->vertex = v;
    he->loop = h->loop;
    //PLUS -> ccw
    //MINUS -> cw
    if(sign == PLUS) {
        //printf("h%d eh PLUS\n", he->id);
        e->he1 = he;
    } else {
        //printf("h%d eh MINUS\n", he->id);
        e->he2 = he;
    }

    return he;
}

/*static HEHalfEdge* delhe(HEHalfEdge *he) {
    if(he->edge == 0) {
        //delete he;
        free(he);
        return NULL;
    } else if (he->next == he) {
        he->edge = 0;
        return he;
    }
    else {
        he->prev->next = he->next;
        he->next->prev = he->prev;
        //delete he;
        free(he);
        return he->prev;
    }
}*/

static HEMesh* mvfs(int mid, float x, float y, float z) {
    HEMesh *m = newHEMesh();
    HEFace *f = newHEFace(m);
    HELoop *l = newHELoop(f);
    HEVertex *v = newHEVertex(m, x, y, z);
    HEHalfEdge *he = malloc(sizeof(HEHalfEdge));
    
    m->id = mid;
    f->lout = l;
    l->hedge = he;
    he->loop = l;
    he->next = he->prev = he;
    he->vertex = v;
    he->edge = NULL;
    //v->hedge = he;
    he->id = nhe;
    //printf("mvfs\n");
    //printf("--criado half edge numero %d\n", nhe++);
    return m;
}

static void lmev(HEHalfEdge *he1, HEHalfEdge *he2,
                 float x, float y, float z) {
    //printf("lmev\n");
    //printf("he1: %d\n", he1->id);
    //printf("he2: %d\n", he2->id);
    HEVertex *v = newHEVertex(he1->loop->face->mesh, x, y, z);
    HEEdge *e = newHEEdge(he1->loop->face->mesh);

    HEHalfEdge *he = he1;
    //percorre todas as half edges que saem do mesmo vertice que
    while(he != he2) {
        he->vertex = v;
        he = HEMate(he)->next;
    }

    addhe(e, he2->vertex, he1, MINUS);
    addhe(e, v, he2, PLUS);

    v->hedge = he2->prev;
    he2->vertex->hedge = he2;
}

static int lmef(HEHalfEdge *he1, HEHalfEdge *he2) {
    //printf("lmef\n");
    //printf("he1: %d\n", he1->id);
    //printf("he2: %d\n", he2->id);
    HEFace *f = newHEFace(he1->loop->face->mesh);
    HELoop *l = newHELoop(f);
    HEEdge *e = newHEEdge(he1->loop->face->mesh);
    HEHalfEdge *he, *nhe1, *nhe2, *temp;

    f->lout = l;

    he = he1;
    while(he != he2) {
        he->loop = l;
        he = he->next;
    }

    nhe1 = addhe(e, he2->vertex, he1, PLUS);
    nhe2 = addhe(e, he1->vertex, he2, MINUS);

    nhe1->prev->next = nhe2;
    nhe2->prev->next = nhe1;
    temp = nhe1->prev;
    nhe1->prev = nhe2->prev;
    nhe2->prev = temp;

    l->hedge = nhe1;
    he2->loop->hedge = nhe2;

    return f->id;
}

/*static bool mev(HEMesh *m, int fid1, int fid2, int vid1, int vid2, 
                int vid3, float x, float y, float z) {
    //HEMesh *s;
    HEFace *f1, *f2;
    HEHalfEdge *he1, *he2;

    //if((s = getHEMesh(mid)) == NULL)
    //    return false;
    if((f1 = getHEFace(m, fid1)) == NULL)
        return false;
    if((f2 = getHEFace(m, fid2)) == NULL)
        return false;
    if((he1 = getHEd(f1, vid1, vid2)) == NULL)
        return false;
    if((he2 = getHEd(f2, vid1, vid3)) == NULL)
        return false;
    lmev(he1, he2, x, y, z);
    return true;
}*/

static bool smev(HEMesh *m, int fid, int vid, float x, float y, float z) {
    //HEMesh *s;
    HEFace *f;
    HEHalfEdge *he;

    //if((s = getHEMesh(mid)) == NULL)
    //    return false;
    if((f = getHEFace(m, fid)) == NULL)
        return false;
    if((he = sgetHEd(f, vid)) == NULL)
        return false;

    lmev(he, he, x, y, z);
    return true;
}

/*static bool mef(HEMesh *m, int fid, int vid1, int vid2, 
                int vid3, int vid4) {
    //HEMesh *s;
    HEFace *f;
    HEHalfEdge *he1, *he2;

    //if((s = getHEMesh(mid)) == NULL)
    //    return false;
    if((f = getHEFace(m, fid)) == NULL)
        return false;
    if((he1 = getHEd(f, vid1, vid2)) == NULL)
        return false;
    if((he2 = getHEd(f, vid3, vid4)) == NULL)
        return false;

    lmef(he1, he2);
    return true;
}*/

static bool smef(HEMesh *m, int fid, int vid1, int vid2) {
    //HEMesh *s;
    HEFace *f;
    HEHalfEdge *he1, *he2;

    //if((s = getHEMesh(mid)) == NULL)
    //    return false;
    if((f = getHEFace(m, fid)) == NULL)
        return false;
    if((he1 = sgetHEd(f, vid1)) == NULL)
        return false;

    //if((he2 = getHEd(f, idVertex3, idVertex2)) == NULL)
    //    return false;
    he2 = he1;
    if(he2->vertex->id != vid2)
        do {
            he2 = he2->next;
            if (he2 == he1)
                return false;
        } while(he2->vertex->id != vid2);

    lmef(he1, he2);
    return true;
}

//TODO receber ID?
static HEMesh *newHEMesh() {
    HEMesh *m = malloc(sizeof(HEMesh));
    m->faces = NULL;
    m->edges = NULL;
    m->vertices = NULL;
    m->numfaces = 0;
    m->numedges = 0;
    m->numvertices = 0;

    return m;
}

static HEFace *newHEFace(HEMesh *m) {
    HEFace *f = malloc(sizeof(HEFace));
    f->mesh = m;
    f->lout = NULL;
    f->loops = NULL;
    f->id = m->numfaces++;
    DL_APPEND(m->faces, f); 
    return f;
}

static HELoop *newHELoop(HEFace *f) {
    HELoop *l = malloc(sizeof(HELoop));
    l->face = f;
    l->hedge = NULL;
    DL_APPEND(f->loops, l); 
    return l;
}
static HEEdge *newHEEdge(HEMesh *m) {
    HEEdge *e = malloc(sizeof(HEEdge));
    e->he1 = NULL;
    e->he2 = NULL;
    e->id = m->numedges++;
    DL_APPEND(m->edges, e); 
    return e;
}

static HEVertex *newHEVertex(HEMesh *m, float x, float y, float z) {
    HEVertex *v = malloc(sizeof(HEVertex));
    v->hedge = NULL;
    v->id = m->numvertices++;
    v->coord[0] = x;
    v->coord[1] = y;
    v->coord[2] = z;
    DL_APPEND(m->vertices, v); 
    return v;
}

//static HEMesh* getHEMesh(HEMesh *head, int mid) {
//    HEMesh *it;
//    DL_FOREACH(head, it) {
//        if(it->id == mid)
//            return it;
//    }
//    return NULL;
//}

static HEFace* getHEFace(HEMesh *m, int fid) {
    HEFace *it;
    DL_FOREACH(m->faces, it) {
        if(it->id == fid)
            return it;
    }
    return NULL;
}


static HEEdge* getHEEdge(HEMesh *m, int eid) {
    HEEdge *it;
    DL_FOREACH(m->edges, it) {
        if(it->id == eid)
            return it;
    }
    return NULL;
}

/*static HEHalfEdge* getHEd(HEFace *f, int vid1, int vid2) {
    HELoop *it;
    DL_FOREACH(f->loops, it) {
        HEHalfEdge *h = it->hedge;
        do {
            if(h->vertex->id == vid1 &&
                    h->next->vertex->id == vid2)
                return h;
        } while((h = h->next) != it->hedge);
    }
    return NULL;
}*/

//retorna
static HEHalfEdge* sgetHEd(HEFace *f, int vid) {
    HELoop *it;
    DL_FOREACH(f->loops, it) {
        HEHalfEdge *h = it->hedge;
        do {
            if(h->vertex->id == vid)
                return h;
        } while((h = h->next) != it->hedge);
    }
    return NULL;
}


void ExtrudeFace(HEMesh *m, int fid, float dx, float dy, float dz)  {
    HEFace *f = getHEFace(m, fid);
    HELoop *l = f->lout;

    HEHalfEdge *first = l->hedge;
    HEHalfEdge *scan = first->next;
    HEVertex *v = scan->vertex;

    lmev(scan, scan, v->coord[0] + dx, v->coord[1] + dy, v->coord[2] + dz);
    while(scan != first) {
        v = scan->next->vertex;
        lmev(scan->next, scan->next, v->coord[0] + dx, v->coord[1] + dy, v->coord[2] + dz);
        lmef(scan->prev, scan->next->next);
        scan = HEMate(scan->next)->next;
    }
    lmef(scan->prev, scan->next->next);
}
