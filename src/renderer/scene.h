#ifndef _SCENE_H_
#define _SCENE_H_

#include "mesh.h"
#include "material.h"
#include "../util/fplist.h"
#include "../math/vec3.h"
#include "../math/boundingbox.h"
#include "light.h"

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
	fplist* lights;
	fplist* nodes;

    boundingbox b;
}scene;

void initializeTriangles(triangles* tri);

int addMesh(scene* s, mesh *m);
int addNode(scene* s, node *m);
int addLight(scene* s, light* l);

scene* initializeDae(char* filename);

scene* initializeScene();

#endif
