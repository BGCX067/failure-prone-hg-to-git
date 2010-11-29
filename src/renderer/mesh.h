#ifndef _MESH_H_
#define _MESH_H_

#include "../util/fplist.h"
#include "material.h"

#define MAX_TEXCOORDS 8

typedef struct _texcoord{

	unsigned int count;
	unsigned int set;
	unsigned short int components;
	float* texCoords;

}texCoord;

typedef struct _triangles{

	unsigned int indicesCount, verticesCount, normalsCount, tangentsCount, binormalsCount;
	unsigned int verticesComponents;
	unsigned int vboId, indicesId, vaoId;
	unsigned int totalAttrs;

	unsigned int* indices; //pode ser short
	float* vertices;
	float* normals;
	float* tangents;
	float* binormals;

	texCoord* texCoords[MAX_TEXCOORDS];
	unsigned int numTexSets;

	material mat;

}triangles;

typedef struct _mesh{

	int trianglesCount;
	//triangles* tris;
	fplist*  tris;

}mesh;

void createVBO(mesh* mesh);


#endif
