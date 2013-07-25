#include "glime.h"
#include <stdio.h>
#include <stdlib.h> //malloc
#include <string.h> //memset
#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
#include "renderer.h"
//#include "glextensions.h"

Batch* InitializeBatch(){
	Batch* b = malloc(sizeof(Batch));

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

	b->verticesComponents = 0;
	return b;
}


void DestroyBatch(Batch *b) {
    if(b) {
        DestroyVBO(b->vertexVBO);
        DestroyVBO(b->normalVBO);
        DestroyVBO(b->colorVBO);
        DestroyVAO(b->vaoid);

        free(b);
        b = NULL;
    }
}

void Begin(Batch *b, int primitive, int nverts, int texSets){
	b->primitive = primitive;
	b->numVerts = nverts;
	b->numTexSets = texSets;

	if (texSets){
		b->texCoordsVBO = malloc(sizeof(unsigned int)*texSets);
		b->texCoords = malloc(sizeof(float*)*texSets);
		memset(b->texCoordsVBO, 0, sizeof(unsigned int)*texSets );
		memset(b->texCoords, 0, sizeof(float*)*texSets);
	}

	b->vaoid = InitEmptyVAO();	
}

void Vertex3f(Batch* b, float x, float y, float z){
	if (!b->verticesComponents)
		b->verticesComponents = 3;

	if (b->vertexVBO == 0)
		b->vertexVBO = InitializeVBO(sizeof(float)*3*b->numVerts, GL_DYNAMIC_DRAW, NULL);
	
	if (b->vertices == NULL)
		b->vertices = (float*) MapVBO(b->vertexVBO, GL_WRITE_ONLY);
		
	if (b->verticesCount >= b->numVerts)
		return;
	
	b->vertices[b->verticesCount*3] = x;
	b->vertices[b->verticesCount*3+1] = y;
	b->vertices[b->verticesCount*3+2] = z;
	b->verticesCount++;
}

void Vertex2f(Batch* b, float x, float y){
	if (!b->verticesComponents)
		b->verticesComponents = 2;

	if (b->vertexVBO == 0)
		b->vertexVBO = InitializeVBO(sizeof(float)*2*b->numVerts, GL_DYNAMIC_DRAW, NULL);
	
	if (b->vertices == NULL)
		b->vertices = (float*) MapVBO(b->vertexVBO, GL_WRITE_ONLY);
		
	if (b->verticesCount >= b->numVerts)
		return;
	
	b->vertices[b->verticesCount*2] = x;
	b->vertices[b->verticesCount*2+1] = y;
	b->verticesCount++;
}

void Normal3f(Batch *b, float x, float y, float z){
	if (b->normalVBO == 0){
		//printf("initialize normals\n");
		b->normalVBO = InitializeVBO(sizeof(float)*3*b->numVerts, GL_DYNAMIC_DRAW, NULL);
	}

	if (b->normals == NULL){
		//printf("map normals vbo \n");
		b->normals = (float*) MapVBO(b->normalVBO, GL_WRITE_ONLY);
	}

	if (b->verticesCount >= b->numVerts)
		return;

	b->normals[b->verticesCount*3] = x;
	b->normals[b->verticesCount*3+1] = y;
	b->normals[b->verticesCount*3+2] = z;
}

void Color4f(Batch* b, float x, float y, float z, float w){
	if (b->colorVBO == 0)
		b->colorVBO = InitializeVBO(sizeof(float)*4*b->numVerts, GL_DYNAMIC_DRAW, NULL);

	if (b->colors == NULL)
		b->colors = (float*) MapVBO(b->colorVBO, GL_WRITE_ONLY);

	if (b->verticesCount >= b->numVerts)
		return;

	b->colors[b->verticesCount*4] = x;
	b->colors[b->verticesCount*4+1] = y;
	b->colors[b->verticesCount*4+2] = z;
	b->colors[b->verticesCount*4+3] = w;
}

