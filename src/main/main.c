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

int samplerstate;
unsigned int minimalShader;
float elapsedTime;

Scene* cena;
renderer *mainrenderer;

void initializeGame(){
    cena = readColladaFile("data/models/duck_triangulate_deindexer.dae");

    shaderflags f;
    f.flags = PHONG | TEX;
    char *vertShader, *fragShader;
    shadergen(f, &vertShader, &fragShader);
    
/*    printf("VERTEX SHADER:\n");
    printf("%s\n\n", vertShader);

    printf("FRAG SHADER:\n");
    printf("%s\n\n", fragShader);*/

    minimalShader = initializeShader(vertShader, fragShader);
}

int render(float ifps, event *e, Scene *cena){
/*
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
	mainloop(app, idle, 0, cena );

	closeVideo();
	free(app);
	return 0;
}



