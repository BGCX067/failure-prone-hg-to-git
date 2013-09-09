#ifndef _MESH_H_
#define _MESH_H_

#include "math/mathutil.h"
#include "math/quaternion.h"
#include "math/matrix.h"
#include "vertexattribute.h"
//#include "renderer.h"

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
    quat orientation;
}Joint;

//TODO armazenar boundingbox
typedef struct SkeletalAnim {
    int numFrames, numJoints, frameRate;
    Joint **skelFrames;
    struct SkeletalAnim *prev, *next;
}SkeletalAnim;

//TODO eliminar pos do weight para que ele fique mais genérico
typedef struct _md5weight {
    int joint;
    float factor;
    vec3 pos;
}Weight;

typedef struct {
    int start;
    int count;
}VertexWeightInfo;

typedef struct Mesh{
    //OpenGL related
	unsigned int indicesId, vaoId;
    //TODO: se usar vertexattrib, não precisa desses ids, guardar na propria struct
    unsigned int verticesVBO, normalsVBO, texVBO[MAX_TEXCOORDS], binormalsVBO, tangentsVBO, colorsVBO;
    VertexAttribute *attrs[MAX_VERTEX_ATTRS];

	unsigned int indicesCount, verticesCount, normalsCount, tangentsCount, binormalsCount, colorsCount;
	unsigned int verticesComponents;
	unsigned int totalAttrs;
	unsigned int* indices; //pode ser short
	float* vertices;
	float* normals;
	float* tangents;
	float* binormals;
    float* colors;

	TexCoord* texCoords[MAX_TEXCOORDS];
	unsigned int numTexSets;

    BBox b;

    unsigned int numweights;
    Weight *weights;
    VertexWeightInfo *weightInfo;

    //informações da lista de meshes
    struct Mesh *prev, *next;
}Mesh;

//FIXME tem que fazer um initialize pro AnimatedMesh que pelo menos faça animList = NULL;
typedef struct _mesh{
    int animated, currAnim;
    SkeletalAnim *animList;
    BBox b;
}AnimatedMesh;

Mesh* InitMesh();
void AddVertices(Mesh *m, int num, int comp, float *vertices);
void AddNormals(Mesh *m, int num, int comp, float *normals);
void AddTexCoords(Mesh *m, int num, int comp, int texset, float *texcoords);
void AddIndices(Mesh *m, int count, unsigned int *indices);
void AddColors(Mesh *m, int count, float *colors);
void PrepareMesh(Mesh *m);

// API pra updatear arrays de geometria do mesh. 
// TODO talvez fosse bom poder updatear somente parte do array
// TODO fazer pra texcoords e indices
void UpdateMeshNormals(Mesh* m, float* normals);
void UpdateMeshVertices(Mesh* m, float* vertices);
void UpdateMeshColors(Mesh* t, float* colors);

void AddAnim(AnimatedMesh *m, SkeletalAnim *anim);
SkeletalAnim* GetCurrentAnim(AnimatedMesh *m);

//Calcula normais assumindo que índices e vértices são dados
void SetNormals(unsigned int *tIndices, float *tVerts, float *tNormals, 
                int indicesCount, int verticesCount);

//Calcula as novas posições do vértices e normais do mesh
//dado o skeleton
//void updateMesh(Mesh* m, Joint *skeleton);

//Interpola de 2 Skeletons
void InterpolateSkeletons(const Joint *skelA, const Joint *skelB,
                          int numJoints, float interp, Joint *out);

//Util
int RayMeshIntersection(vec3 ro, vec3 rd, Mesh *m, int *indices);
int Picking(int mouseX, int mouseY, mat4 modelviewMatrix, mat4 projectionMatrix, int viewPort[4], Mesh *m, int *indices);


//Primitives
Mesh* CreateBox(float w, float h, float l);

#endif
