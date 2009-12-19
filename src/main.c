#include <stdio.h>

#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"

int idle(void* a){
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

	mainloop(app, idle, render );

	closeVideo();
	free(app);
	return 0;
}