void TexCoord2f(Batch* b, unsigned int texUnit, float s, float t){
	if (b->texCoordsVBO[texUnit] == 0){
		//printf("initialize vbo \n");
		b->texCoordsVBO[texUnit] = InitializeVBO(sizeof(float)*2*b->numVerts, GL_DYNAMIC_DRAW, NULL);
		//printf("initialize texcoords vbo: %d \n", b->texCoordsVBO[texUnit] );
	}

	if (b->texCoords[texUnit] == NULL){
		b->texCoords[texUnit] = (float*) MapVBO(b->texCoordsVBO[texUnit], GL_WRITE_ONLY);
		//printf("map texcoords vbo\n");
	}

	if (b->verticesCount >= b->numVerts){
		return;
	}

	b->texCoords[texUnit][b->verticesCount*2] = s;
	b->texCoords[texUnit][b->verticesCount*2+1] = t;

//	printf("texCoord2f %f %f s %f  t %f \n", b->texCoords[texUnit][b->verticesCount*2], b->texCoords[texUnit][b->verticesCount*2+1], s , t );
}
void End(Batch* b){
	if(b->vertices != NULL) {
		UnmapVBO(b->vertexVBO);
		b->vertices = NULL;
	}
                
	if(b->colors != NULL) {
		UnmapVBO( b->colorVBO);
		b->colors = NULL; 
	}
                
    if(b->normals != NULL) {
		UnmapVBO( b->normalVBO);
	//	printf("unmap normalsvbo \n");
//		for(int k = 0; k < b->verticesCount*3; k++)
//			printf("%f \n", b->normals[k]);
		b->normals = NULL;
	}
                
	for(unsigned int i = 0; i < b->numTexSets; i++)
		if(b->texCoords[i] != NULL) {
	//		printf("unmap texcoordsvbo \n");
	//		for(int k = 0; k <b->verticesCount*2; k++ )
	//			printf("%f \n", b->texCoords[i][k]);
			UnmapVBO(b->texCoordsVBO[i]);
			b->texCoords[i] = NULL;
		}

	VertexAttribute** attr = InitializeVertexFormat();

    if(b->vertexVBO !=0) {
        attr[ATTR_VERTEX] = malloc(sizeof(VertexAttribute));
        attr[ATTR_VERTEX]->count = b->numVerts;
		attr[ATTR_VERTEX]->size = b->numVerts*sizeof(float);
		attr[ATTR_VERTEX]->type = ATTR_VERTEX;
		attr[ATTR_VERTEX]->offset =  0;
		attr[ATTR_VERTEX]->components = b->verticesComponents;
		attr[ATTR_VERTEX]->vboID = b->vertexVBO;
	}
                
	if(b->colorVBO != 0) {
		attr[ATTR_COLOR] = malloc(sizeof(VertexAttribute));
		attr[ATTR_COLOR]->count = b->numVerts;
		attr[ATTR_COLOR]->size = b->numVerts*sizeof(float);
		attr[ATTR_COLOR]->type = ATTR_COLOR;
		attr[ATTR_COLOR]->offset =  0;
		attr[ATTR_COLOR]->components = 4;
		attr[ATTR_COLOR]->vboID = b->colorVBO;
	}
                
	if(b->normalVBO != 0) {
		attr[ATTR_NORMAL] = malloc(sizeof(VertexAttribute));
		attr[ATTR_NORMAL]->count = b->numVerts;
		attr[ATTR_NORMAL]->size = b->numVerts*sizeof(float);
		attr[ATTR_NORMAL]->type = ATTR_NORMAL;
		attr[ATTR_NORMAL]->offset =  0;
		attr[ATTR_NORMAL]->components = 3;
		attr[ATTR_NORMAL]->vboID = b->normalVBO;
	}
                
	for(unsigned int i = 0; i < b->numTexSets; i++)
		if(b->texCoordsVBO[i] != 0) {
			//printf("configurando texcoords vbo %d vboid: %d \n", i, b->texCoordsVBO[i]);
			attr[ATTR_TEXCOORD0+i] = malloc(sizeof(VertexAttribute));
			attr[ATTR_TEXCOORD0+i]->count = b->numVerts;
			attr[ATTR_TEXCOORD0+i]->size = b->numVerts*sizeof(float);
			attr[ATTR_TEXCOORD0+i]->type = ATTR_TEXCOORD0+i;
			attr[ATTR_TEXCOORD0+i]->offset = 0;
			attr[ATTR_TEXCOORD0+i]->components = 2;
			attr[ATTR_TEXCOORD0+i]->vboID = b->texCoordsVBO[i];
		}
	//printf("configura vao \n");
	ConfigureVAO(b->vaoid, attr);
	//printf("configure vao done \n");
}

void Draw(Batch* b){
	DrawArraysVAO(b->vaoid, b->primitive, b->numVerts);
}


