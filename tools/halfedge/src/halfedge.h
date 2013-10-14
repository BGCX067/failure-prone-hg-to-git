#ifndef _HALFEDGE_H_
#define _HALFEDGE_H_


/**
 *  TODO:
 *      1. Merge Edge and HalfEdge structs
 **/

typedef struct hemesh HEMesh;
typedef struct heface HEFace;
typedef struct heloop HELoop;
typedef struct heedge HEEdge;
typedef struct hehalfedge HEHalfEdge;
typedef struct hevertex HEVertex;

struct hemesh {
    unsigned int id;
    HEFace *faces;
    HEEdge *edges;
    HEVertex *vertices;

    HEMesh *prev, *next;
};

struct heface {
    unsigned int id;
    HEMesh *fmesh; //back pointer to mesh
    HELoop *flout; //pointer to the outter loop of the face
    HELoop *loops; //list of loops
    HEFace *prev, *next;
};

struct heloop {
    HEHalfEdge *ledge;
    HEFace *lface;
    HELoop *prev, *next;
};

struct heedge {
    HEHalfEdge *he1, *he2;
    HEEdge *prev, *next;
};

struct hehalfedge {
    unsigned int id;
    HEEdge *edge;
    HEVertex *vertex;
    HELoop *loop;
    HEHalfEdge *next, *prev;
};

struct hevertex {
    unsigned int id;
    HEHalfEdge *vedge; //ponteiro pra he que sai desse vertice
    float vcoord[3];
    HEVertex *prev, *next;
};

inline HEHalfEdge *HEMate(HEHalfEdge *he) {
    return (he == he->edge->he1) ? he->edge->he2 : he->edge->he1; 
}

#endif
