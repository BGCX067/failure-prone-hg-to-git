#include <stdio.h>

#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "util/fplist.h"
#include "util/malloc.h"

int idle(float ifps, event* e, scene* s){
	
	

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
	mesh *quad = dlmalloc(sizeof(mesh));
	addMesh(s, quad);
	
	mainloop(app, idle, render, s );

	closeVideo();
	free(app);
	return 0;
}
