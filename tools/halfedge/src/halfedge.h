#ifndef _HALFEDGE_H_
#define _HALFEDGE_H_


/**
 *  TODO HP:
 *      1. Sweeping *
 *      2. Face split *
 *      3. Primitives
 *
 *  TODO LP:
 *      1. Merge Edge and HalfEdge structs
 **/

typedef struct hescene HEScene;
typedef struct hemesh HEMesh;
typedef struct heface HEFace;
typedef struct heloop HELoop;
typedef struct heedge HEEdge;
typedef struct hehalfedge HEHalfEdge;
typedef struct hevertex HEVertex;

struct hescene {
    int nummeshes;
    HEMesh *meshes;
};

struct hemesh {
    int id;
    int numfaces, numedges, numvertices;
    HEFace *faces;
    HEEdge *edges;
    HEVertex *vertices;

    HEMesh *prev, *next;
};

struct heface {
    int id;
    HEMesh *mesh; //back pointer to mesh
    HELoop *lout; //pointer to the outter loop of the face
    HELoop *loops; //list of loops
    HEFace *prev, *next;
};

struct heloop {
    HEHalfEdge *hedge;
    HEFace *face;
    HELoop *prev, *next;
};

struct heedge {
    int id;
    HEHalfEdge *he1, *he2;
    HEEdge *prev, *next;
};

struct hehalfedge {
    HEEdge *edge;
    HEVertex *vertex;
    HELoop *loop;
    HEHalfEdge *next, *prev;
    int id;
};

struct hevertex {
    int id;
    HEHalfEdge *hedge; //ponteiro pra alguma he que sai desse vertice
    float coord[3];
    HEVertex *prev, *next;
};

inline HEHalfEdge *HEMate(HEHalfEdge *he) {
    return (he == he->edge->he1) ? he->edge->he2 : he->edge->he1; 
}

HEMesh *HECreateTriangle();
HEMesh *HECreateQuad();
HEMesh *HECreateQuadXZ(float xmin, float zmin, float xmax, float zmax);
HEMesh *HECreateCube();
HEMesh *HECreateCubeExtrude();


void SplitEdge(HEMesh *m, int eid, float a);
int SplitFace(HEMesh*m, int fid, int v1, int v2);


void ExtrudeFace(HEMesh *m, int fid, float dx, float dy, float dz);


#endif
