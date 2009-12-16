#include <stdio.h>

#include "glapp.h"
<<<<<<< local
#include "math/vec3.h"
=======
>>>>>>> other

int idle(void* a){
	return 1;
}

int render(void* a){
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

	keyboard key;
	mouse m;
	mainloop(app, key, m, idle, render );

	closeVideo();
	free(app);
	return 0;
}
