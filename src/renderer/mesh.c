#include "mesh.h"
#include "../util/malloc.h"
#include "renderer.h"
#include <stdio.h>

void createVBO(mesh* m){

	printf("criando vbo a\n"); fflush(stdout);
	for (int i = 0; i < m->trianglesCount; i++){
		
		vertexAttribute** attrs = dlmalloc (sizeof (vertexAttribute*) * 16);
		//FIXME substituir por um memset
		for (int k = 0; k < 16; k++)
			attrs[k] = NULL;

		//TODO pode ser short int
		m->tris[i].indicesId = initializeVBO(m->tris[i].indicesCount*sizeof(unsigned int), m->tris[i].indices);

		int vboSize = m->tris[i].verticesCount;
		attrs[ATTR_VERTEX] = dlmalloc(sizeof(vertexAttribute));
		attrs[ATTR_VERTEX]->count = m->tris[i].verticesCount;
		attrs[ATTR_VERTEX]->size =  m->tris[i].verticesCount*sizeof(float);
		attrs[ATTR_VERTEX]->type = ATTR_VERTEX;
		attrs[ATTR_VERTEX]->offset = 0;
		attrs[ATTR_VERTEX]->components = 3;
		unsigned int offset = attrs[ATTR_VERTEX]->size;

		if (m->tris[i].normals){

			vboSize += m->tris[i].normalsCount;
			attrs[ATTR_NORMAL] = dlmalloc(sizeof(vertexAttribute));
			attrs[ATTR_NORMAL]->count = m->tris[i].normalsCount;
			attrs[ATTR_NORMAL]->size =  m->tris[i].normalsCount*sizeof(float);
			attrs[ATTR_NORMAL]->type = ATTR_NORMAL;
			attrs[ATTR_NORMAL]->offset = offset;
			attrs[ATTR_NORMAL]->components = 3;
			offset += attrs[ATTR_NORMAL]->size;
		}

		if (m->tris[i].tangents){
			vboSize += m->tris[i].tangentsCount;
			attrs[ATTR_TANGENT] = dlmalloc(sizeof(vertexAttribute));
			attrs[ATTR_TANGENT]->count = m->tris[i].tangentsCount;
			attrs[ATTR_TANGENT]->size =  m->tris[i].tangentsCount*sizeof(float);
			attrs[ATTR_TANGENT]->type = ATTR_TANGENT;
			attrs[ATTR_TANGENT]->offset = offset;
			attrs[ATTR_TANGENT]->components = 3;
			offset += attrs[ATTR_TANGENT]->size;
		}

		if (m->tris[i].binormals){
			vboSize += m->tris[i].binormalsCount;
			attrs[ATTR_BINORMAL] = dlmalloc(sizeof(vertexAttribute));
			attrs[ATTR_BINORMAL]->count = m->tris[i].binormalsCount;
			attrs[ATTR_BINORMAL]->size =  m->tris[i].binormalsCount*sizeof(float);
			attrs[ATTR_BINORMAL]->type = ATTR_BINORMAL;
			attrs[ATTR_BINORMAL]->offset = offset;
			attrs[ATTR_BINORMAL]->components = 3;
			offset += attrs[ATTR_BINORMAL]->size;
		}

		for( int j = 0; j < m->tris[i].numTexSets; j++){
			vboSize += m->tris[i].texCoords[j]->count;
			attrs[ATTR_TEXCOORD0+j] = dlmalloc(sizeof(vertexAttribute));
			attrs[ATTR_TEXCOORD0+j]->count = m->tris[i].texCoords[j]->count;
			attrs[ATTR_TEXCOORD0+j]->size =  m->tris[i].texCoords[j]->count*sizeof(float);
			attrs[ATTR_TEXCOORD0+j]->type = ATTR_TEXCOORD0+j;
			attrs[ATTR_TEXCOORD0+j]->offset = offset;
			attrs[ATTR_TEXCOORD0+j]->components = m->tris[i].texCoords[j]->components;
			offset += attrs[ATTR_TEXCOORD0+j]->size;

		}

		printf("alocando alldata\n");
		float* alldata = dlmalloc(sizeof(float)*vboSize);
		int indice = 0;

               for (unsigned int k = 0; k < m->tris[i].verticesCount; k++, indice++ )
			alldata[indice] = m->tris[i].vertices[k];

		if ( m->tris[i].normals )
			for (unsigned int k = 0; k < m->tris[i].normalsCount; k++, indice++ )
				alldata[indice] = m->tris[i].normals[k];

		if ( m->tris[i].tangents )
			for (unsigned int k = 0; k < m->tris[i].tangentsCount; k++, indice++ )
				alldata[indice] = m->tris[i].tangents[k];

		if ( m->tris[i].binormals )
			for (unsigned int k = 0; k < m->tris[i].binormalsCount; k++, indice++ )
				alldata[indice] = m->tris[i].binormals[k];
		
		for(unsigned int l  = 0; l < m->tris[i].numTexSets; l++)
			for (unsigned int k = 0; k < m->tris[i].texCoords[l]->count; k++, indice++)
				alldata[indice] = m->tris[i].texCoords[l]->texCoords[k];


		m->tris[i].vboId = initializeVBO(vboSize*sizeof(float), alldata);
		m->tris[i].vertexFormatId  = addVertexFormat(attrs, m->tris[i].totalAttrs);

		printf("vbo criada\n");
	}

}
