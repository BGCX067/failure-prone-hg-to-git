#include "mesh.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //memset
#include "util/utlist.h"

Mesh* InitMesh() {
    Mesh *m = malloc(sizeof(Mesh));
    memset(m, 0, sizeof(Mesh));
    return m;
}

void AddVertices(Mesh *m, int num, int comp, float *vertices) {
    m->verticesCount = num;
    m->vertices = vertices;
    SetVertexAttribute(m->attrs, ATTR_VERTEX, m->verticesCount, m->verticesCount*sizeof(float), 0, comp, 0);
    CalcBBox(&m->b, m->vertices, m->verticesCount/3);
    //setMeshBBox(m);
}

void AddNormals(Mesh* m, int num, int comp, float *normals) {
    m->normalsCount = num;
    m->normals = normals;
    SetVertexAttribute(m->attrs, ATTR_NORMAL, m->normalsCount, m->normalsCount*sizeof(float), 0, comp, 0);
}

void AddTexCoords(Mesh *m, int num, int comp, int texset, float *texcoords) {
    m->texCoords[texset] = malloc(sizeof(TexCoord));
    m->texCoords[texset]->count = num;
    m->texCoords[texset]->components = comp;
    m->texCoords[texset]->set = texset;
    m->texCoords[texset]->texCoords = texcoords;
    m->numTexSets++;
    SetVertexAttribute(m->attrs, ATTR_TEXCOORD0+texset, m->texCoords[texset]->count, m->texCoords[texset]->count*sizeof(float), 0, comp, 0);
}

void AddIndices(Mesh *m, int num, unsigned int *indices) {
    m->indicesCount = num;
    m->indices = indices;
}

void AddColors(Mesh *m, int count, float *colors) {
   m->colors = colors;
   m->colorsCount = count;
   SetVertexAttribute(m->attrs, ATTR_COLOR, m->colorsCount, m->colorsCount*sizeof(float), 0, 3, 0);
}


void PrepareMesh(Mesh *m) {
    //Criar VBO de indices
    m->indicesId = InitializeVBO(m->indicesCount*sizeof(unsigned int), GL_STATIC_DRAW, m->indices);
    m->verticesVBO = InitializeVBO(m->verticesCount*sizeof(float), GL_STATIC_DRAW, m->vertices);
    m->attrs[ATTR_VERTEX]->vboID = m->verticesVBO;

    if(m->normals) {
        m->normalsVBO = InitializeVBO(m->normalsCount*sizeof(float), GL_STATIC_DRAW, m->normals);
        m->attrs[ATTR_NORMAL]->vboID = m->normalsVBO;
    }
    if(m->binormals) {
        m->binormalsVBO = InitializeVBO(m->binormalsCount*sizeof(float), GL_STATIC_DRAW, m->binormals);
        m->attrs[ATTR_BINORMAL]->vboID = m->binormalsVBO;
    }
    if(m->tangents) {
        m->tangentsVBO = InitializeVBO(m->tangentsCount*sizeof(float), GL_STATIC_DRAW, m->tangents);
        m->attrs[ATTR_TANGENT]->vboID = m->tangentsVBO;
    }
    for(unsigned int i = 0; i < m->numTexSets; i++) {
        m->texVBO[i] = InitializeVBO(m->texCoords[i]->count*sizeof(float), GL_STATIC_DRAW, m->texCoords[i]->texCoords);
        m->attrs[ATTR_TEXCOORD0 + i]->vboID = m->texVBO[i];
    }
    if(m->colors) {
        m->colorsVBO = InitializeVBO(m->colorsCount*sizeof(float), GL_STATIC_DRAW, m->colors);
        m->attrs[ATTR_COLOR]->vboID = m->colorsVBO;
    }
    m->vaoId = InitEmptyVAO();
    ConfigureIndexedVAO(m->vaoId, m->indicesId, m->attrs);
}

//TODO: versão pra AnimatedMesh 
/*void prepareMesh(Mesh *m) {
    for(fpnode *n = m->tris->first; n != NULL; n = n->next)
        prepareTris(n->data);
    setmeshboundingbox(m);
}*/


void AddAnim(AnimatedMesh *m, SkeletalAnim *anim) {
    if(m->animated == 0) {
        m->animList = NULL;
        m->animated = 1;
    }
    DL_APPEND(m->animList, anim);
}


SkeletalAnim* GetCurrentAnim(AnimatedMesh *m) {
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
void SetNormals(unsigned int *tIndices, float *tVerts, float *tNormals, 
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
        Subv(iapos, ibpos, e1);
        vec3 e2;
        Subv(icpos, ibpos, e2);
        vec3 no;
        Cross(e2, e1, no);

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
        Normalizev(n);
        tNormals[3*j] = n[0];
        tNormals[3*j + 1] = n[1];
        tNormals[3*j + 2] = n[2];
    }
}

//TODO verificar se precisa liberar memória do vetor retornado pelo map
void UpdateMeshNormals(Mesh* m,  float* normals){
	float* n = MapVBO(m->normalsVBO, GL_WRITE_ONLY);
	memcpy(n, normals, m->normalsCount*sizeof(float));
	UnmapVBO(m->normalsVBO);
}

