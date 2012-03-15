#include "mesh.h"
#include "../util/fplist.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

void createVBO(Mesh* m){
	printf("criando vbo a:  %d \n" , m->trianglesCount);
	fpnode *n = m->tris->first;
	Triangles* tri = NULL;

	//verifica quais vertex attributes existem no mesh e configura a struct attrs
	while( n != NULL ){
		printf("n nao eh null\n");
		tri =  n->data;

		VertexAttribute** attrs = initializeVertexFormat();

		//TODO pode ser short int
		tri->indicesId = initializeVBO(tri->indicesCount*sizeof(unsigned int), GL_STATIC_DRAW, tri->indices);

		int vboSize = tri->verticesCount;
		attrs[ATTR_VERTEX] = malloc(sizeof(VertexAttribute));
		attrs[ATTR_VERTEX]->count = tri->verticesCount;
		attrs[ATTR_VERTEX]->size =  tri->verticesCount*sizeof(float);
		attrs[ATTR_VERTEX]->type = ATTR_VERTEX;
		attrs[ATTR_VERTEX]->offset = 0;
		attrs[ATTR_VERTEX]->components = tri->verticesComponents;
		unsigned int offset = attrs[ATTR_VERTEX]->size;

		if (tri->normals){

			vboSize += tri->normalsCount;
			attrs[ATTR_NORMAL] = malloc(sizeof(VertexAttribute));
			attrs[ATTR_NORMAL]->count = tri->normalsCount;
			attrs[ATTR_NORMAL]->size =  tri->normalsCount*sizeof(float);
			attrs[ATTR_NORMAL]->type = ATTR_NORMAL;
			attrs[ATTR_NORMAL]->offset = offset;
			attrs[ATTR_NORMAL]->components = 3;
			offset += attrs[ATTR_NORMAL]->size;
		}

		if (tri->tangents){
			vboSize += tri->tangentsCount;
			attrs[ATTR_TANGENT] = malloc(sizeof(VertexAttribute));
			attrs[ATTR_TANGENT]->count = tri->tangentsCount;
			attrs[ATTR_TANGENT]->size =  tri->tangentsCount*sizeof(float);
			attrs[ATTR_TANGENT]->type = ATTR_TANGENT;
			attrs[ATTR_TANGENT]->offset = offset;
			attrs[ATTR_TANGENT]->components = 3;
			offset += attrs[ATTR_TANGENT]->size;
		}

		if (tri->binormals){
			vboSize += tri->binormalsCount;
			attrs[ATTR_BINORMAL] = malloc(sizeof(VertexAttribute));
			attrs[ATTR_BINORMAL]->count = tri->binormalsCount;
			attrs[ATTR_BINORMAL]->size =  tri->binormalsCount*sizeof(float);
			attrs[ATTR_BINORMAL]->type = ATTR_BINORMAL;
			attrs[ATTR_BINORMAL]->offset = offset;
			attrs[ATTR_BINORMAL]->components = 3;
			offset += attrs[ATTR_BINORMAL]->size;
		}

		for( int j = 0; j < tri->numTexSets; j++){
			printf("configurando texset: %d\n", j);
			vboSize += tri->texCoords[j]->count;
			attrs[ATTR_TEXCOORD0+j] = malloc(sizeof(VertexAttribute));
			attrs[ATTR_TEXCOORD0+j]->count = tri->texCoords[j]->count;
			attrs[ATTR_TEXCOORD0+j]->size =  tri->texCoords[j]->count*sizeof(float);
			attrs[ATTR_TEXCOORD0+j]->type = ATTR_TEXCOORD0+j;
			attrs[ATTR_TEXCOORD0+j]->offset = offset;
			attrs[ATTR_TEXCOORD0+j]->components = tri->texCoords[j]->components;
			offset += attrs[ATTR_TEXCOORD0+j]->size;

		}

		printf("alocando alldata\n");
		float* alldata = malloc(sizeof(float)*vboSize);
		int indice = 0;

		printf("vertices \n");
               for (unsigned int k = 0; k < tri->verticesCount; k++, indice++ )
			alldata[indice] = tri->vertices[k];

		printf("normais \n");
		if ( tri->normals )
			for (unsigned int k = 0; k < tri->normalsCount; k++, indice++ )
				alldata[indice] = tri->normals[k];

		printf("tangentes\n");
		if ( tri->tangents ){
			printf("tem tangentes \n");
			for (unsigned int k = 0; k < tri->tangentsCount; k++, indice++ )
				alldata[indice] = tri->tangents[k];
		}

		printf("binormais \n");
		if ( tri->binormals )
			for (unsigned int k = 0; k < tri->binormalsCount; k++, indice++ )
				alldata[indice] = tri->binormals[k];
		
		printf("texcoords\n");
		for(unsigned int l  = 0; l < tri->numTexSets; l++)
			for (unsigned int k = 0; k < tri->texCoords[l]->count; k++, indice++)
				alldata[indice] = tri->texCoords[l]->texCoords[k];


		printf("inicializando vbo  para os dados \n");
		tri->vboId = initializeVBO(vboSize*sizeof(float),GL_STATIC_DRAW, alldata);
		for(int i =0; i < MAX_VERTEX_ATTRS; i++){
			if (attrs[i]){
				attrs[i]->vboID = tri->vboId;
			}
		}
		printf("inicializando vao \n");
		//tri->vaoId = initializeIndexedVAO(tri->indicesId, attrs);
        tri->vaoId = initEmptyVAO();
        configureIndexedVAO(tri->vaoId, tri->indicesId, attrs);


		n = n->next;
		printf("vbo criada\n");
	}

}

