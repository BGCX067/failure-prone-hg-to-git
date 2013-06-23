#include "mesh.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //memset
#include "util/utlist.h"

Mesh* initMesh() {
    Mesh *m = malloc(sizeof(Mesh));
    memset(m, 0, sizeof(Mesh));
    fpIdentity(m->transform);
    return m;
}

void addVertices(Mesh *m, int num, int comp, float *vertices) {
    m->verticesCount = num;
    m->vertices = vertices;
    setVertexAttribute(m->attrs, ATTR_VERTEX, m->verticesCount, m->verticesCount*sizeof(float), 0, comp, 0);
    setMeshBoundingBox(m);
}

void addNormals(Mesh* m, int num, int comp, float *normals) {
    m->normalsCount = num;
    m->normals = normals;
    setVertexAttribute(m->attrs, ATTR_NORMAL, m->normalsCount, m->normalsCount*sizeof(float), 0, comp, 0);
}

void addTexCoords(Mesh *m, int num, int comp, int texset, float *texcoords) {
    m->texCoords[texset] = malloc(sizeof(TexCoord));
    m->texCoords[texset]->count = num;
    m->texCoords[texset]->components = comp;
    m->texCoords[texset]->set = texset;
    m->texCoords[texset]->texCoords = texcoords;
    m->numTexSets++;
    setVertexAttribute(m->attrs, ATTR_TEXCOORD0+texset, m->texCoords[texset]->count, m->texCoords[texset]->count*sizeof(float), 0, comp, 0);
}

void addIndices(Mesh *m, int num, unsigned int *indices) {
    m->indicesCount = num;
    m->indices = indices;
}

void addColors(Mesh *m, int count, float *colors) {
   m->colors = colors;
   m->colorsCount = count;
   setVertexAttribute(m->attrs, ATTR_COLOR, m->colorsCount, m->colorsCount*sizeof(float), 0, 3, 0);
}


void prepareMesh(Mesh *m) {
    //Criar VBO de indices
    m->indicesId = initializeVBO(m->indicesCount*sizeof(unsigned int), GL_STATIC_DRAW, m->indices);
    m->verticesVBO = initializeVBO(m->verticesCount*sizeof(float), GL_STATIC_DRAW, m->vertices);
    m->attrs[ATTR_VERTEX]->vboID = m->verticesVBO;

    if(m->normals) {
        m->normalsVBO = initializeVBO(m->normalsCount*sizeof(float), GL_STATIC_DRAW, m->normals);
        m->attrs[ATTR_NORMAL]->vboID = m->normalsVBO;
    }
    if(m->binormals) {
        m->binormalsVBO = initializeVBO(m->binormalsCount*sizeof(float), GL_STATIC_DRAW, m->binormals);
        m->attrs[ATTR_BINORMAL]->vboID = m->binormalsVBO;
    }
    if(m->tangents) {
        m->tangentsVBO = initializeVBO(m->tangentsCount*sizeof(float), GL_STATIC_DRAW, m->tangents);
        m->attrs[ATTR_TANGENT]->vboID = m->tangentsVBO;
    }
    for(unsigned int i = 0; i < m->numTexSets; i++) {
        m->texVBO[i] = initializeVBO(m->texCoords[i]->count*sizeof(float), GL_STATIC_DRAW, m->texCoords[i]->texCoords);
        m->attrs[ATTR_TEXCOORD0 + i]->vboID = m->texVBO[i];
    }
    if(m->colors) {
        m->colorsVBO = initializeVBO(m->colorsCount*sizeof(float), GL_STATIC_DRAW, m->colors);
        m->attrs[ATTR_COLOR]->vboID = m->colorsVBO;
    }
    m->vaoId = initEmptyVAO();
    configureIndexedVAO(m->vaoId, m->indicesId, m->attrs);
}

//TODO: versão pra AnimatedMesh 
/*void prepareMesh(Mesh *m) {
    for(fpnode *n = m->tris->first; n != NULL; n = n->next)
        prepareTris(n->data);
    setmeshboundingbox(m);
}*/


void addAnim(AnimatedMesh *m, SkeletalAnim *anim) {
    if(m->animated == 0) {
        m->animList = NULL;
        m->animated = 1;
    }
    DL_APPEND(m->animList, anim);
}


SkeletalAnim* getCurrentAnim(AnimatedMesh *m) {
    int i = 0;
    SkeletalAnim *it;
    DL_FOREACH(m->animList, it) {
        if(i == m->currAnim)
            return it;
        i++;
    }
    return NULL;
}

