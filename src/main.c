#include <stdio.h>

#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "util/fplist.h"
#include "util/malloc.h"
//#include "util/chipmunk/chipmunk.h"

float    genericSpriteVerts[] = {
	0.0, 0.0,
	50.0, 0.0, 
	0.0,  50.0,
	50.0,  50.0,  };

float  genericTexCoords [] =  {

	0.0, 0.0,
	1.0, 0.0,
	0.0, 1.0,
	1.0, 1.0

};

unsigned int indices[] = { 0, 1, 2, 3, 2, 1};

node player;
//cpSpace *space;
//cpBody *playerBody;

int idle(float ifps, event* e, scene* s){
	
	if (e->type & KEYBOARD_EVENT){
		if (e->keys[KEY_D]){
			player.pos[0] +=3;
			if (player.pos[0] > 800)
				player.pos[0] = 800;
		}
		if (e->keys[KEY_A]){
			player.pos[0] -= 3;
			if (player.pos[0] < 0)
				player.pos[0] = 0;
		}
		if  (e->keys[KEY_W]){
			player.pos[1] += 3;
			if (player.pos[1] > 600)
				player.pos[1] = 600;
		}
		if (e->keys[KEY_S]){
			player.pos[1]  -= 3;
			if (player.pos[1] < 0)
				player.pos[1] = 0;
		}
	}

//	cpSpaceStep(space, 1.0f/60.0f);
	//printf("x:  %f y: %f \n ",playerBody->p.x, playerBody->p.y );
	
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

	setWindowTitle("Failure Prone");
	renderer* renderer  = initializeRenderer(app->width, app->height, 1.0, 1000.0, 75.0 );
	scene* s = initializeScene();

	triangles*  t  = dlmalloc(sizeof(triangles));
	memset(t, 0, sizeof(triangles));
	t->indicesCount = 6;
	t->verticesComponents = 2;
	t->verticesCount = 8;
	t->vertices =  genericSpriteVerts;
	t->indices = indices;
	printf("configurando texsets \n");
	t->texCoords[0] = dlmalloc(sizeof(texCoord));
	t->numTexSets = 1;
	t->texCoords[0]->count = 8;
	t->texCoords[0]->set = 0;
	t->texCoords[0]->components = 2;
	t->texCoords[0]->texCoords = genericTexCoords;
	printf("done  \n");

	player.pos[0] =  400;  player.pos[1] = 300; player.pos[2] =  0;

	mesh *quad = dlmalloc(sizeof(mesh));
	player.model = quad;
	quad->trianglesCount = 1;
	quad->tris = fplist_init(NULL, dlfree);
	fplist_insback( t, quad->tris);
	createVBO(quad);
    //quad->tris = fplist_init(NULL, dlfree);
	//fplist_insback(t, quad->tris);
	addMesh(s, quad);
	addNode(s, &player);
	
/*	space = cpSpaceNew();
	space->iterations = 10;
	space->gravity = cpv(0, -100);

	playerBody = cpBodyNew(10, cpMomentForCircle(10, 0.0f, 25, cpvzero));
	playerBody->p = cpv(400, 300);
	cpSpaceAddBody(space, playerBody);

	cpShape* playerShape = cpSpaceAddShape(space, cpCircleShapeNew(playerBody, 25, cpvzero));
	playerShape->e = 0;
	playerShape->u = 0.9;
*/
	mainloop(app, idle, render, s );

	closeVideo();
	free(app);
	return 0;
}