void setmeshboundingbox(Mesh *m) {
    //memset(m->b.pmin, 9999999, 3*sizeof(float));
    //memset(m->b.pmax, -999999, 3*sizeof(float));
    m->b.pmin[0] = 99999999;
    m->b.pmin[1] = 99999999;
    m->b.pmin[2] = 99999999;

    m->b.pmax[0] = -999999999;
    m->b.pmax[1] = -999999999;
    m->b.pmax[2] = -999999999;

    //para cada triangles da lista
    for(int i = 0; i < m->tris->size; i++) {
        Triangles *t = fplist_getdata(i, m->tris);
        printf("t->verticesCount: %d\n", t->verticesCount);
        for(int j = 0; j < t->verticesCount/3; j++) {
            //X
            if(t->vertices[3*j] < m->b.pmin[0])
                m->b.pmin[0] = t->vertices[3*j];
            if (t->vertices[3*j] > m->b.pmax[0])
                m->b.pmax[0] = t->vertices[3*j];
            //Y
            if(t->vertices[3*j + 1] < m->b.pmin[1])
                m->b.pmin[1] = t->vertices[3*j + 1];
            if (t->vertices[3*j + 1] > m->b.pmax[1])
                m->b.pmax[1] = t->vertices[3*j + 1];
            //Z
            if(t->vertices[3*j + 2] < m->b.pmin[2])
                m->b.pmin[2] = t->vertices[3*j + 2];
            if (t->vertices[3*j + 2] > m->b.pmax[2])
                m->b.pmax[2] = t->vertices[3*j + 2];
        }
    }
}

Mesh* initMesh() {
    Mesh *m = malloc(sizeof(Mesh));
    memset(m, 0, sizeof(Mesh));
    m->tris = fplist_init(free);
    return m;
}

Triangles* addTris(Mesh *m) {
    Triangles *t = malloc(sizeof(Triangles));
    memset(t, 0, sizeof(Triangles));
    fplist_insback(t, m->tris);
    return t;
}

void addVertices(Triangles *t, int num, int comp, float *vertices) {
    t->verticesCount = num;
    
    //só armazena o ponteiro
    t->vertices = vertices;

    t->attrs[ATTR_VERTEX] = malloc(sizeof(VertexAttribute));
    t->attrs[ATTR_VERTEX]->count = t->verticesCount;
    t->attrs[ATTR_VERTEX]->size =  t->verticesCount*sizeof(float);
    t->attrs[ATTR_VERTEX]->type = ATTR_VERTEX;
    t->attrs[ATTR_VERTEX]->offset = 0;
    t->attrs[ATTR_VERTEX]->components = comp;
}

void addNormals(Triangles* t, int num, int comp, float *normals) {
    t->normalsCount = num;
    t->normals = normals;

    t->attrs[ATTR_NORMAL] = malloc(sizeof(VertexAttribute));
    t->attrs[ATTR_NORMAL]->count = t->normalsCount;
    t->attrs[ATTR_NORMAL]->size =  t->normalsCount*sizeof(float);
    t->attrs[ATTR_NORMAL]->type = ATTR_NORMAL;
    t->attrs[ATTR_NORMAL]->offset = 0;
    t->attrs[ATTR_NORMAL]->components = comp;
}

