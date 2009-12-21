#ifndef _SCENE_H_
#define _SCENE_H_

#include "mesh.h"

typedef struct _scene{
	int nodeCount;
	int textureCount;
	int meshCount;

	mesh* meshes;
}scene;

void initializeTriangles(triangles* tri);

scene* initializeDae(char* filename);

#endif
