#include "mesh.h"
#include "../util/fplist.h"
#include "renderer.h"
#include <stdio.h>

void createVBO(mesh* m){

	printf("criando vbo a:  %d \n" , m->trianglesCount);
	fpnode *n = m->tris->first;
	triangles* tri = NULL;

	//verifica quais vertex attributes existem no mesh e configura a struct attrs
	while( n != NULL ){
		printf("n nao eh null\n");
		tri =  n->data;

		vertexAttribute** attrs = initializeVertexFormat();

		//TODO pode ser short int
		tri->indicesId = initializeVBO(tri->indicesCount*sizeof(unsigned int), GL_STATIC_DRAW, tri->indices);

		int vboSize = tri->verticesCount;
		attrs[ATTR_VERTEX] = malloc(sizeof(vertexAttribute));
		attrs[ATTR_VERTEX]->count = tri->verticesCount;
		attrs[ATTR_VERTEX]->size =  tri->verticesCount*sizeof(float);
		attrs[ATTR_VERTEX]->type = ATTR_VERTEX;
		attrs[ATTR_VERTEX]->offset = 0;
		attrs[ATTR_VERTEX]->components = tri->verticesComponents;
		unsigned int offset = attrs[ATTR_VERTEX]->size;

		if (tri->normals){

			vboSize += tri->normalsCount;
			attrs[ATTR_NORMAL] = malloc(sizeof(vertexAttribute));
			attrs[ATTR_NORMAL]->count = tri->normalsCount;
			attrs[ATTR_NORMAL]->size =  tri->normalsCount*sizeof(float);
			attrs[ATTR_NORMAL]->type = ATTR_NORMAL;
			attrs[ATTR_NORMAL]->offset = offset;
			attrs[ATTR_NORMAL]->components = 3;
			offset += attrs[ATTR_NORMAL]->size;
		}

		if (tri->tangents){
			vboSize += tri->tangentsCount;
			attrs[ATTR_TANGENT] = malloc(sizeof(vertexAttribute));
			attrs[ATTR_TANGENT]->count = tri->tangentsCount;
			attrs[ATTR_TANGENT]->size =  tri->tangentsCount*sizeof(float);
			attrs[ATTR_TANGENT]->type = ATTR_TANGENT;
			attrs[ATTR_TANGENT]->offset = offset;
			attrs[ATTR_TANGENT]->components = 3;
			offset += attrs[ATTR_TANGENT]->size;
		}

		if (tri->binormals){
			vboSize += tri->binormalsCount;
			attrs[ATTR_BINORMAL] = malloc(sizeof(vertexAttribute));
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
			attrs[ATTR_TEXCOORD0+j] = malloc(sizeof(vertexAttribute));
			attrs[ATTR_TEXCOORD0+j]->count = tri->texCoords[j]->count;
			attrs[ATTR_TEXCOORD0+j]->size =  tri->texCoords[j]->count*sizeof(float);
			attrs[ATTR_TEXCOORD0+j]->type = ATTR_TEXCOORD0+j;
			attrs[ATTR_TEXCOORD0+j]->offset = offset;
			attrs[ATTR_TEXCOORD0+j]->components = tri->texCoords[j]->components;
			offset += attrs[ATTR_TEXCOORD0+j]->size;

		}

		printf("alocando alldata\n");
		float* alldata = malloc(sizeof(float)*vboSize);
		int indice = 0;

		printf("vertices \n");
               for (unsigned int k = 0; k < tri->verticesCount; k++, indice++ )
			alldata[indice] = tri->vertices[k];

		printf("normais \n");
		if ( tri->normals )
			for (unsigned int k = 0; k < tri->normalsCount; k++, indice++ )
				alldata[indice] = tri->normals[k];

		printf("tangentes\n");
		if ( tri->tangents ){
			printf("tem tangentes \n");
			for (unsigned int k = 0; k < tri->tangentsCount; k++, indice++ )
				alldata[indice] = tri->tangents[k];
		}

		printf("binormais \n");
		if ( tri->binormals )
			for (unsigned int k = 0; k < tri->binormalsCount; k++, indice++ )
				alldata[indice] = tri->binormals[k];
		
		printf("texcoords\n");
		for(unsigned int l  = 0; l < tri->numTexSets; l++)
			for (unsigned int k = 0; k < tri->texCoords[l]->count; k++, indice++)
				alldata[indice] = tri->texCoords[l]->texCoords[k];


		printf("inicializando vbo  para os dados \n");
		tri->vboId = initializeVBO(vboSize*sizeof(float),GL_STATIC_DRAW, alldata);
		for(int i =0; i < MAX_VERTEX_ATTRS; i++){
			if (attrs[i]){
				attrs[i]->vboID = tri->vboId;
			}
		}
		printf("inicializando vao \n");
		tri->vaoId = initializeIndexedVAO(tri->indicesId, attrs);

		n = n->next;
		printf("vbo criada\n");
	}

}

void setmeshboundingbox(mesh *m) {
    //memset(m->b.pmin, 9999999, 3*sizeof(float));
    //memset(m->b.pmax, -999999, 3*sizeof(float));
    m->b.pmin[0] = 99999999;
    m->b.pmin[1] = 99999999;
    m->b.pmin[2] = 99999999;

    m->b.pmax[0] = -999999999;
    m->b.pmax[1] = -999999999;
    m->b.pmax[2] = -999999999;

    //para cada triangles da lista
    for(int i = 0; i < m->tris->size; i++) {
        triangles *t = fplist_getdata(i, m->tris);
        printf("t->verticesCount: %d\n", t->verticesCount);
        for(int j = 0; j < t->verticesCount/3; j++) {
            //X
            if(t->vertices[3*j] < m->b.pmin[0])
                m->b.pmin[0] = t->vertices[3*j];
            if (t->vertices[3*j] > m->b.pmax[0])
                m->b.pmax[0] = t->vertices[3*j];
            //Y
            if(t->vertices[3*j + 1] < m->b.pmin[1])
                m->b.pmin[1] = t->vertices[3*j + 1];
            if (t->vertices[3*j + 1] > m->b.pmax[1])
                m->b.pmax[1] = t->vertices[3*j + 1];
            //Z
            if(t->vertices[3*j + 2] < m->b.pmin[2])
                m->b.pmin[2] = t->vertices[3*j + 2];
            if (t->vertices[3*j + 2] > m->b.pmax[2])
                m->b.pmax[2] = t->vertices[3*j + 2];
        }
    }
}
