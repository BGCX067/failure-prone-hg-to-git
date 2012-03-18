#include <stdio.h>
#include <stdlib.h> //malloc
#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/glime.h"
#include "renderer/camera.h"
#include "renderer/gui.h"
#include "util/colladaloader.h"
#include "util/textfile.h"

int idle(float ifps, event* e, Scene* s){
	return 1;
}

Scene* cena;
Shader *shdr;
renderer *mainrenderer;
Camera c;
BoundingBox bbox;

void initializeGame(){
    initCamera(&c, TRACKBALL);
    
    Mesh *m = initMesh();
    Triangles *t = addTris(m);

    float *vertices = malloc(sizeof(float)*9);
    vertices[0] = -0.8; vertices[1] = -0.8; vertices[2] = -1.0;
    vertices[3] = 0.8; vertices[4] = -0.8; vertices[5] = -1.0;
    vertices[6] = 0.0; vertices[7] = 0.8; vertices[8] = -1.0;

    unsigned int *indices = malloc(sizeof(unsigned int)*3);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    
    addVertices(t, 9, 3, vertices);
    addIndices(t, 3, indices);
    prepareMesh(m);

    cena = initializeScene();
    addMesh(cena, m);

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

    initializeGUI(800, 600);

}

int render(float ifps, event *e, Scene *cena){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //handle camera events
    cameraHandleEvent(&c, e);
    //setup matrixes
    setupViewMatrix(&c);

    vec3 bboxcenter;
    bboxcenter[0] = 0.0;
    bboxcenter[1] = 0.0;
    bboxcenter[2] = -1.0;

    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    fpMultMatrix(c.mvp, c.projection, c.modelview);
    setShaderConstant4x4f(shdr, "mvp", c.mvp);

    bindShader(shdr);
    drawScene(cena);

	beginGUI(e);
		rect r;
		r.x = 50;
		r.y = 50;
		doButton(1, &r, "Button");
	endGUI();

    glFlush();

    return 1;
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
	setWindowTitle("Font Test");
	mainrenderer  = initializeRenderer(app->width, app->height, 0.1, 10000.0, 45.0, TRACKBALL);
	initializeGame();
	mainloop(app, idle, render, cena );

	closeVideo();
	free(app);
	return 0;
}



