#include "glime.h"
#include <stdio.h>
#include <GL/gl.h>
#include "renderer.h"
//#include "glextensions.h"

batch* initializeBatch(){

	batch* b = malloc(sizeof(batch));

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

void begin(batch *b, int primitive, int nverts, int texSets){
	
	b->primitive = primitive;
	b->numVerts = nverts;
	b->numTexSets = texSets;

	if (texSets){
		b->texCoordsVBO = malloc(sizeof(unsigned int)*texSets);
		b->texCoords = malloc(sizeof(float*)*texSets);
		memset(b->texCoordsVBO, 0, sizeof(unsigned int)*texSets );
		memset(b->texCoords, NULL, sizeof(float*)*texSets);
	}

	b->vaoid = initEmptyVAO();	
}

void vertex3f(batch* b, float x, float y, float z){

	if (!b->verticesComponents)
		b->verticesComponents = 3;

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

void vertex2f(batch* b, float x, float y){

	if (!b->verticesComponents)
		b->verticesComponents = 2;

	if (b->vertexVBO == 0)
		b->vertexVBO = initializeVBO(sizeof(float)*2*b->numVerts, GL_DYNAMIC_DRAW, NULL);
	
	if (b->vertices == NULL)
		b->vertices = (float*) mapVBO(b->vertexVBO, GL_WRITE_ONLY);
		
	if (b->verticesCount >= b->numVerts)
		return;
	
	b->vertices[b->verticesCount*2] = x;
	b->vertices[b->verticesCount*2+1] = y;
	b->verticesCount++;
	
}

void normal3f(batch *b, float x, float y, float z){

	if (b->normalVBO == 0){
		//printf("initialize normals\n");
		b->normalVBO = initializeVBO(sizeof(float)*3*b->numVerts, GL_DYNAMIC_DRAW, NULL);
	}

	if (b->normals == NULL){
		//printf("map normals vbo \n");
		b->normals = (float*) mapVBO(b->normalVBO, GL_WRITE_ONLY);
	}

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

	b->colors[b->verticesCount*4] = x;
	b->colors[b->verticesCount*4+1] = y;
	b->colors[b->verticesCount*4+2] = y;
	b->colors[b->verticesCount*4+3] = w;

}

void texCoord2f(batch* b, unsigned int texUnit, float s, float t){

	if (b->texCoordsVBO[texUnit] == 0){
		//printf("initialize vbo \n");
		b->texCoordsVBO[texUnit] = initializeVBO(sizeof(float)*2*b->numVerts, GL_DYNAMIC_DRAW, NULL);
		//printf("initialize texcoords vbo: %d \n", b->texCoordsVBO[texUnit] );
	}

	if (b->texCoords[texUnit] == NULL){
		b->texCoords[texUnit] = (float*) mapVBO(b->texCoordsVBO[texUnit], GL_WRITE_ONLY);
		//printf("map texcoords vbo\n");
	}

	if (b->verticesCount >= b->numVerts){
		return;
	}

	b->texCoords[texUnit][b->verticesCount*2] = s;
	b->texCoords[texUnit][b->verticesCount*2+1] = t;

//	printf("texCoord2f %f %f s %f  t %f \n", b->texCoords[texUnit][b->verticesCount*2], b->texCoords[texUnit][b->verticesCount*2+1], s , t );

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
			unmapVBO(b->texCoordsVBO[i]);
			b->texCoords[i] = NULL;
		}


	VertexAttribute** attr = initializeVertexFormat();

        
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
	configureVAO(b->vaoid, attr);
	//printf("configure vao done \n");
}

void draw(batch* b){
	drawArraysVAO(b->vaoid, b->primitive, b->numVerts);
}


batch* makeCube(float radius){
	
	batch* b = initializeBatch();
	begin(b, GL_TRIANGLES, 36, 1);
	
	//top
	//printf("normal \n");
	normal3f(b, 0.0, 1.0, 0.0);
	//printf("normal done \n");
	texCoord2f(b, 0, radius, radius);
	//printf("texcoord \n");
	vertex3f(b, radius, radius, radius);
	//printf("vertex \n");

	normal3f(b, 0.0, 1.0, 0.0);
	texCoord2f(b, 0, radius, 0.0 );
	vertex3f(b, radius, radius, -radius);

	normal3f(b, 0.0, 1.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);
	vertex3f(b, -radius, radius, -radius);

	normal3f(b, 0.0, 1.0, 0.0);
	texCoord2f(b, 0, radius, radius);
	vertex3f(b, radius, radius, radius);

	normal3f(b, 0.0, 1.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);
	vertex3f(b, -radius, radius, -radius);

	normal3f(b,  0.0, 1.0, 0.0);
	texCoord2f(b, 0, 0.0, radius);
	vertex3f(b, -radius, radius, radius);

	//bottom
	normal3f(b, 0.0, -1.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);
	vertex3f(b, -radius, -radius, -radius);

	normal3f(b, 0.0, -1.0, 0.0);
	texCoord2f(b, 0, radius, 0.0);
	vertex3f(b, radius, -radius, -radius);

	normal3f(b, 0.0, -1.0, 0.0);
	texCoord2f(b, 0, radius, radius);
	vertex3f(b, radius, -radius, radius);

	normal3f(b, 0.0, -1.0, 0.0);
	texCoord2f(b,  0, 0.0, radius);
	vertex3f(b, -radius, -radius, radius);

	normal3f(b,  0.0, -1.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);
	vertex3f(b, -radius, -radius, -radius);

	normal3f(b, 0.0, -1.0, 0.0);
	texCoord2f(b, 0, radius, radius);
	vertex3f(b, radius, -radius, radius);

	//left
	normal3f(b, -1.0, 0.0, 0.0);
	texCoord2f(b, 0, radius, radius);
	vertex3f(b, -radius, radius, radius);

	normal3f(b, -1.0, 0.0, 0.0);
	texCoord2f(b, 0, radius, 0.0);
	vertex3f(b, -radius, radius, -radius);

	normal3f(b, -1.0, 0.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);
	vertex3f(b, -radius, -radius, -radius);

	normal3f(b, -1.0, 0.0, 0.0);
	texCoord2f(b, 0, radius, radius);
	vertex3f(b, -radius, radius, radius);

	normal3f(b, -1.0, 0.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);
	vertex3f(b, -radius, -radius, -radius);

	normal3f(b, -1.0, 0.0, 0.0);
	texCoord2f(b, 0, 0.0, radius);
	vertex3f(b, -radius, -radius, radius);
	
	//right 
	normal3f(b, 1.0, 0.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);
	vertex3f(b, radius, -radius, -radius);

	normal3f(b, 1.0, 0.0, 0.0);
	texCoord2f(b, 0, radius, 0.0);
	vertex3f(b, radius, radius, -radius);

	normal3f(b, 1.0, 0.0, 0.0);
	texCoord2f(b, 0, radius, radius);
	vertex3f(b, radius, radius, radius);

	normal3f(b, 1.0, 0.0, 0.0);
	texCoord2f(b, 0, radius, radius);
	vertex3f(b, radius, radius, radius);

	normal3f(b, 1.0, 0.0, 0.0);
	texCoord2f(b, 0, 0.0, radius);
	vertex3f(b, radius, -radius, radius);

	normal3f(b, 1.0, 0.0, 0.0);
	texCoord2f(b, 0, 0.0, 0.0);
	vertex3f(b, radius, -radius, -radius);

	//front
	normal3f(b, 0.0, 0.0, 1.0);
	texCoord2f(b, 0, radius, 0 );
	vertex3f(b, radius, -radius, radius);

	normal3f(b, 0.0, 0.0, 1.0);
	texCoord2f(b, 0, radius, radius);
	vertex3f(b, radius, radius, radius);

	normal3f(b, 0.0, 0.0, 1.0);
	texCoord2f(b, 0, 0, radius);
	vertex3f(b, -radius, radius, radius);

	normal3f(b, 0.0, 0.0, 1.0);
	texCoord2f(b, 0, 0, radius );
	vertex3f(b, -radius, radius, radius);

	normal3f(b, 0.0, 0.0, 1.0);
	texCoord2f(b, 0, 0, 0);
	vertex3f(b, -radius, -radius, radius);

	normal3f(b, 0.0, 0.0, 1.0);
	texCoord2f(b, 0, radius, 0.0 );
	vertex3f(b, radius, -radius, radius);

	//back
	normal3f(b, 0.0, 0.0, -1.0);
	texCoord2f(b, 0, radius, 0.0 );
	vertex3f(b, radius, -radius, -radius);

	normal3f(b, 0.0, 0.0, -1.0);
	texCoord2f(b, 0, 0.0, 0.0);
	vertex3f(b, -radius, -radius, -radius);

	normal3f(b, 0.0, 0.0, -1.0);
	texCoord2f(b, 0, 0.0, radius );
	vertex3f(b, -radius, radius, -radius);

	normal3f(b, 0.0, 0.0, -1.0);
	texCoord2f(b, 0, 0.0, radius );
	vertex3f(b, -radius, radius, -radius);

	normal3f(b, 0.0, 0.0, -1.0);
	texCoord2f(b, 0, radius, radius );
	vertex3f(b, radius, radius, -radius);

	normal3f(b, 0.0, 0.0, -1.0);
	texCoord2f(b, 0, radius, 0.0);
	vertex3f(b, radius, -radius, -radius);

	end(b);
	
	return b;
}