Batch* MakeCube(float radius){
	
	Batch* b = InitializeBatch();
	Begin(b, GL_TRIANGLES, 36, 1);
	
	//top
	//printf("normal \n");
	Normal3f(b, 0.0, 1.0, 0.0);
	//printf("normal done \n");
	TexCoord2f(b, 0, radius, radius);
	//printf("texcoord \n");
	Vertex3f(b, radius, radius, radius);
	//printf("vertex \n");

	Normal3f(b, 0.0, 1.0, 0.0);
	TexCoord2f(b, 0, radius, 0.0 );
	Vertex3f(b, radius, radius, -radius);

	Normal3f(b, 0.0, 1.0, 0.0);
	TexCoord2f(b, 0, 0.0, 0.0);
	Vertex3f(b, -radius, radius, -radius);

	Normal3f(b, 0.0, 1.0, 0.0);
	TexCoord2f(b, 0, radius, radius);
	Vertex3f(b, radius, radius, radius);

	Normal3f(b, 0.0, 1.0, 0.0);
	TexCoord2f(b, 0, 0.0, 0.0);
	Vertex3f(b, -radius, radius, -radius);

	Normal3f(b,  0.0, 1.0, 0.0);
	TexCoord2f(b, 0, 0.0, radius);
	Vertex3f(b, -radius, radius, radius);

	//bottom
	Normal3f(b, 0.0, -1.0, 0.0);
	TexCoord2f(b, 0, 0.0, 0.0);
	Vertex3f(b, -radius, -radius, -radius);

	Normal3f(b, 0.0, -1.0, 0.0);
	TexCoord2f(b, 0, radius, 0.0);
	Vertex3f(b, radius, -radius, -radius);

	Normal3f(b, 0.0, -1.0, 0.0);
	TexCoord2f(b, 0, radius, radius);
	Vertex3f(b, radius, -radius, radius);

	Normal3f(b, 0.0, -1.0, 0.0);
	TexCoord2f(b,  0, 0.0, radius);
	Vertex3f(b, -radius, -radius, radius);

	Normal3f(b,  0.0, -1.0, 0.0);
	TexCoord2f(b, 0, 0.0, 0.0);
	Vertex3f(b, -radius, -radius, -radius);

	Normal3f(b, 0.0, -1.0, 0.0);
	TexCoord2f(b, 0, radius, radius);
	Vertex3f(b, radius, -radius, radius);

	//left
	Normal3f(b, -1.0, 0.0, 0.0);
	TexCoord2f(b, 0, radius, radius);
	Vertex3f(b, -radius, radius, radius);

	Normal3f(b, -1.0, 0.0, 0.0);
	TexCoord2f(b, 0, radius, 0.0);
	Vertex3f(b, -radius, radius, -radius);

	Normal3f(b, -1.0, 0.0, 0.0);
	TexCoord2f(b, 0, 0.0, 0.0);
	Vertex3f(b, -radius, -radius, -radius);

	Normal3f(b, -1.0, 0.0, 0.0);
	TexCoord2f(b, 0, radius, radius);
	Vertex3f(b, -radius, radius, radius);

	Normal3f(b, -1.0, 0.0, 0.0);
	TexCoord2f(b, 0, 0.0, 0.0);
	Vertex3f(b, -radius, -radius, -radius);

	Normal3f(b, -1.0, 0.0, 0.0);
	TexCoord2f(b, 0, 0.0, radius);
	Vertex3f(b, -radius, -radius, radius);
	
	//right 
	Normal3f(b, 1.0, 0.0, 0.0);
	TexCoord2f(b, 0, 0.0, 0.0);
	Vertex3f(b, radius, -radius, -radius);

	Normal3f(b, 1.0, 0.0, 0.0);
	TexCoord2f(b, 0, radius, 0.0);
	Vertex3f(b, radius, radius, -radius);

	Normal3f(b, 1.0, 0.0, 0.0);
	TexCoord2f(b, 0, radius, radius);
	Vertex3f(b, radius, radius, radius);

	Normal3f(b, 1.0, 0.0, 0.0);
	TexCoord2f(b, 0, radius, radius);
	Vertex3f(b, radius, radius, radius);

	Normal3f(b, 1.0, 0.0, 0.0);
	TexCoord2f(b, 0, 0.0, radius);
	Vertex3f(b, radius, -radius, radius);

	Normal3f(b, 1.0, 0.0, 0.0);
	TexCoord2f(b, 0, 0.0, 0.0);
	Vertex3f(b, radius, -radius, -radius);

	//front
	Normal3f(b, 0.0, 0.0, 1.0);
	TexCoord2f(b, 0, radius, 0 );
	Vertex3f(b, radius, -radius, radius);

	Normal3f(b, 0.0, 0.0, 1.0);
	TexCoord2f(b, 0, radius, radius);
	Vertex3f(b, radius, radius, radius);

	Normal3f(b, 0.0, 0.0, 1.0);
	TexCoord2f(b, 0, 0, radius);
	Vertex3f(b, -radius, radius, radius);

	Normal3f(b, 0.0, 0.0, 1.0);
	TexCoord2f(b, 0, 0, radius );
	Vertex3f(b, -radius, radius, radius);

	Normal3f(b, 0.0, 0.0, 1.0);
	TexCoord2f(b, 0, 0, 0);
	Vertex3f(b, -radius, -radius, radius);

	Normal3f(b, 0.0, 0.0, 1.0);
	TexCoord2f(b, 0, radius, 0.0 );
	Vertex3f(b, radius, -radius, radius);

	//back
	Normal3f(b, 0.0, 0.0, -1.0);
	TexCoord2f(b, 0, radius, 0.0 );
	Vertex3f(b, radius, -radius, -radius);

	Normal3f(b, 0.0, 0.0, -1.0);
	TexCoord2f(b, 0, 0.0, 0.0);
	Vertex3f(b, -radius, -radius, -radius);

	Normal3f(b, 0.0, 0.0, -1.0);
	TexCoord2f(b, 0, 0.0, radius );
	Vertex3f(b, -radius, radius, -radius);

	Normal3f(b, 0.0, 0.0, -1.0);
	TexCoord2f(b, 0, 0.0, radius );
	Vertex3f(b, -radius, radius, -radius);

	Normal3f(b, 0.0, 0.0, -1.0);
	TexCoord2f(b, 0, radius, radius );
	Vertex3f(b, radius, radius, -radius);

	Normal3f(b, 0.0, 0.0, -1.0);
	TexCoord2f(b, 0, radius, 0.0);
	Vertex3f(b, radius, -radius, -radius);

	End(b);
	
	return b;
}
