#include <stdio.h>

#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/glime.h"
#include "util/fplist.h"
#include "renderer/particles.h"
#include "renderer/camera.h"

int idle(float ifps, event* e, Scene* s){
	return 1;
}


int samplerstate;
unsigned int particleShader;
unsigned int tex;
float elapsedTime;

particlesystem* ps;

void initializeGame(){

	samplerstate = initializeSamplerState(CLAMP, LINEAR, LINEAR, 0);

/*	quad = initializeBatch();
	begin(quad, GL_TRIANGLE_STRIP, 4, 1);
		texCoord2f(quad, 0, 0.0, 0.0);
//		normal3f(quad,0.0, 0.0, 0.0);
		vertex3f(quad, 0.0, 0.0, 0.0);

		texCoord2f(quad, 0, 1.0, 0.0);
//		normal3f(quad,1.0, 0.0, 0.0);
		vertex3f(quad, 800.0, 0.0, 0.0);

		texCoord2f(quad, 0, 0.0, 1.0);
//		normal3f(quad,0.0, 1.0, 0.0);
		vertex3f(quad, 0.0, 600.0, 0.0);

		texCoord2f(quad, 0, 1.0, 1.0);
//		normal3f(quad,1.0, 1.0, 0.0);
		vertex3f(quad, 800.0, 600.0, 0.0);
	end(quad);

//	points = malloc(sizeof(batch));
//	initializeBatch(points);
	points = initializeBatch();
	begin(points, GL_POINTS, 100000, 0);
		float A, B, C, D;
		A = 28.0; B = 46.90; C = 4; D = 0.01;
		float prevx = 0.1, prevy = 0, prevz = 0;
		float x, y, z;
		for(int i = 0; i < 100000; i++){
//			x = sin(A* prevy) - prevz *cos(B*prevx)*10;
//			z = sin(prevx)*10;
//			y = prevz * sin(C*prevx) - cos(D*prevy)*10;
//			x = y - z*y*10;
//			y = z - z*x*10;
//			z = x - y*x*10;
			x = prevx + D * A * (prevy - prevx);
			//y = prevy + D * (prevx + (B - prevz) - prevy);
			y = prevy + D*(B*prevx - prevy - prevz*prevx);
			z = prevz + D * (prevx * prevy - C * prevz);
			vertex3f(points, x, y, z);
			prevx = x;
			prevy = y;
			prevz = z;
			//normal3f(points, 0, noise(2), noise(3) );
		}
		
	end(points);
*/
//	initializeGUI(800, 600);

	vec3 pos;
	pos[0] = 7.7;
	pos[1] = -17.2;
	pos[2] = 9.7;
	vec3 force;
	force[0] = 0.0;
	force[1] = -9.8;//6.0;
	force[2] = -6.0;//-9.8;
	ps = initparticles(300, pos, 2.0, 2.0,  force);

	particleShader = initializeShader( readTextFile("data/shaders/particles.vert"), readTextFile("data/shaders/particles.frag") );

	tex = initializeTexture("data/textures/shine2.tga", TEXTURE_2D, RGB, RGB8, UNSIGNED_BYTE);

	glEnable(GL_POINT_SPRITE);

}

int render(float ifps, event *e, Scene *s){

	updateparticles(ps, ifps);

	glDepthMask(GL_FALSE);
	beginRender(e);
	glEnable(GL_BLEND);
//	glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	bindSamplerState(0, samplerstate);
	bindTexture(0, tex);
	bindShader(particleShader);
	renderparticles(ps);


//	beginGUI(e);

//	endGUI();

    glFinish();
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
	renderer* renderer = initializeRenderer(app->width, app->height, 0.1, 10000.0, 45.0, TRACKBALL);

	Scene* s = initializeScene();
	initializeGame();
	mainloop(app, idle, render, s );

	closeVideo();
	free(app);
	return 0;
}



