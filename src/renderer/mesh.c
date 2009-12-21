#include "mesh.h"
#include "../util/malloc.h"
#include "renderer.h"

extern renderer* r;

void createVBO(mesh* m){

	for (int i = 0; i < m->trianglesCount; i++){
		
		vertexAttribute** attrs = dlmalloc (sizeof (vertexAttribute*) * 16);
		for (int k = 0; k < 16; k++)
			attrs[k] = NULL;

		m->tris[indicedId].indicesId = initializeVBO( 


	}


}
