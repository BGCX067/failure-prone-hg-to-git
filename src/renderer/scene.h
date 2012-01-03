#ifndef _SCENE_H_
#define _SCENE_H_

#include "mesh.h"
#include "material.h"
#include "../util/fplist.h"
#include "../math/vec3.h"
#include "../math/boundingbox.h"
#include "light.h"

typedef struct _node{
	Mesh* model;
	vec3 pos;	
	struct _node* child;
} Node;

typedef struct _scene{
	int nodeCount;
	int textureCount;
	int meshCount;

	fplist* meshes;
	fplist* lights;
	fplist* nodes;

    boundingbox b;
}Scene;

void initializeTriangles(Triangles* tri);

int addMesh(Scene* s, Mesh *m);
int addNode(Scene* s, Node *m);
int addLight(Scene* s, Light* l);

Scene* initializeDae(char* filename);

Scene* initializeScene();

#endif
