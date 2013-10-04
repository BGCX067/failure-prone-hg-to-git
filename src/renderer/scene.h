#ifndef _SCENE_H_
#define _SCENE_H_

#include "mesh.h"
//#include "material.h"
#include "renderer.h"
#include "math/mathutil.h"
#include "light.h"
#include "math/matrix.h"


//TODO
//1. automatizar uso do material? (passar pras funções de AddMesh) 
typedef struct _node{
	Mesh *mesh;
    mat4 transform;
    mat4 ltransform;
    Shader *material;
	struct _node* parent;
	struct _node* children;
	struct _node *prev, *next;
} Node;

typedef struct MeshInfo MeshInfo;

typedef struct _scene{
    Node *root;
    MeshInfo *mInfos;
    BBox b;
}Scene;

Scene* InitializeScene();
Node* AddMesh(Scene *s, Mesh *m);
Node* AddMeshFile(Scene *s, char *filename);
void AddChildNode(Node *parent, Node *child);
//void RmMesh(Scene *s, Mesh *m);
void CalcBBoxs(Scene *s);
void DrawScene(Scene* s);
//void initializeTriangles(Triangles* tri);
//int scaddTris(Scene *s, Triangles *t);
//void AddMaterial(Scene *s, Shader *m);
//int addTexture(Scene *s, Texture *t);
//int addNode(Scene *s, Node *m);
//Scene* initializeDae(char* filename);

#endif
