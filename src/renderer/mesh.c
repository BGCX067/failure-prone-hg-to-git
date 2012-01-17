#include "mesh.h"
#include "../util/fplist.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

void createVBO(Mesh* m){
	printf("criando vbo a:  %d \n" , m->trianglesCount);
	fpnode *n = m->tris->first;
	Triangles* tri = NULL;

	//verifica quais vertex attributes existem no mesh e configura a struct attrs
	while( n != NULL ){
		printf("n nao eh null\n");
		tri =  n->data;

		VertexAttribute** attrs = initializeVertexFormat();

		//TODO pode ser short int
		tri->indicesId = initializeVBO(tri->indicesCount*sizeof(unsigned int), GL_STATIC_DRAW, tri->indices);

		int vboSize = tri->verticesCount;
		attrs[ATTR_VERTEX] = malloc(sizeof(VertexAttribute));
		attrs[ATTR_VERTEX]->count = tri->verticesCount;
		attrs[ATTR_VERTEX]->size =  tri->verticesCount*sizeof(float);
		attrs[ATTR_VERTEX]->type = ATTR_VERTEX;
		attrs[ATTR_VERTEX]->offset = 0;
		attrs[ATTR_VERTEX]->components = tri->verticesComponents;
		unsigned int offset = attrs[ATTR_VERTEX]->size;

		if (tri->normals){

			vboSize += tri->normalsCount;
			attrs[ATTR_NORMAL] = malloc(sizeof(VertexAttribute));
			attrs[ATTR_NORMAL]->count = tri->normalsCount;
			attrs[ATTR_NORMAL]->size =  tri->normalsCount*sizeof(float);
			attrs[ATTR_NORMAL]->type = ATTR_NORMAL;
			attrs[ATTR_NORMAL]->offset = offset;
			attrs[ATTR_NORMAL]->components = 3;
			offset += attrs[ATTR_NORMAL]->size;
		}

		if (tri->tangents){
			vboSize += tri->tangentsCount;
			attrs[ATTR_TANGENT] = malloc(sizeof(VertexAttribute));
			attrs[ATTR_TANGENT]->count = tri->tangentsCount;
			attrs[ATTR_TANGENT]->size =  tri->tangentsCount*sizeof(float);
			attrs[ATTR_TANGENT]->type = ATTR_TANGENT;
			attrs[ATTR_TANGENT]->offset = offset;
			attrs[ATTR_TANGENT]->components = 3;
			offset += attrs[ATTR_TANGENT]->size;
		}

		if (tri->binormals){
			vboSize += tri->binormalsCount;
			attrs[ATTR_BINORMAL] = malloc(sizeof(VertexAttribute));
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
			attrs[ATTR_TEXCOORD0+j] = malloc(sizeof(VertexAttribute));
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
		//tri->vaoId = initializeIndexedVAO(tri->indicesId, attrs);
        tri->vaoId = initEmptyVAO();
        configureIndexedVAO(tri->vaoId, tri->indicesId, attrs);


		n = n->next;
		printf("vbo criada\n");
	}

}

void setmeshboundingbox(Mesh *m) {
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
        Triangles *t = fplist_getdata(i, m->tris);
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

Mesh* initMesh() {
    Mesh *m = malloc(sizeof(Mesh));
    m->tris = fplist_init(free);
    return m;
}

Triangles* addTris(Mesh *m) {
    Triangles *t = malloc(sizeof(Triangles));
    memset(t, 0, sizeof(Triangles));
    fplist_insback(t, m->tris);

    return t;
}

void addVertices(Triangles *t, int num, int comp, float *vertices) {
    t->verticesCount = num;
    
    //só armazena o ponteiro
    t->vertices = vertices;

    t->attrs[ATTR_VERTEX] = malloc(sizeof(VertexAttribute));
    t->attrs[ATTR_VERTEX]->count = t->verticesCount;
    t->attrs[ATTR_VERTEX]->size =  t->verticesCount*sizeof(float);
    t->attrs[ATTR_VERTEX]->type = ATTR_VERTEX;
    t->attrs[ATTR_VERTEX]->offset = 0;
    t->attrs[ATTR_VERTEX]->components = comp;
}

void addNormals(Triangles* t, int num, int comp, float *normals) {
    t->normalsCount = num;
    t->normals = normals;

    t->attrs[ATTR_NORMAL] = malloc(sizeof(VertexAttribute));
    t->attrs[ATTR_NORMAL]->count = t->normalsCount;
    t->attrs[ATTR_NORMAL]->size =  t->normalsCount*sizeof(float);
    t->attrs[ATTR_NORMAL]->type = ATTR_NORMAL;
    t->attrs[ATTR_NORMAL]->offset = 0;
    t->attrs[ATTR_NORMAL]->components = comp;
}

void addTexCoords(Triangles *t, int num, int comp, int texset, float *texcoords) {
    t->texCoords[texset] = malloc(sizeof(TexCoord));
    t->texCoords[texset]->count = num;
    t->texCoords[texset]->components = comp;
    t->texCoords[texset]->set = texset;
    t->texCoords[texset]->texCoords = texcoords;
    t->numTexSets++;

    t->attrs[ATTR_TEXCOORD0 + texset] = malloc(sizeof(VertexAttribute));
    t->attrs[ATTR_TEXCOORD0 + texset]->count = t->texCoords[texset]->count;
    t->attrs[ATTR_TEXCOORD0 + texset]->size =  t->texCoords[texset]->count*sizeof(float);
    t->attrs[ATTR_TEXCOORD0 + texset]->type = ATTR_TEXCOORD0 + texset;
    t->attrs[ATTR_TEXCOORD0 + texset]->offset = 0;
    t->attrs[ATTR_TEXCOORD0 + texset]->components = t->texCoords[texset]->components;
}

void addIndices(Triangles *t, int num, unsigned int *indices) {
    t->indicesCount = num;
    t->indices = indices;
}

void prepareMesh(Mesh *m) {
    //Para cada triangles
    
    for(fpnode *n = m->tris->first; n != NULL; n = n->next) {
        Triangles *tri = n->data;
        
        //Criar VBO de indices
        tri->indicesId = initializeVBO(tri->indicesCount*sizeof(unsigned int), GL_STATIC_DRAW, tri->indices);
        tri->verticesVBO = initializeVBO(tri->verticesCount*sizeof(float), GL_STATIC_DRAW, tri->vertices);
        tri->attrs[ATTR_VERTEX]->vboID = tri->verticesVBO;

        if(tri->normals) {
            tri->normalsVBO = initializeVBO(tri->normalsCount*sizeof(float), GL_STATIC_DRAW, tri->normals);
        }
        if(tri->binormals)
            tri->binormalsVBO = initializeVBO(tri->binormalsCount*sizeof(float), GL_STATIC_DRAW, tri->binormals);
        if(tri->tangents)
            tri->tangentsVBO = initializeVBO(tri->tangentsCount*sizeof(float), GL_STATIC_DRAW, tri->tangents);
        for(unsigned int i = 0; i < tri->numTexSets; i++)
            tri->texVBO[i] = initializeVBO(tri->texCoords[i]->count*sizeof(float), GL_STATIC_DRAW, tri->texCoords[i]->texCoords);
        tri->vaoId = initEmptyVAO();
        configureIndexedVAO(tri->vaoId, tri->indicesId, tri->attrs);
        
    }
}

