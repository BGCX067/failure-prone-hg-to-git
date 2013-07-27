#ifndef _SCENE_H_
#define _SCENE_H_

#include "mesh.h"
//#include "material.h"
#include "renderer.h"
#include "math/mathutil.h"
#include "light.h"
#include "math/matrix.h"

typedef struct _node{
	Mesh* model;
    mat4 transformation;
	struct _node* parent;
	struct _node** children;
} Node;

typedef struct _scene{
	int nodeCount;
	int meshCount;

    //TODO animatedMeshList
    Mesh *meshList;
    //MeshElem *meshList;
    Light *lightList;
    Shader *materialList;
    //fplist* texList;
    //fplist* nodes;

    BBox b;
}Scene;

//void initializeTriangles(Triangles* tri);
//int scaddTris(Scene *s, Triangles *t);
void AddMesh(Scene *s, Mesh *m);
void AddLight(Scene *s, Light *l);
void AddMaterial(Scene *s, Shader *m);
//int addTexture(Scene *s, Texture *t);
//int addNode(Scene *s, Node *m);
void RmMesh(Scene *s, Mesh *m);
void CalcBBoxs(Scene *s);
//Scene* initializeDae(char* filename);
Scene* InitializeScene();

void DrawScene(Scene* s);

#endif
