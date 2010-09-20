#include "glime.h"
#include <stdio.h>
#include <GL/gl.h>
#include "renderer.h"
//#include "glextensions.h"

void initializeBatch(batch* b){

	b->vertexVBO = 0;
	b->normalVBO = 0;
	b->colorVBO = 0;
	
	b->vaoid = 0;
	
	b->numTexSets = 0;
	b->numVerts = 0;
	b->verticesCount = 0;
	
	b->vertices = NULL;
	b->normals = NULL;
	b->colors = NULL;
	b->texCoords = NULL;
	
}

void begin(batch *b, int primitive, int nverts, int texSets){
	
	b->primitive = primitive;
	b->numVerts = nverts;
	b->numTexSets = texSets;

	b->texCoordsVBO = malloc(sizeof(unsigned int)*texSets);
	b->texCoords = malloc(sizeof(float*)*texSets);
	memset(b->texCoordsVBO, 0, sizeof(unsigned int)*texSets );
	memset(b->texCoords, NULL, sizeof(float*)*texSets);

	b->vaoid = createVAO();	
}

void vertex3f(batch* b, float x, float y, float z){

	if (b->vertexVBO == 0)
		b->vertexVBO = initializeVBO(sizeof(float)*3*b->numVerts, GL_DYNAMIC_DRAW, NULL);
	
	if (b->vertices == NULL)
		b->vertices = (float*) mapVBO(b->vertexVBO, GL_WRITE_ONLY);
		
	if (b->verticesCount >= b->numVerts)
		return;
	
	b->vertices[b->verticesCount*3] = x;
	b->vertices[b->verticesCount*3+1] = y;
	b->vertices[b->verticesCount*3+2] = z;
	b->verticesCount++;
	
}

void normal3f(batch *b, float x, float y, float z){

	if (b->normalVBO == 0)
		b->normalVBO = initializeVBO(sizeof(float)*3*b->numVerts, GL_DYNAMIC_DRAW, NULL);

	if (b->normals == NULL)
		b->normals = (float*) mapVBO(b->normalVBO, GL_WRITE_ONLY);

	if (b->verticesCount >= b->numVerts)
		return;

	b->normals[b->verticesCount*3] = x;
	b->normals[b->verticesCount*3+1] = y;
	b->normals[b->verticesCount*3+2] = z;

}

void color4f(batch* b, float x, float y, float z, float w){

	if (b->colorVBO == 0)
		b->colorVBO = initializeVBO(sizeof(float)*4*b->numVerts, GL_DYNAMIC_DRAW, NULL);

	if (b->colors == NULL)
		b->colors = (float*) mapVBO(b->colorVBO, GL_WRITE_ONLY);

	if (b->verticesCount >= b->numVerts)
		return;

	b->colors[b->verticesCount*3] = x;
	b->colors[b->verticesCount*3+1] = y;
	b->colors[b->verticesCount*3+2] = y;
	b->colors[b->verticesCount*3+3] = w;

}

