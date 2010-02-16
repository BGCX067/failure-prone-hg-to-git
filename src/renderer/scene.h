#ifndef _SCENE_H_
#define _SCENE_H_

#include "mesh.h"
#include "../util/fplist.h"
#include "../math/vec3.h"

typedef struct _node{
	mesh* model;

	vec3 pos;	

	struct _node* child;
} node;

typedef struct _scene{
	int nodeCount;
	int textureCount;
	int meshCount;

	fplist* meshes;

	fplist* nodes;
}scene;

void initializeTriangles(triangles* tri);

int addMesh(scene* s, mesh *m);
int addMesh(scene* s, mesh *m);

scene* initializeDae(char* filename);

scene* initializeScene();

#endif
