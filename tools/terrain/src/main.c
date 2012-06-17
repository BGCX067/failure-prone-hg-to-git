#include <stdio.h>
#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/glime.h"
#include "renderer/camera.h"
#include "renderer/gui.h"
#include "util/shadergen.h"
#include "util/colladaloader.h"
#include "util/sdnoise1234.h"
#include "util/image.h"
#include <stdlib.h>

Scene* cena;
Shader *shdr;
Camera c;
BoundingBox bbox;
Light l;
//Material* mat;

void generateTerrain(int sizex, int sizey){
	float* terrain = malloc(sizex*sizey*sizeof(float));

    Mesh* m = initMesh();
    Triangles* t = addTris(m); 	

    unsigned char* terrainbmp = malloc(sizex*sizey*sizeof (unsigned char));
    for( int i = 0; i < sizex; i++)
	for(int j = 0; j < sizey; j++)
		terrain[j + i*sizex] = fbm2( (float) (i),  (float) (j), 10, 0.01, 4)*10;

    unsigned int* indices = malloc( sizeof(unsigned int)*sizex*sizey*2*3 );
    float* vertices = malloc( sizeof(float)*sizex*sizey*6*3 ); //cada tile sao 2 triangulos, com 3 vertices cada com 3 componentes

    for (int i = 0; i < sizex*sizey*6; i++)
	indices[i] = i;

    int index = 0;
    for (int i = 0; i < sizex; i++){
	for(int j = 0; j < sizey; j++){
		//index = i + j*512;
        
        //Vert 1
		vertices[index] = (float)i;
		vertices[index+1] = terrain[j + i*sizex];
		//vertices[index+1] = 0.0;
		vertices[index+2] = (float)j;

        //Vert 3
		vertices[index+3] = (float)i;
		vertices[index+4] = terrain[(j+1) + i*sizex];
		//vertices[index+4] = 0.0; 
		vertices[index+5] = (float)(j+1.0);

        //Vert 2
		vertices[index+6] = (float)(i+1.0);
		vertices[index+7] = terrain[j+(i+1)*sizex];
		//vertices[index+7] = 0.0;
		vertices[index+8] = (float)j;

        //Vert 4
		vertices[index+9] = (float)i;
		vertices[index+10] = terrain[(j+1) + i*sizex];
		//vertices[index+10] = 0.0;
		vertices[index+11] = (float)(j+1.0);

        //Vert 6
		vertices[index+12] = (float)(i+1.0);
		vertices[index+13] = terrain[(j+1) + (i+1)*sizex];
		//vertices[index+13] = 0.0;
		vertices[index+14] = (float)(j+1.0);

        //Vert 5
		vertices[index+15] = (float)(i+1.0);
		vertices[index+16] = terrain[j+(i+1)*sizex];
		//vertices[index+16] = 0.0;
		vertices[index+17] = (float)j;


		index += 18;
	}
    }

    float* normals = malloc(sizeof(float)*sizex*sizey*6*3);

    setNormals(indices, vertices, normals, sizex*sizey*2, sizex*sizey*6);
    addVertices(t, sizex*sizey*6*3, 3, vertices);
    addIndices(t, sizex*sizey*6, indices);
    addNormals(t, sizex*sizey*6*3, 3, normals);
    prepareMesh(m);
    addMesh(cena, m);

    t->material = colorMaterialDir();
    free(vertices);
    free(normals);
    free(indices);
    free(terrain);
}

static void subdivide(vec3 v1, vec3 v2, vec3 v3, int depth, float *vertices)
{
    vec3 v12, v23, v31;
    static int index = 0;
    if (depth == 0) {
        vertices[index] = v1[0];
        vertices[index + 1] = v1[1];
        vertices[index + 2] = v1[2];

        vertices[index + 3] = v2[0];
        vertices[index + 4] = v2[1];
        vertices[index + 5] = v2[2];

        vertices[index + 6] = v3[0];
        vertices[index + 7] = v3[1];
        vertices[index + 8] = v3[2];
        index += 9;
        return;
    }

    vecAdd(v1, v2, v12);
    vecAdd(v2, v3, v23);
    vecAdd(v3, v1, v31);

    vecNormalize(v12);
    vecNormalize(v23);
    vecNormalize(v31);

    subdivide(v1, v12, v31, depth-1, vertices);
    subdivide(v2, v23, v12, depth-1, vertices);
    subdivide(v3, v31, v23, depth-1, vertices);
    subdivide(v12, v23, v31, depth-1,vertices);
}

