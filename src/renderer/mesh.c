#include "mesh.h"
#include "../util/malloc.h"
#include "../util/fplist.h"
#include "renderer.h"
#include <stdio.h>

void createVBO(mesh* m){

	printf("criando vbo a:  %d \n" , m->trianglesCount); fflush(stdout);
	fpnode *n = m->tris->first;
	triangles* tri = NULL;
	while( n != NULL ){
		printf("n nao eh null\n");
		tri =  n->data;

		vertexAttribute** attrs = dlmalloc (sizeof (vertexAttribute*) * 16);
		//FIXME substituir por um memset
		for (int k = 0; k < 16; k++)
			attrs[k] = NULL;

		//TODO pode ser short int
		tri->indicesId = initializeVBO(tri->indicesCount*sizeof(unsigned int), tri->indices);

		int vboSize = tri->verticesCount;
		attrs[ATTR_VERTEX] = dlmalloc(sizeof(vertexAttribute));
		attrs[ATTR_VERTEX]->count = tri->verticesCount;
		attrs[ATTR_VERTEX]->size =  tri->verticesCount*sizeof(float);
		attrs[ATTR_VERTEX]->type = ATTR_VERTEX;
		attrs[ATTR_VERTEX]->offset = 0;
		attrs[ATTR_VERTEX]->components = tri->verticesComponents;
		unsigned int offset = attrs[ATTR_VERTEX]->size;

		if (tri->normals){

			vboSize += tri->normalsCount;
			attrs[ATTR_NORMAL] = dlmalloc(sizeof(vertexAttribute));
			attrs[ATTR_NORMAL]->count = tri->normalsCount;
			attrs[ATTR_NORMAL]->size =  tri->normalsCount*sizeof(float);
			attrs[ATTR_NORMAL]->type = ATTR_NORMAL;
			attrs[ATTR_NORMAL]->offset = offset;
			attrs[ATTR_NORMAL]->components = 3;
			offset += attrs[ATTR_NORMAL]->size;
		}

		if (tri->tangents){
			vboSize += tri->tangentsCount;
			attrs[ATTR_TANGENT] = dlmalloc(sizeof(vertexAttribute));
			attrs[ATTR_TANGENT]->count = tri->tangentsCount;
			attrs[ATTR_TANGENT]->size =  tri->tangentsCount*sizeof(float);
			attrs[ATTR_TANGENT]->type = ATTR_TANGENT;
			attrs[ATTR_TANGENT]->offset = offset;
			attrs[ATTR_TANGENT]->components = 3;
			offset += attrs[ATTR_TANGENT]->size;
		}

		if (tri->binormals){
			vboSize += tri->binormalsCount;
			attrs[ATTR_BINORMAL] = dlmalloc(sizeof(vertexAttribute));
			attrs[ATTR_BINORMAL]->count = tri->binormalsCount;
			attrs[ATTR_BINORMAL]->size =  tri->binormalsCount*sizeof(float);
			attrs[ATTR_BINORMAL]->type = ATTR_BINORMAL;
			attrs[ATTR_BINORMAL]->offset = offset;
			attrs[ATTR_BINORMAL]->components = 3;
			offset += attrs[ATTR_BINORMAL]->size;
		}

		for( int j = 0; j < tri->numTexSets; j++){
			printf("configurando texset: %d\n", j);
			vboSize += tri->texCoords[j]->count;
			attrs[ATTR_TEXCOORD0+j] = dlmalloc(sizeof(vertexAttribute));
			attrs[ATTR_TEXCOORD0+j]->count = tri->texCoords[j]->count;
			attrs[ATTR_TEXCOORD0+j]->size =  tri->texCoords[j]->count*sizeof(float);
			attrs[ATTR_TEXCOORD0+j]->type = ATTR_TEXCOORD0+j;
			attrs[ATTR_TEXCOORD0+j]->offset = offset;
			attrs[ATTR_TEXCOORD0+j]->components = tri->texCoords[j]->components;
			offset += attrs[ATTR_TEXCOORD0+j]->size;

		}

		printf("alocando alldata\n");
		float* alldata = dlmalloc(sizeof(float)*vboSize);
		int indice = 0;

               for (unsigned int k = 0; k < tri->verticesCount; k++, indice++ )
			alldata[indice] = tri->vertices[k];

		if ( tri->normals )
			for (unsigned int k = 0; k < tri->normalsCount; k++, indice++ )
				alldata[indice] = tri->normals[k];

		if ( tri->tangents )
			for (unsigned int k = 0; k < tri->tangentsCount; k++, indice++ )
				alldata[indice] = tri->tangents[k];

		if ( tri->binormals )
			for (unsigned int k = 0; k < tri->binormalsCount; k++, indice++ )
				alldata[indice] = tri->binormals[k];
		
		for(unsigned int l  = 0; l < tri->numTexSets; l++)
			for (unsigned int k = 0; k < tri->texCoords[l]->count; k++, indice++)
				alldata[indice] = tri->texCoords[l]->texCoords[k];


		tri->vboId = initializeVBO(vboSize*sizeof(float), alldata);
		tri->vertexFormatId  = addVertexFormat(attrs, tri->totalAttrs);

		n = n->next;
		printf("vbo criada\n");
	}

}
