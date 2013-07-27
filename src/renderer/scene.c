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

void AddMesh(Scene* s, Mesh *m){
    DL_APPEND(s->meshList, m);
    //FIXME com struct auxiliar
    //MeshElem *melem = malloc(sizeof(MeshElem));
    //melem->m = m;
    //DL_APPEND(s->meshList, melem);
    BBUnion(&s->b, m->b, s->b);
}

void RmMesh(Scene *s, Mesh *m) {
    DL_DELETE(s->meshList, m);
    //FIXME Com struct auxiliar
    //MeshElem melem, *tmpelem;
    //melem.m = m;
    //DL_SEARCH(s->meshList, tmpelem, &melem, meshElemCmp);
    //DL_DELETE(s->meshList, tmpelem);
    //Atualizar bounding box
    CalcBBoxs(s);
}

void AddLight(Scene* s, Light* l){
    DL_APPEND(s->lightList, l);
}

void AddMaterial(Scene *s, Shader *m) {
    DL_APPEND(s->materialList, m);
}

//void addTexture(Scene *s, Texture *t) {
//  return fplist_insback(t, s->texList);
//}

//int addNode(Scene* s, Node* n){
//	return fplist_insback(n, s->nodes);
//}

Scene* InitializeScene(){
	Scene * s = malloc(sizeof(Scene));
	memset(s, 0, sizeof(Scene));
    
    //TODO animatedmeshlist
    s->meshList = NULL;
    s->lightList = NULL;
    s->materialList = NULL; 

    s->b.pmin[0] = 99999999.0f;
    s->b.pmin[1] = 99999999.0f;
    s->b.pmin[2] = 99999999.0f;

    s->b.pmax[0] = -999999999.0f;
    s->b.pmax[1] = -999999999.0f;
    s->b.pmax[2] = -999999999.0f;

	return s;
}

void DrawScene(Scene* scn){
	if (scn == NULL)
		return;

    //Iterator pra percorrer a lista
    Mesh *it;
    DL_FOREACH(scn->meshList, it)
        DrawIndexedVAO(it->vaoId, it->indicesCount, GL_TRIANGLES);


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
    Mesh *it;
    DL_FOREACH(s->meshList, it) { 
        BBUnion(&s->b, it->b, s->b);
        size++;
    }

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
