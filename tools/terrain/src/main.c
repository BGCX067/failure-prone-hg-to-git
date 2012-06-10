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

Scene* cena;
Shader *shdr;
Camera c;
BoundingBox bbox;
Mesh* m;
Light l;
Material* mat;

float* generateTerrain(int sizex, int sizey){

	float* terrain = malloc(sizex*sizey*sizeof(float));

/*	for(int i = 0; i < sizex; i++)
		for (int j = 0; j < sizey; j++)
				terrain[i][j] = 0;
*/
	return terrain;
}


void initializeGame(){

    cena = initializeScene();
    m = initMesh();
    Triangles* t = addTris(m); 	

    float* terrain = generateTerrain(512, 512);
    unsigned char* terrainbmp = malloc(512*512*sizeof (unsigned char));
    for( int i = 0; i < 512; i++)
	for(int j = 0; j < 512; j++)
		terrain[j + i*512] = fbm2( (float) (i),  (float) (j), 10, 0.01, 4)*10;

    unsigned int* indices = malloc( sizeof(unsigned int)*512*512*2*3 );
    float* vertices = malloc( sizeof(float)*512*512*6*3 ); //cada tile sao 2 triangulos, com 3 vertices cada com 3 componentes

    for (int i = 0; i < 512*512*6; i++)
	indices[i] = i;

    int index = 0;
    for (int i = 0; i < 512; i++){
	for(int j = 0; j < 512; j++){
		//index = i + j*512;
        
        //Vert 1
		vertices[index] = (float)i;
		vertices[index+1] = terrain[j + i*512];
		//vertices[index+1] = 0.0;
		vertices[index+2] = (float)j;

        //Vert 3
		vertices[index+3] = (float)i;
		vertices[index+4] = terrain[(j+1) + i*512];
		//vertices[index+4] = 0.0; 
		vertices[index+5] = (float)(j+1.0);

        //Vert 2
		vertices[index+6] = (float)(i+1.0);
		vertices[index+7] = terrain[j+(i+1)*512];
		//vertices[index+7] = 0.0;
		vertices[index+8] = (float)j;

        //Vert 4
		vertices[index+9] = (float)i;
		vertices[index+10] = terrain[(j+1) + i*512];
		//vertices[index+10] = 0.0;
		vertices[index+11] = (float)(j+1.0);

        //Vert 6
		vertices[index+12] = (float)(i+1.0);
		vertices[index+13] = terrain[(j+1) + (i+1)*512];
		//vertices[index+13] = 0.0;
		vertices[index+14] = (float)(j+1.0);

        //Vert 5
		vertices[index+15] = (float)(i+1.0);
		vertices[index+16] = terrain[j+(i+1)*512];
		//vertices[index+16] = 0.0;
		vertices[index+17] = (float)j;


		index += 18;
	}
    }

    float* normals = malloc(sizeof(float)*512*512*6*3);

    setNormals(indices, vertices, normals, 512*512*2, 512*512*6);
    addVertices(t, 512*512*6*3, 3, vertices);
    addIndices(t, 512*512*6, indices);
    addNormals(t, 512*512*6*3, 3, normals);
    prepareMesh(m);
    addMesh(cena, m);

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
    camerafit(&c, bbox, 45.0, 800/600, 0.1, 1000.0);

    l.pos[0]= 256.0; l.pos[1] = 10.0; l.pos[2] = 256.0;
    l.color[0] = 1.0;
    l.color[1] = 1.0;
    l.color[2] = 1.0;
    l.color[3] = 1.0;

    mat = colorMaterialDir();

    /*mat.shininess = 2.0;
    mat.ks[0] = 1.1; mat.ks[1] = 0.1; mat.ks[2] = 0.1; mat.ks[3] = 1.0;
    mat.kd[0] = 1.1; mat.kd[1] = 0.1; mat.kd[2] = 0.1; mat.kd[3] = 1.0;
    mat.ka[0] = 1.1; mat.ka[1] = 0.1; mat.ka[2] = 0.1; mat.ka[3] = 1.0;
    mat.ke[0] = 1.1; mat.ke[1] = 0.1; mat.ke[2] = 0.1; mat.ke[3] = 1.0;
    mat.diffsource = NULL;
    mat.shdr = shdr;
*/
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
    setShaderConstant3f(mat->shdr, "eyepos", c.pos); 

    glPolygonMode(GL_BACK, GL_LINE);
    bindMaterial(mat, &l); 
    bindShader(mat->shdr);
    drawScene(cena);

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