void UpdateMeshVertices(Mesh* m,  float* normals){
	float* v = MapVBO(m->verticesVBO, GL_WRITE_ONLY);
	memcpy(v, normals, m->verticesCount*sizeof(float));
	UnmapVBO(m->verticesVBO);
}

void UpdateMeshColors(Mesh* m, float* colors) {
	float* c = MapVBO(m->colorsVBO, GL_WRITE_ONLY);
	memcpy(c, colors, m->colorsCount*sizeof(float));
	UnmapVBO(m->colorsVBO);
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

void InterpolateSkeletons(const Joint *skelA, const Joint *skelB,
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
        Slerpq(skelA[i].orientation, skelB[i].orientation, interp, out[i].orientation);
    }
}

int RayMeshIntersection(vec3 ro, vec3 rd, Mesh *m, int *indices) {
    //TODO testar logo com bounding box pra otimizar casos em que não há interseção
    int intersect = 0;
    float minT = 99999999.0f;
    //percorrer a lista de triangulos mesmo
    for(unsigned int j = 0; j < m->indicesCount; j+=3) {
        unsigned int ia = m->indices[j], ib = m->indices[j + 1], ic = m->indices[j + 2];
        vec3 va, vb, vc;
        va[0] = m->vertices[3*ia]; va[1] = m->vertices[3*ia + 1]; va[2] = m->vertices[3*ia + 2];
        vb[0] = m->vertices[3*ib]; vb[1] = m->vertices[3*ib + 1]; vb[2] = m->vertices[3*ib + 2];
        vc[0] = m->vertices[3*ic]; vc[1] = m->vertices[3*ic + 1]; vc[2] = m->vertices[3*ic + 2];

        float t;
        if(RayTriangleIntersection(ro, rd, va, vb, vc, &t)) {
            if(t < minT) {
                minT = t;
                intersect = 1;
                indices[0] = ia; indices[1] = ib; indices[2] = ic;
            }
        }
    }

    return intersect;
}

int Picking(int mouseX, int mouseY, mat4 modelviewMatrix, mat4 projectionMatrix, int viewPort[4], Mesh *m, int *indices) {
    float pNear[3], pFar[3];
    Unproject(mouseX, mouseY, 0.0, modelviewMatrix, projectionMatrix, viewPort, &pNear[0], &pNear[1], &pNear[2]);
    Unproject(mouseX, mouseY, 1.0, modelviewMatrix, projectionMatrix, viewPort, &pFar[0], &pFar[1], &pFar[2]);
   
    //Pega a posição da camera a partir da modelview
    vec3 rayOrigin;
    PosFromMatrix(modelviewMatrix, rayOrigin);

    vec3 rayDir;
    rayDir[0] = pFar[0] - pNear[0];
    rayDir[1] = pFar[1] - pNear[1];
    rayDir[2] = pFar[2] - pNear[2];
    Normalizev(rayDir);

    return RayMeshIntersection(rayOrigin, rayDir, m, indices);
}

Mesh* CreateBox(float w, float h, float l) {
    Mesh *m = InitMesh();

    float x = 0.5*w;
    float y = 0.5*h;
    float z = 0.5*l;

    unsigned int indices[] = { 0, 3, 6, 0, 6, 9, //Front
                               15, 12, 21, 15, 21, 18, //Back
                               5, 17, 20, 5, 20, 8, //Right
                               14, 2, 11, 14, 11, 23, //Left
                               10, 7, 19, 10, 19, 22, // TOP
                               13, 16, 4, 13, 4, 1 //BOT
                             };

    float vertices[] = { -x, -y, z, -x, -y, z, -x, -y, z,
                          x, -y, z,  x, -y, z,  x, -y, z, 
                          x, y, z, x, y, z, x, y, z,
                          -x, y, z, -x, y, z, -x, y, z,
                          -x, -y, -z, -x, -y, -z, -x, -y, -z,
                          x, -y, -z, x, -y, -z, x, -y, -z,
                          x, y, -z, x, y, -z, x, y, -z,
                          -x, y, -z, -x, y, -z, -x, y, -z };

    float normals[] = { 0.0, 0.0, 1.0, 0.0, -1.0, 0.0, -1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0, 1.0, 0.0, -1.0, 0.0, 0.0,
                        0.0, 0.0, -1.0, 0.0, -1.0, 0.0, -1.0, 0.0, 0.0,
                        0.0, 0.0, -1.0, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, -1.0, 0.0, 1.0, 0.0, -1.0, 0.0, 0.0,
                      };

    float colors[24*3];
    for(int i = 0; i < 24; i++) {
        colors[3*i] = 1.0;
        colors[3*i + 1] = 0.82;
        colors[3*i + 2] = 0.55;
    }
    AddVertices(m, 24*3, 3, vertices);
    AddNormals(m, 24*3, 3, normals);
    AddColors(m, 24*3, colors);
    AddIndices(m, 36, indices);
    PrepareMesh(m);

    return m;
}
