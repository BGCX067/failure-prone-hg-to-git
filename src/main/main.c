#include <stdio.h>

#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/glime.h"
#include "renderer/light.h"
#include "util/fplist.h"
#include "renderer/particles.h"

int idle(float ifps, event* e, scene* s){
	

	//printf("x:  %f y: %f \n ",playerBody->p.x, playerBody->p.y );
	
	return 1;
}


batch* cube;
batch* quad;
batch* points;
batch* star;

int samplerstate;
unsigned int minimalShader;
unsigned int texShader;
unsigned int tex;

float elapsedTime;

scene* cena;
mesh* duck; //pato dentro da scena

void initializeGame(){

	samplerstate = initializeSamplerState(CLAMP, LINEAR, LINEAR, 0);

//	initializeGUI(800, 600);

//	minimalShader = initializeShader( readTextFile("data/shaders/minimal.vert"), readTextFile("data/shaders/minimal.frag") );
	cena = initializeDae("data/models/spaceship2.dae");
	light* l = malloc(sizeof(light));
	l->pos[0] = 10;
	l->pos[1] = 100;
	l->pos[2] = 200;
	l->color[0] = 0.4;
	l->color[1] = 1.0;
	l->color[2] = 0.1;
	l->color[3] = 1.0;
	addLight(cena, l);
	setupScene(cena);

}

int render(float ifps, event *e, scene *cena){

	beginRender(e);


//	beginGUI(e); //TODO da pal se nao chamar o initializegui antes

//	endGUI();

	triangles* tri = duck->tris->first->data;
//	bindShader(minimalShader);
	drawScene(cena);

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

	setWindowTitle("Mathfeel");
	renderer* renderer  = initializeRenderer(app->width, app->height, 1.0, 1000.0, 75.0 );
	initializeGame();
	mainloop(app, idle, render, cena );

	closeVideo();
	free(app);
	return 0;
}



