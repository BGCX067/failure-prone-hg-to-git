#ifndef _MESH_H_
#define _MESH_H_

#include "../util/fplist.h"
#include "../math/boundingbox.h"
#include "material.h"

#define MAX_TEXCOORDS 8

typedef struct _texcoord{
	unsigned int count;
	unsigned int set;
	unsigned short int components;
	float* texCoords;
}TexCoord;

typedef struct _triangles{
    //OpenGL related
	unsigned int vboId, indicesId, vaoId;
    //TODO um vbo para cada atributo - adicionar os outros
    unsigned int vertVBO, normalVBO, texVBO;

	unsigned int indicesCount, verticesCount, normalsCount, tangentsCount, binormalsCount;
	unsigned int verticesComponents;
	unsigned int totalAttrs;
	unsigned int* indices; //pode ser short
	float* vertices;
	float* normals;
	float* tangents;
	float* binormals;

	TexCoord* texCoords[MAX_TEXCOORDS];
	unsigned int numTexSets;

	Material mat;
}Triangles;

typedef struct _mesh{
    //TODO em fplist já é armazenado o size
	int trianglesCount;
	//Triangles* tris;
    //TODO armazena um ponteiro pra uma lista. deveria armazenar um objeto lista mesmo
	fplist*  tris;
    boundingbox b;
}Mesh;

void createVBO(Mesh* mesh);
void setmeshboundingbox(Mesh *m);

//TODO: API de mesh
/*2- // gerar os arrays de geometria/indices
4.1- void addVertices(tri* t, int num, int comp, float* vertices); //e outros
4.2- void addNormals(tri* t, int num, int comp, float* normals); //e outros
4.3- void addTexCoords(tri* t, int num, int comp, int texset, float* normals); //e outros
4.4- void addIndices(tri* t, int count, unsigned int* indices );
5- initVBO(mesh* );*/

Mesh* initMesh();
void addTris(Mesh *m, Triangles *tri);
void addVertices(Triangles *t, int num, int comp, float *vertices);
void addNormals(Triangles* t, int num, int comp, float *normals);
void addTexCoords(Triangles *t, int num, int comp, int texset, float *texcoords);
void addIndices(Triangles *t, int count, unsigned int *indices);

#endif
