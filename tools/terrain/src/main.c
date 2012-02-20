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

int idle(float ifps, event* e, Scene* s){
	return 1;
}

Scene* cena;
Shader *shdr;
renderer *mainrenderer;
Camera c;
BoundingBox bbox;
batch *b;

float* generateTerrain(int sizex, int sizey){

	float* terrain = malloc(sizex*sizey*sizeof(float));

/*	for(int i = 0; i < sizex; i++)
		for (int j = 0; j < sizey; j++)
				terrain[i][j] = 0;
*/
	return terrain;
}


void initializeGame(){

    float* terrain = generateTerrain(512, 512);
    unsigned char* terrainbmp = malloc(512*512*sizeof (unsigned char));
    for( int i = 0; i < 512; i++)
	for(int j = 0; j < 512; j++)
		terrain[j + i*512] = fbm2( (float) (i),  (float) (j), 10, 0.01, 4)*10;

    b = initializeBatch();
    begin(b, GL_TRIANGLES, 512*512, 0);
    for (int i = 0; i < 511; i++){
	for(int j = 0; j < 511; j++){
		float x, y;
		x = (float)i;
		y = (float)j;
		//float n = turbulence2( (float) (i),  (float) (j), 0.01, 2.0, 4); //, 2, 2.0 );
//		float n = fbm2( (float) (i),  (float) (j), 10, 0.01, 4);
//		printf("n: %f x: %f y: %f \n ", n, x, y );
		//n = 1;
		vertex3f(b, (float) i, terrain[j + i*512], (float) j);
		vertex3f(b, (float) (i + 1.0), terrain[j + (i+1)*512] , (float)j );
		vertex3f(b, (float)i, terrain[(j+1) + i*512], (float) (j+1.0));

		vertex3f(b, (float)i, terrain[(j+1) + i*512] , (float) (j+1.0));
		vertex3f(b, (float)(i+1.0), terrain[j + (i+1)*512], (float) (j)); 
		vertex3f(b, (float)(i+1.0), terrain[(j+1) + (i+1)*512], (float) (j+1.0));
//		terrainbmp[i + j*512] = 255 - 255*(max - n)/(max-min); 
	}
    }   
    end(b);

  //  stbi_write_tga("noise.tga", 512, 512, 1, terrainbmp );

    initCamera(&c, TRACKBALL);
    
    char *vertshader = readTextFile("data/shaders/vertshader.vert");
    char *fragshader = readTextFile("data/shaders/fragshader.frag");
    shdr = initializeShader(vertshader, fragshader); 

    bbox.pmin[0] = -1.0;
    bbox.pmin[1] = -1.0;
    bbox.pmin[2] = -1.0;
    bbox.pmax[0] = 1.0;
    bbox.pmax[1] = 1.0;
    bbox.pmax[2] = -1.0;
    camerafit(&c, bbox, 45.0, 800/600, 0.1, 1000.0);
}

int render(float ifps, event *e, Scene *cena){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //handle camera events
    cameraHandleEvent(&c, e);
    //setup matrixes
    setupViewMatrix(&c);

    vec3 bboxcenter;
    bboxcenter[0] = 50.0;
    bboxcenter[1] = 0.0;
    bboxcenter[2] = 256.0;
    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    fpMultMatrix(c.mvp, c.projection, c.modelview);
    setShaderConstant4x4f(shdr, "mvp", c.mvp);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    bindShader(shdr);
    draw(b);

    glFlush();
}


int main(){
	glapp* app = setVideoMode(800, 600, 0);
	if (!app){
		printf("Invalid Video Mode\n");
		return 1;
	}else{
		printf("Video mode: w:  %d h: %d depth: %d \n", app->width, app->height, app->depth);
	}
	warpmouse( app, 0);
	setWindowTitle("Terra");
	mainrenderer  = initializeRenderer(app->width, app->height, 0.1, 10000.0, 45.0, TRACKBALL);
	initializeGame();
	mainloop(app, idle, render, cena );

	closeVideo();
	free(app);
	return 0;
}