void texCoord2f(batch* b, unsigned int texUnit, float s, float t){

	if (b->texCoordsVBO[texUnit] == 0){
		b->texCoordsVBO[texUnit] = initializeVBO(sizeof(float)*2*b->numVerts, GL_DYNAMIC_DRAW, NULL);
		printf("initialize texcoords vbo: %d \n", b->texCoordsVBO[texUnit] );
	}

	if (b->texCoords[texUnit] == NULL){
		b->texCoords[texUnit] = (float*) mapVBO(b->texCoordsVBO[texUnit], GL_WRITE_ONLY);
		printf("map texcoords vbo\n");
	}

	if (b->verticesCount >= b->numVerts);
		return;

	b->texCoords[texUnit][b->verticesCount*3] = s;
	b->texCoords[texUnit][b->verticesCount*3+1] = t;

}
void end(batch* b){
	if(b->vertices != NULL) {
		unmapVBO(b->vertexVBO);
		b->vertices = NULL;
	}
                
	if(b->colors != NULL) {
		unmapVBO( b->colorVBO);
		b->colors = NULL; 
	}
                
        if(b->normals != NULL) {
		unmapVBO( b->normalVBO);
		b->normals = NULL;
	}
                
	for(unsigned int i = 0; i < b->numTexSets; i++)
		if(b->texCoords[i] != NULL) {
			printf("unmap texcoordsvbo \n");
			unmapVBO(b->texCoordsVBO[i]);
			b->texCoords[i] = NULL;
		}


	vertexAttribute** attr = initializeVertexFormat();

        
        if(b->vertexVBO !=0) {
		attr[ATTR_VERTEX] = malloc(sizeof(vertexAttribute));
		attr[ATTR_VERTEX]->count = b->numVerts;
		attr[ATTR_VERTEX]->size = b->numVerts*sizeof(float);
		attr[ATTR_VERTEX]->type = ATTR_VERTEX;
		attr[ATTR_VERTEX]->offset =  0;
		attr[ATTR_VERTEX]->components = 3;
		attr[ATTR_VERTEX]->vboID = b->vertexVBO;
	}
                
	if(b->colorVBO != 0) {
		attr[ATTR_COLOR] = malloc(sizeof(vertexAttribute));
		attr[ATTR_COLOR]->count = b->numVerts;
		attr[ATTR_COLOR]->size = b->numVerts*sizeof(float);
		attr[ATTR_COLOR]->type = ATTR_COLOR;
		attr[ATTR_COLOR]->offset =  0;
		attr[ATTR_COLOR]->components = 4;
		attr[ATTR_COLOR]->vboID = b->colorVBO;
	}
                
	if(b->normalVBO != 0) {
		attr[ATTR_NORMAL] = malloc(sizeof(vertexAttribute));
		attr[ATTR_NORMAL]->count = b->numVerts;
		attr[ATTR_NORMAL]->size = b->numVerts*sizeof(float);
		attr[ATTR_NORMAL]->type = ATTR_NORMAL;
		attr[ATTR_NORMAL]->offset =  0;
		attr[ATTR_NORMAL]->components = 3;
		attr[ATTR_NORMAL]->vboID = b->normalVBO;
	}
                
	for(unsigned int i = 0; i < b->numTexSets; i++)
		if(b->texCoordsVBO[i] != 0) {
			printf("configurando texcoords vbo %d vboid: %d \n", i, b->texCoordsVBO[i]);
			attr[ATTR_TEXCOORD0+i] = malloc(sizeof(vertexAttribute));
			attr[ATTR_TEXCOORD0+i]->count = b->numVerts;
			attr[ATTR_TEXCOORD0+i]->size = b->numVerts*sizeof(float);
			attr[ATTR_TEXCOORD0+i]->type = ATTR_TEXCOORD0+i;
			attr[ATTR_TEXCOORD0+i]->offset = 0;
			attr[ATTR_TEXCOORD0+i]->components = 2;
			attr[ATTR_TEXCOORD0+i]->vboID = b->texCoordsVBO[i];
		}
        

	printf("configura vao \n");
	configureVAO(b->vaoid, attr);
	printf("configure vao done \n");
}

void draw(batch* b){
	drawArraysVAO(b->vaoid, b->primitive, b->numVerts);
}


