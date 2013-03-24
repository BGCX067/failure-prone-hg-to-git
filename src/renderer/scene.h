#ifndef _SCENE_H_
#define _SCENE_H_

#include "mesh.h"
#include "material.h"
#include "math/boundingbox.h"
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
    Material *materialList;
    //fplist* texList;
    //fplist* nodes;

    BoundingBox b;
}Scene;

//void initializeTriangles(Triangles* tri);
//int scaddTris(Scene *s, Triangles *t);
void addMesh(Scene *s, Mesh *m);
void addLight(Scene *s, Light *l);
void addMaterial(Scene *s, Material *m);
//int addTexture(Scene *s, Texture *t);
//int addNode(Scene *s, Node *m);

void rmMesh(Scene *s, Mesh *m);

void sceneSetBoundingBox(Scene *s);
//Scene* initializeDae(char* filename);

Scene* initializeScene();

void drawScene(Scene* s);

#endif