void addTexCoords(Triangles *t, int num, int comp, int texset, float *texcoords) {
    t->texCoords[texset] = malloc(sizeof(TexCoord));
    t->texCoords[texset]->count = num;
    t->texCoords[texset]->components = comp;
    t->texCoords[texset]->set = texset;
    t->texCoords[texset]->texCoords = texcoords;
    t->numTexSets++;

    t->attrs[ATTR_TEXCOORD0 + texset] = malloc(sizeof(VertexAttribute));
    t->attrs[ATTR_TEXCOORD0 + texset]->count = t->texCoords[texset]->count;
    t->attrs[ATTR_TEXCOORD0 + texset]->size =  t->texCoords[texset]->count*sizeof(float);
    t->attrs[ATTR_TEXCOORD0 + texset]->type = ATTR_TEXCOORD0 + texset;
    t->attrs[ATTR_TEXCOORD0 + texset]->offset = 0;
    t->attrs[ATTR_TEXCOORD0 + texset]->components = t->texCoords[texset]->components;
}

void addIndices(Triangles *t, int num, unsigned int *indices) {
    t->indicesCount = num;
    t->indices = indices;
}

void prepareMesh(Mesh *m) {
    //Para cada triangles
    
    for(fpnode *n = m->tris->first; n != NULL; n = n->next) {
        Triangles *tri = n->data;
        
        //Criar VBO de indices
        tri->indicesId = initializeVBO(tri->indicesCount*sizeof(unsigned int), GL_STATIC_DRAW, tri->indices);
        tri->verticesVBO = initializeVBO(tri->verticesCount*sizeof(float), GL_STATIC_DRAW, tri->vertices);
        tri->attrs[ATTR_VERTEX]->vboID = tri->verticesVBO;

        if(tri->normals) {
            tri->normalsVBO = initializeVBO(tri->normalsCount*sizeof(float), GL_STATIC_DRAW, tri->normals);
            tri->attrs[ATTR_NORMAL]->vboID = tri->normalsVBO;
        }
        if(tri->binormals) {
            tri->binormalsVBO = initializeVBO(tri->binormalsCount*sizeof(float), GL_STATIC_DRAW, tri->binormals);
            tri->attrs[ATTR_BINORMAL]->vboID = tri->binormalsVBO;
        }
        if(tri->tangents) {
            tri->tangentsVBO = initializeVBO(tri->tangentsCount*sizeof(float), GL_STATIC_DRAW, tri->tangents);
            tri->attrs[ATTR_TANGENT]->vboID = tri->tangentsVBO;
        }
        for(unsigned int i = 0; i < tri->numTexSets; i++) {
            tri->texVBO[i] = initializeVBO(tri->texCoords[i]->count*sizeof(float), GL_STATIC_DRAW, tri->texCoords[i]->texCoords);
            tri->attrs[ATTR_TEXCOORD0 + i]->vboID = tri->texVBO[i];
        }
        tri->vaoId = initEmptyVAO();
        configureIndexedVAO(tri->vaoId, tri->indicesId, tri->attrs);
        
    }
}


//Função auxiliar pra calcular normais, dados os índices e os vértices
//baseado
void setNormals(unsigned int *tIndices, float *tVerts, float *tNormals, 
                int indicesCount, int verticesCount) 
{
    for(int j = 0; j < indicesCount; j++) {
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
        cross(e1, e2, no);

        tNormals[3*ia] = no[0];
        tNormals[3*ia + 1] = no[1];
        tNormals[3*ia + 2] = no[2];
        tNormals[3*ib] = no[0];
        tNormals[3*ib + 1] = no[1];
        tNormals[3*ib + 2] = no[2];
        tNormals[3*ic] = no[0];
        tNormals[3*ic + 1] = no[1];
        tNormals[3*ic + 2] = no[2];
    }
    for(int j = 0; j < verticesCount; j++) {
        vec3 n = { tNormals[3*j], tNormals[3*j + 1], tNormals[3*j + 2] };
        vecNormalize(n);
        tNormals[3*j] = n[0];
        tNormals[3*j + 1] = n[1];
        tNormals[3*j + 2] = n[2];
    }
}


void updateMesh(Mesh* m, Joint *skeleton) {
    for(int i = 0; i < m->tris->size; i++) {
        Triangles *t = fplist_getdata(i, m->tris); 
        float *tVerts = mapVBO(t->verticesVBO, GL_WRITE_ONLY);
        for(int j = 0; j < t->verticesCount/3; j++) {
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
}

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

