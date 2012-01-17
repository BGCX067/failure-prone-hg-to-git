#ifndef _SCENE_H_
#define _SCENE_H_

#include "mesh.h"
#include "material.h"
#include "../util/fplist.h"
#include "../math/boundingbox.h"
#include "light.h"
#include "../math/matrix.h"

typedef struct _node{
	Mesh* model;
    mat4 transformation;
	struct _node* parent;
	struct _node** children;
} Node;

typedef struct _scene{
	int nodeCount;
	int meshCount;

	//fplist* meshes;
    fplist* meshList;
	fplist* lightList;
    fplist* materialList;
    fplist* texList;
    
    fplist* nodes;

    BoundingBox b;
}Scene;

//void initializeTriangles(Triangles* tri);

int addMesh(Scene *s, Mesh *m);
int addLight(Scene *s, Light *l);
int addMaterial(Scene *s, Material *m);
int addTexture(Scene *s, Texture *t);
int addNode(Scene *s, Node *m);

//Scene* initializeDae(char* filename);

Scene* initializeScene();

#endif
