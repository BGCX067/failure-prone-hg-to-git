#include <stdio.h>
#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/glime.h"
#include "renderer/camera.h"
#include "util/shadergen.h"
#include "util/colladaloader.h"

int idle(float ifps, event* e, Scene* s){
	return 1;
}

Scene* cena;
Shader *shdr;
renderer *mainrenderer;
Camera c;

void initializeGame(){
    initCamera(&c, TRACKBALL);
    //cena = readColladaFile("../../data/models/duck_triangulate_deindexer.dae");
    
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

//    printf("vertex shader:\n%s\n", vertshader);
//    printf("fragment shader:\n%s\n", fragshader);

    BoundingBox bbox;
    bbox.pmin[0] = -1.0;
    bbox.pmin[1] = -1.0;
    bbox.pmin[2] = -1.0;
    bbox.pmax[0] = 1.0;
    bbox.pmax[1] = 1.0;
    bbox.pmax[2] = 1.0;
    camerafit(&c, bbox, 45.0, 800/600, 0.1, 1000.0);
}

int render(float ifps, event *e, Scene *cena){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawScene(cena);

/*    batch *tri = initializeBatch();
    begin(tri, GL_TRIANGLES, 24, 0);
        vertex3f(tri, -0.8, -0.8, -1.0);
        vertex3f(tri, 0.8, -0.8, -1.0);
        vertex3f(tri, 0.0, 0.8, -1.0);
    end(tri);

    draw(tri);*/


    glFlush();
//    swapBuffers();
/*    //transformações da camera
	beginRender(e);
	glFlush();*/
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
	setWindowTitle("Mathfeel");
	mainrenderer  = initializeRenderer(app->width, app->height, 0.1, 10000.0, 45.0, TRACKBALL);
	initializeGame();
	mainloop(app, idle, render, cena );

	closeVideo();
	free(app);
	return 0;
}



