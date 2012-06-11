#ifndef _MESH_H_
#define _MESH_H_

#include "../util/fplist.h"
#include "../math/boundingbox.h"
#include "../math/quaternion.h"
#include "material.h"
#include "vertexattribute.h"

#define MAX_TEXCOORDS 8

typedef struct _texcoord{
	unsigned int count;
	unsigned int set;
	unsigned short int components;
	float* texCoords;
}TexCoord;

typedef struct _joint {
    int parent;
    vec3 pos;
    quaternion orientation;
}Joint;

//TODO armazenar boundingbox
typedef struct _animationdata {
    int numFrames, numJoints, frameRate;
    Joint **skelFrames;
}SkeletalAnim;

//TODO eliminar pos do weight para que ele fique mais genérico
typedef struct _md5weight {
    int joint;
    float factor;
    vec3 pos;
}Weight;

//TODO lista de weights ao inves de start e count
typedef struct {
    int start;
    int count;
}VertexWeightInfo;

typedef struct _triangles{
    //OpenGL related
	unsigned int vboId, indicesId, vaoId;
    //TODO um vbo para cada atributo - adicionar os outros
    //TODO 2: se usar vertexattrib, não precisa desses ids, guardar na propria struct
    unsigned int verticesVBO, normalsVBO, texVBO[MAX_TEXCOORDS], binormalsVBO, tangentsVBO;
    VertexAttribute *attrs[MAX_VERTEX_ATTRS];

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

    Material *material;

    BoundingBox b;

    unsigned int numweights;
    Weight *weights;
    VertexWeightInfo *weightInfo;
}Triangles;

typedef struct _mesh{
    //TODO em fplist já é armazenado o size
	int trianglesCount;
	//Triangles* tris;
    //TODO armazena um ponteiro pra uma lista. deveria armazenar um objeto lista mesmo
	fplist*  tris;
    BoundingBox b;

    //FIXME usando o id só pra montar o scenegraph
    char *id;
    
    //TODO Vetor de animações
    int animated, currAnim;
//    SkeletalAnim *anim;
    fplist* anims;
}Mesh;

void createVBO(Mesh* mesh);
void setmeshboundingbox(Mesh *m);

//TODO: API de mesh
/*2- // gerar os arrays de geometria/indices
5- O(mesh* );*/


//FIXME remover esse initMesh por algo em scene como addMesh()
Mesh* initMesh();
Triangles* addTris(Mesh *m);
void addVertices(Triangles *t, int num, int comp, float *vertices);
void addNormals(Triangles* t, int num, int comp, float *normals);
void addTexCoords(Triangles *t, int num, int comp, int texset, float *texcoords);
void addIndices(Triangles *t, int count, unsigned int *indices);
void prepareMesh(Mesh *m);

void addAnim(Mesh *m, SkeletalAnim *anim);
SkeletalAnim* getCurrentAnim(Mesh *m);

//Calcula normais assumindo que índices e vértices são dados
void setNormals(unsigned int *tIndices, float *tVerts, float *tNormals, 
                int indicesCount, int verticesCount);

//Calcula as novas posições do vértices e normais do mesh
//dado o skeleton
void updateMesh(Mesh* m, Joint *skeleton);

//Interpola de 2 Skeletons
void interpolateSkeletons(const Joint *skelA, const Joint *skelB,
                          int numJoints, float interp, Joint *out);
#endif