void generateSkyDome(int ndiv, float radius) {
    int ntris = 20*pow(4, ndiv);

    unsigned int *indices = malloc(sizeof(unsigned int)*ntris*3);
    for(int i = 0; i < 3*ntris; i++)
        indices[i] = i;

    float *vertices = malloc(sizeof(float)*ntris*3*3); // 3 vertices por triangulo, com 3 componentes
    
    const float X = 0.525731112119133606;
    const float Z = 0.850650808352039932;

    vec3 vdata[12];
    vdata[0][0] = -X; vdata[0][1] = 0.0; vdata[0][2] = Z;
    vdata[1][0] = X; vdata[1][1] = 0.0; vdata[1][2] = Z;
    vdata[2][0] = -X; vdata[2][1] = 0.0; vdata[2][2] = -Z;
    vdata[3][0] = X; vdata[3][1] = 0.0; vdata[3][2] = -Z;
    vdata[4][0] = 0.0; vdata[4][1] = Z; vdata[4][2] = X;
    vdata[5][0] = 0.0; vdata[5][1] = Z; vdata[5][2] = -X;
    vdata[6][0] = 0.0; vdata[6][1] = -Z; vdata[6][2] = X;
    vdata[7][0] = 0.0; vdata[7][1] = -Z; vdata[7][2] = -X;
    vdata[8][0] = Z; vdata[8][1] = X; vdata[8][2] = 0.0;
    vdata[9][0] = -Z; vdata[9][1] = X; vdata[9][2] = 0.0;
    vdata[10][0] = Z; vdata[10][1] = -X; vdata[10][2] = 0.0;
    vdata[11][0] = -Z; vdata[11][1] = -X; vdata[11][2] = 0.0;

    int tindices[20][3] = { {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
                            {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
                            {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
                            {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };
    for(int i = 0; i < 20; i++)
        subdivide(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]], ndiv, vertices);

    for(int i = 0; i < ntris*3*3; i++)
        vertices[i] *= radius;


    Mesh *m = initMesh();
    Triangles *t = addTris(m);
    
    float* normals = malloc(sizeof(float)*ntris*3*3);

    setNormals(indices, vertices, normals, ntris, ntris*3);
    addVertices(t, ntris*3*3, 3, vertices);
    addIndices(t, ntris*3, indices);
    addNormals(t, ntris*3*3, 3, normals);
    prepareMesh(m);
    addMesh(cena, m);

    t->material = colorMaterialDir2();

    free(vertices);
    free(normals);
    free(indices);
}


void initializeGame(){
    cena = initializeScene();
    
    generateTerrain(512, 512);
    generateSkyDome(5, 1024);

  //  stbi_write_tga("noise.tga", 512, 512, 1, terrainbmp );

    initCamera(&c, TRACKBALL);
    
    char *vertshader = readTextFile("data/shaders/phong.vert");
    char *fragshader = readTextFile("data/shaders/phong.frag");
    shdr = initializeShader(vertshader, fragshader); 

    bbox.pmin[0] = 0.0;
    bbox.pmin[1] = 0.0;
    bbox.pmin[2] = 0.0;
    bbox.pmax[0] = 512.0;
    bbox.pmax[1] = 0.0;
    bbox.pmax[2] = 512.0;
    camerafit(&c, bbox, 45.0, 800/600, 0.1, 10000.0);

    l.pos[0]= 256.0; l.pos[1] = 10.0; l.pos[2] = 256.0;
    l.color[0] = 1.0;
    l.color[1] = 1.0;
    l.color[2] = 1.0;
    l.color[3] = 1.0;
    
    //mat = colorMaterialDir();
    //glPolygonMode(GL_BACK, GL_LINE);
    glEnable(GL_CULL_FACE);
}

int Update(event* e, double* dt){
    cameraHandleEvent(&c, e);
    setupViewMatrix(&c);

}

int Render(event *e, double* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 bboxcenter;
    bboxcenter[0] = 256.0;
    bboxcenter[1] = 0.0;
    bboxcenter[2] = 256.0;
    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    fpMultMatrix(c.mvp, c.projection, c.modelview);
    setView(c.modelview);
    setProjection(c.projection);
    //setShaderConstant3f(mat->shdr, "eyepos", c.pos); 

    //bindMaterial(mat, &l); 
    //bindShader(mat->shdr);
    //drawScene(cena);
    
    
    //Desenhar cena
    if (cena->meshList){
        Mesh* m = NULL;
        for( int i = 0; i < cena->meshList->size; i++){ // para da mesh da cena
            m = fplist_getdata(i, cena->meshList);
            if (m->tris){
                Triangles* tri = NULL;
                for( int k = 0; k < m->tris->size; k++){ //para cada chunk de triangles do mesh
                    tri = fplist_getdata(k, m->tris);
                    setShaderConstant3f(tri->material, "eyepos", c.pos); 
                    bindMaterial(tri->material, &l);
                    bindShader(tri->material->shdr);
                    drawIndexedVAO(tri->vaoId, tri->indicesCount, GL_TRIANGLES);
                }
            }
        }
    }

    glFlush();
}


int main(){
	setVideoMode(800, 600, 0);
	warpmouse(0);
	setWindowTitle("Terra");
	initializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop();

	closeVideo();
	return 0;
}



