#include "scene.h"
#include <stdio.h>
#include <string.h>
#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
#include "util/utlist.h"
#include <stdlib.h>

struct MeshInfo {
    Mesh *m;
    char *id;
    int refs;
    struct MeshInfo *prev, *next;
};

static MeshInfo *meshIsLoadedId(Scene *s, char *filename) {
    MeshInfo *mi;
    DL_FOREACH(s->mInfos, mi) {
        if(!strcmp(mi->id, filename)) {
            return mi;
        }
    }
    return NULL;
}

static MeshInfo *meshIsLoadedPtr(Scene *s, Mesh *m) {
    MeshInfo *mi;
    DL_FOREACH(s->mInfos, mi) {
        if(m == mi->m) {
            return mi;
        }
    }
    return NULL;
}

static MeshInfo* meshAddInfo(Scene *s, Mesh *m, char *filename) {
    MeshInfo *mi = malloc(sizeof(MeshInfo));
    mi->m = m;
    mi->id = malloc(sizeof(char)*(strlen(filename) + 1));
    strcpy(mi->id, filename);
    //mi->id = strdup(filename);
    mi->refs = 1;
    DL_APPEND(s->mInfos, mi);    
    return mi;
}

//Usado para meshes lidos de arquivos
Node* AddMeshFile(Scene *s, char *filename) {
    Node *n = malloc(sizeof(Node));
    Identity(n->transform);
    Identity(n->ltransform);
    n->parent = s->root;
    n->children = NULL;
    n->material = NULL;
    DL_APPEND(s->root->children, n);
    
    MeshInfo *mi = meshIsLoadedId(s, filename);
    if(!mi) {
        //Mesh com esse identificador ainda não existe, carregar o mesh
        //Mesh *m = loadMeshFromFile(filename);
        Mesh *m =  NULL;
        mi = meshAddInfo(s, m, filename);
    } else {
        mi->refs++;
    }
    n->mesh = mi->m;
    return n;
}

//Usado para meshes gerados proceduralmente
//a unica utilidade de verificar se um mesh igual já existe
//é gerenciar meio que automaticamente a memória e deletar
//meshes para os quais já não há mais referencias
Node* AddMesh(Scene *s, Mesh *m) {
    Node *n = malloc(sizeof(Node));
    Identity(n->transform);
    Identity(n->ltransform);
    n->parent = s->root;
    n->children = NULL;
    n->material = NULL;
    DL_APPEND(s->root->children, n);

    MeshInfo *mi = meshIsLoadedPtr(s, m);
    if(!mi) {
        mi = meshAddInfo(s, m, "fpmesh");
    } else {
        mi->refs++;
    }
    n->mesh = mi->m;
    return n;
}

void AddChildNode(Node *parent, Node *child) {
    if(child->parent) {
        DL_DELETE(child->parent->children, child);
    }
    child->parent = parent;
    DL_APPEND(parent->children, child);
}

//void AddMesh(Scene* s, Mesh *m){
//    DL_APPEND(s->meshList, m);
//    BBUnion(&s->b, m->b, s->b);
//}

//void RmMesh(Scene *s, Mesh *m) {
    //DL_DELETE(s->meshList, m);
    //FIXME Com struct auxiliar
    //MeshElem melem, *tmpelem;
    //melem.m = m;
    //DL_SEARCH(s->meshList, tmpelem, &melem, meshElemCmp);
    //DL_DELETE(s->meshList, tmpelem);
    //Atualizar bounding box
    //CalcBBoxs(s);
//}

Scene* InitializeScene(){
	Scene * s = malloc(sizeof(Scene));
	memset(s, 0, sizeof(Scene));

    s->root = malloc(sizeof(Node));
    s->root->material = NULL;
    s->root->mesh = NULL;
    Identity(s->root->transform);
    Identity(s->root->ltransform);
    s->root->children = NULL;
    s->root->parent = NULL;
    
    //TODO animatedmeshlist
    s->mInfos = NULL;

    s->b.pmin[0] = 99999999.0f;
    s->b.pmin[1] = 99999999.0f;
    s->b.pmin[2] = 99999999.0f;

    s->b.pmax[0] = -999999999.0f;
    s->b.pmax[1] = -999999999.0f;
    s->b.pmax[2] = -999999999.0f;

	return s;
}

static void renderNode(Node *n, mat4 t) {
    mat4 tres, lres;
    Multm(tres, t, n->transform);
    Multm(lres, n->transform, n->ltransform);
    Multm(lres, t, lres);
    SetModel(lres);
    BindShader(n->material);
    if(n->mesh)
        DrawIndexedVAO(n->mesh->vaoId, n->mesh->indicesCount, GL_TRIANGLES);
    Node *it;
    DL_FOREACH(n->children, it) {
        renderNode(it, tres);
    }
}

void DrawScene(Scene* scn){
	if (scn == NULL)
		return;
    mat4 ident;
    Identity(ident);
    SetModel(ident);
    //renderNode(scn->root, ident); 
    Node *it;
    DL_FOREACH(scn->root->children, it) {
        renderNode(it, ident);
    }
    //Outras coisas além de scn podem ser desenhadas,
    //volta a matriz model para a identidade.
    SetModel(ident);
    //Iterator pra percorrer a lista
    //Mesh *it;
    //DL_FOREACH(scn->meshList, it)
    //    DrawIndexedVAO(it->vaoId, it->indicesCount, GL_TRIANGLES);


    //FIXME com struct auxiliar:
    //MeshElem *it;
    //DL_FOREACH(scn->meshList, it) {
    //    drawIndexedVAO(it->m->vaoId, it->m->indicesCount, GL_TRIANGLES);
    //}
    
    //TODO draw animatedmesh
/*	if (scn->meshList){
		Mesh* m = NULL;
		for( int i = 0; i < scn->meshList->size; i++){ // para da mesh da cena
			m = fplist_getdata(i, scn->meshList);
			if (m->tris){
				Triangles* tri = NULL;
				for( int k = 0; k < m->tris->size; k++){ //para cada chunk de triangles do mesh
					tri = fplist_getdata(k, m->tris);
					drawIndexedVAO(tri->vaoId, tri->indicesCount, GL_TRIANGLES);
				}
			}
		}
	}*/
}

//TODO considerar animated mesh
void CalcBBoxs(Scene *s) {
    memset(&s->b, 0, sizeof(BBox));
    //Ver se a lista tá vazia
    int size = 0;
    //Mesh *it;
    //DL_FOREACH(s->meshList, it) { 
    //    BBUnion(&s->b, it->b, s->b);
    //    size++;
    //}

    //FIXME com struct auxiliar
    //MeshElem *it;
    //DL_FOREACH(s->meshList, it) {
    //    bbunion(&s->b, it->m->b, s->b);
    //    size++;
    //}
    
    if(size == 0) {
        s->b.pmin[0] = 99999999.0f;
        s->b.pmin[1] = 99999999.0f;
        s->b.pmin[2] = 99999999.0f;
        s->b.pmax[0] = -99999999.0f;
        s->b.pmax[1] = -99999999.0f;
        s->b.pmax[2] = -99999999.0f;
    }
}