//Função auxiliar pra calcular normais, dados os índices e os vértices
//baseado no código do iq
void setNormals(unsigned int *tIndices, float *tVerts, float *tNormals, 
                int trisCount, int verticesCount) 
{
    for(int j = 0; j < trisCount; j++) {
        const int ia = tIndices[3*j];
        const int ib = tIndices[3*j + 1];
        const int ic = tIndices[3*j + 2];

        vec3 iapos = { tVerts[3*ia], tVerts[3*ia + 1], tVerts[3*ia + 2]  };
        vec3 ibpos = { tVerts[3*ib], tVerts[3*ib + 1], tVerts[3*ib + 2]  };
        vec3 icpos = { tVerts[3*ic], tVerts[3*ic + 1], tVerts[3*ic + 2]  };

        vec3 e1;
        vecSub(iapos, ibpos, e1);
        vec3 e2;
        vecSub(icpos, ibpos, e2);
        vec3 no;
        cross(e2, e1, no);

        tNormals[3*ia] += no[0];
        tNormals[3*ia + 1] += no[1];
        tNormals[3*ia + 2] += no[2];
        tNormals[3*ib] += no[0];
        tNormals[3*ib + 1] += no[1];
        tNormals[3*ib + 2] += no[2];
        tNormals[3*ic] += no[0];
        tNormals[3*ic + 1] += no[1];
        tNormals[3*ic + 2] += no[2];
    }
    for(int j = 0; j < verticesCount; j++) {
        vec3 n = { tNormals[3*j], tNormals[3*j + 1], tNormals[3*j + 2] };
        vecNormalize(n);
        tNormals[3*j] = n[0];
        tNormals[3*j + 1] = n[1];
        tNormals[3*j + 2] = n[2];
    }
}

//TODO verificar se precisa liberar memória do vetor retornado pelo map
void updateMeshNormals(Mesh* m,  float* normals){
	float* n = mapVBO(m->normalsVBO, GL_WRITE_ONLY);
	memcpy(n, normals, m->normalsCount*sizeof(float));
	unmapVBO(m->normalsVBO);
}

void updateMeshVertices(Mesh* m,  float* normals){
	float* v = mapVBO(m->verticesVBO, GL_WRITE_ONLY);
	memcpy(v, normals, m->verticesCount*sizeof(float));
	unmapVBO(m->verticesVBO);
}

void updateMeshColors(Mesh* m, float* colors) {
	float* c = mapVBO(m->colorsVBO, GL_WRITE_ONLY);
	memcpy(c, colors, m->colorsCount*sizeof(float));
	unmapVBO(m->colorsVBO);
}


/*void updateMesh(Mesh* m, Joint *skeleton) {
    for(int i = 0; i < m->tris->size; i++) {
        Triangles *t = fplist_getdata(i, m->tris); 
        float *tVerts = mapVBO(t->verticesVBO, GL_WRITE_ONLY);
        for(unsigned int j = 0; j < t->verticesCount/3; j++) {
            tVerts[3*j] = 0.0;
            tVerts[3*j + 1] = 0.0;
            tVerts[3*j + 2] = 0.0;
            for(int k = 0; k < t->weightInfo[j].count; k++) {
                Weight *w = &t->weights[t->weightInfo[j].start + k];
                Joint *joint = &skeleton[w->joint];
                vec3 wv;
                rotateVec(w->pos, joint->orientation, wv);
                tVerts[3*j] += (joint->pos[0]+ wv[0])*w->factor;
                tVerts[3*j + 1] += (joint->pos[1]+ wv[1])*w->factor;
                tVerts[3*j + 2] += (joint->pos[2]+ wv[2])*w->factor;
            }
        }
        unmapVBO(t->verticesVBO);

        float *tNormals = mapVBO(t->normalsVBO, GL_WRITE_ONLY);
        setNormals(t->indices, t->vertices, tNormals, t->indicesCount/3, t->verticesCount/3);
        unmapVBO(t->normalsVBO);
    }
}*/

void interpolateSkeletons(const Joint *skelA, const Joint *skelB,
                          int numJoints, float interp, Joint *out)
{
    for(int i = 0; i < numJoints; i++) {
        /* Copy parent index */
        out[i].parent = skelA[i].parent;

        /* Linear interpolation for position */
        out[i].pos[0] = skelA[i].pos[0] + interp*(skelB[i].pos[0] - skelA[i].pos[0]);
        out[i].pos[1] = skelA[i].pos[1] + interp*(skelB[i].pos[1] - skelA[i].pos[1]);
        out[i].pos[2] = skelA[i].pos[2] + interp*(skelB[i].pos[2] - skelA[i].pos[2]);

        /* Spherical linear interpolation for orientation */
        quatSlerp(skelA[i].orientation, skelB[i].orientation, interp, out[i].orientation);
    }
}

void setMeshBoundingBox(Mesh *m) {
    m->b.pmin[0] = 99999999;
    m->b.pmin[1] = 99999999;
    m->b.pmin[2] = 99999999;

    m->b.pmax[0] = -999999999;
    m->b.pmax[1] = -999999999;
    m->b.pmax[2] = -999999999;

    for(unsigned int j = 0; j < m->verticesCount/3; j++) {
        //X
        if(m->vertices[3*j] < m->b.pmin[0])
            m->b.pmin[0] = m->vertices[3*j];
        if (m->vertices[3*j] > m->b.pmax[0])
            m->b.pmax[0] = m->vertices[3*j];
        //Y
        if(m->vertices[3*j + 1] < m->b.pmin[1])
            m->b.pmin[1] = m->vertices[3*j + 1];
        if (m->vertices[3*j + 1] > m->b.pmax[1])
            m->b.pmax[1] = m->vertices[3*j + 1];
        //Z
        if(m->vertices[3*j + 2] < m->b.pmin[2])
            m->b.pmin[2] = m->vertices[3*j + 2];
        if (m->vertices[3*j + 2] > m->b.pmax[2])
            m->b.pmax[2] = m->vertices[3*j + 2];
    }
}


int meshElemCmp(MeshElem *el1, MeshElem *el2) {
    if(el1->m == el2->m)
        return 0;
    else
        return -1;
}
