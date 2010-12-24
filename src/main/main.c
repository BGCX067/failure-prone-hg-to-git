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
#include "renderer/camera.h"

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

renderer *mainrenderer;

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
    
    //FIXME 
    camerafit(getcamera(), cena->b, 45.0, 800.0/600.0, 1.0, 10000.0);
    printf("boundingbox da cena\n");
    printf("\tpmin: %f, %f, %f\n", cena->b.pmin[0], cena->b.pmin[1], cena->b.pmin[2]);
    printf("\tpmax: %f, %f, %f\n", cena->b.pmax[0], cena->b.pmax[1], cena->b.pmax[2]);
}

int render(float ifps, event *e, scene *cena){

	beginRender(e);


//	beginGUI(e); //TODO da pal se nao chamar o initializegui antes

//	endGUI();

	triangles* tri = duck->tris->first->data;
//	bindShader(minimalShader);
	drawScene(cena);

    batch *bbox = initializeBatch();
    begin(bbox, GL_LINES, 24, 0);
        vertex3f(bbox, cena->b.pmin[0], cena->b.pmin[1], cena->b.pmin[2]);
        vertex3f(bbox, cena->b.pmin[0], cena->b.pmin[1], cena->b.pmax[2]);

        vertex3f(bbox, cena->b.pmin[0], cena->b.pmin[1], cena->b.pmin[2]);
        vertex3f(bbox, cena->b.pmin[0], cena->b.pmax[1], cena->b.pmin[2]);

        vertex3f(bbox, cena->b.pmin[0], cena->b.pmin[1], cena->b.pmin[2]);
        vertex3f(bbox, cena->b.pmax[0], cena->b.pmin[1], cena->b.pmin[2]);

        
        vertex3f(bbox, cena->b.pmin[0], cena->b.pmax[1], cena->b.pmax[2]);
        vertex3f(bbox, cena->b.pmin[0], cena->b.pmax[1], cena->b.pmin[2]);


        vertex3f(bbox, cena->b.pmin[0], cena->b.pmax[1], cena->b.pmax[2]);
        vertex3f(bbox, cena->b.pmax[0], cena->b.pmax[1], cena->b.pmax[2]);


        vertex3f(bbox, cena->b.pmin[0], cena->b.pmax[1], cena->b.pmax[2]);
        vertex3f(bbox, cena->b.pmin[0], cena->b.pmin[1], cena->b.pmax[2]);


        vertex3f(bbox, cena->b.pmax[0], cena->b.pmax[1], cena->b.pmin[2]);
        vertex3f(bbox, cena->b.pmax[0], cena->b.pmax[1], cena->b.pmax[2]);

        
        vertex3f(bbox, cena->b.pmax[0], cena->b.pmax[1], cena->b.pmin[2]);
        vertex3f(bbox, cena->b.pmin[0], cena->b.pmax[1], cena->b.pmin[2]);

        vertex3f(bbox, cena->b.pmax[0], cena->b.pmax[1], cena->b.pmin[2]);
        vertex3f(bbox, cena->b.pmax[0], cena->b.pmin[1], cena->b.pmin[2]);

        vertex3f(bbox, cena->b.pmax[0], cena->b.pmin[1], cena->b.pmax[2]);
        vertex3f(bbox, cena->b.pmin[0], cena->b.pmin[1], cena->b.pmax[2]);

        vertex3f(bbox, cena->b.pmax[0], cena->b.pmin[1], cena->b.pmax[2]);
        vertex3f(bbox, cena->b.pmax[0], cena->b.pmax[1], cena->b.pmax[2]);

        vertex3f(bbox, cena->b.pmax[0], cena->b.pmin[1], cena->b.pmax[2]);
        vertex3f(bbox, cena->b.pmax[0], cena->b.pmin[1], cena->b.pmin[2]);
    end(bbox);

    draw(bbox);


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
	//renderer* renderer  = initializeRenderer(app->width, app->height, 1.0, 1000.0, 45.0, TRACKBALL);
	mainrenderer  = initializeRenderer(app->width, app->height, 0.1, 10000.0, 45.0, TRACKBALL);
	initializeGame();
	mainloop(app, idle, render, cena );

	closeVideo();
	free(app);
	return 0;
}