batch* makeCube(float radius){
	
	batch* b = malloc(sizeof(batch));
	initializeBatch(b);
	
	begin(b, GL_TRIANGLES, 36, 1);
	
	//top
	vertex3f(b, radius, radius, radius);
	normal3f(b, 0.0, 1.0, 0.0);
	texCoord2f(b, 0, radius, radius);

	vertex3f(b, radius, radius, -radius);
	normal3f(b, 0.0, 1.0, 0.0);
	texCoord2f(b, 0, radius, 0.0 );

	vertex3f(b, -radius, radius, -radius);
	normal3f(b, 0.0, 1.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);

	vertex3f(b, radius, radius, radius);
	normal3f(b, 0.0, 1.0, 0.0);
	texCoord2f(b, 0, radius, radius);

	vertex3f(b, -radius, radius, -radius);
	normal3f(b, 0.0, 1.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);

	vertex3f(b, -radius, radius, radius);
	normal3f(b,  0.0, 1.0, 0.0);
	texCoord2f(b, 0, 0.0, radius);

	//bottom
	vertex3f(b, -radius, -radius, -radius);
	normal3f(b, 0.0, -1.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);

	vertex3f(b, radius, -radius, -radius);
	normal3f(b, 0.0, -1.0, 0.0);
	texCoord2f(b, 0, radius, 0.0);

	vertex3f(b, radius, -radius, radius);
	normal3f(b, 0.0, -1.0, 0.0);
	texCoord2f(b, 0, radius, radius);

	vertex3f(b, -radius, -radius, radius);
	normal3f(b, 0.0, -1.0, 0.0);
	texCoord2f(b,  0, 0.0, radius);

	vertex3f(b, -radius, -radius, -radius);
	normal3f(b,  0.0, -1.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);

	vertex3f(b, radius, -radius, radius);
	normal3f(b, 0.0, -1.0, 0.0);
	texCoord2f(b, 0, radius, radius);

	//left
	vertex3f(b, -radius, radius, radius);
	normal3f(b, -1.0, 0.0, 0.0);
	texCoord2f(b, 0, radius, radius);

	vertex3f(b, -radius, radius, -radius);
	normal3f(b, -1.0, 0.0, 0.0);
	texCoord2f(b, 0, radius, 0.0);

	vertex3f(b, -radius, -radius, -radius);
	normal3f(b, -1.0, 0.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);

	vertex3f(b, -radius, radius, radius);
	normal3f(b, -1.0, 0.0, 0.0);
	texCoord2f(b, 0, radius, radius);

	vertex3f(b, -radius, -radius, -radius);
	normal3f(b, -1.0, 0.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);

	vertex3f(b, -radius, -radius, radius);
	normal3f(b, -1.0, 0.0, 0.0);
	texCoord2f(b, 0, 0.0, radius);
	
	//right 
	vertex3f(b, radius, -radius, -radius);
	normal3f(b, 1.0, 0.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);

	vertex3f(b, radius, radius, -radius);
	normal3f(b, 1.0, 0.0, 0.0);
	texCoord2f(b, 0, radius, 0.0);

	vertex3f(b, radius, radius, radius);
	normal3f(b, 1.0, 0.0, 0.0);
	texCoord2f(b, 0, radius, radius);

	vertex3f(b, radius, radius, radius);
	normal3f(b, 1.0, 0.0, 0.0);
	texCoord2f(b, 0, radius, radius);

	vertex3f(b, radius, -radius, radius);
	normal3f(b, 1.0, 0.0, 0.0);
	texCoord2f(b, 0, 0.0, radius);

	vertex3f(b, radius, -radius, -radius);
	normal3f(b, 1.0, 0.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);

	//front
	vertex3f(b, radius, -radius, radius);
	normal3f(b, 0.0, 0.0, 1.0);
	texCoord2f(b, 0, radius, 0 );

	vertex3f(b, radius, radius, radius);
	normal3f(b, 0.0, 0.0, 1.0);
	texCoord2f(b, 0, radius, radius);

	vertex3f(b, -radius, radius, radius);
	normal3f(b, 0.0, 0.0, 1.0);
	texCoord2f(b, 0, 0, radius);

	vertex3f(b, -radius, radius, radius);
	normal3f(b, 0.0, 0.0, 1.0);
	texCoord2f(b, 0, 0, radius );

	vertex3f(b, -radius, -radius, radius);
	normal3f(b, 0.0, 0.0, 1.0);
	texCoord2f(b, 0, 0, 0);

	vertex3f(b, radius, -radius, radius);
	normal3f(b, 0.0, 0.0, 1.0);
	texCoord2f(b, 0, radius, 0.0 );

	
	//back
	vertex3f(b, radius, -radius, -radius);
	normal3f(b, 0.0, 0.0, -1.0);
	texCoord2f(b, 0, radius, 0.0 );

	vertex3f(b, -radius, -radius, -radius);
	normal3f(b, 0.0, 0.0, -1.0);
	texCoord2f(b, 0, 0.0, 0.0);

	vertex3f(b, -radius, radius, -radius);
	normal3f(b, 0.0, 0.0, -1.0);
	texCoord2f(b, 0, 0.0, radius );

	vertex3f(b, -radius, radius, -radius);
	normal3f(b, 0.0, 0.0, -1.0);
	texCoord2f(b, 0, 0.0, radius );

	vertex3f(b, radius, radius, -radius);
	normal3f(b, 0.0, 0.0, -1.0);
	texCoord2f(b, 0, radius, radius );

	vertex3f(b, radius, -radius, -radius);
	normal3f(b, 0.0, 0.0, -1.0);
	texCoord2f(b, 0, radius, 0.0);

	end(b);
	
	return b;
}
