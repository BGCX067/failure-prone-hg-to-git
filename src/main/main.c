#include <stdio.h>

#include "glapp.h"
#include "math/vec3.h"
#include "math/matrix.h"
#include "math/algebra.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/glime.h"
#include "renderer/light.h"
#include "util/fplist.h"
#include "renderer/particles.h"
#include "renderer/camera.h"

#include "util/perlin.h"
#include "renderer/shadowmap.h"

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
unsigned int minimalTex;
unsigned int shadowShader;
unsigned int texShader;
unsigned int tex;

float elapsedTime;

scene* cena;
mesh* duck; //pato dentro da scena

int shadowheight = 1200;
int shadowwidth = 1600;
int texid;
int navetex;
renderer *mainrenderer;

void initializeGame(){

	samplerstate = initializeSamplerState(CLAMP, NEAREST, NEAREST, 1.0);

	initializeGUI(800, 600);

	navetex = initializeTexture("data/textures/nave.jpg", TEXTURE_2D, RGB, RGB, UNSIGNED_BYTE);

	minimalShader = initializeShader( readTextFile("data/shaders/minimal.vert"), readTextFile("data/shaders/minimal.frag") );
	minimalTex = initializeShader( readTextFile("data/shaders/minimal.vert"), readTextFile("data/shaders/minimaltexture.frag"));
	shadowShader = initializeShader( readTextFile("data/shaders/minimalshadow.vert"), readTextFile("data/shaders/minimalshadow.frag") );
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

	initializeShadowFBO(1600, 1200, &texid);
    
    //FIXME 
    camerafit(getcamera(), cena->b, 45.0, 800.0/600.0, 1.0, 10000.0);
}

int render(float ifps, event *e, scene *cena){

	beginRender(e);


	vec3 lightpos;
	lightpos[0] = 456.0; lightpos[1] = -47.0;  lightpos[2] = 48.0;
	//lightpos[0] = 0.0; lightpos[1] = 0.0;  lightpos[2] = 603.0;
	//lightpos[0] = 579.36; lightpos[1] = 90.157;  lightpos[2] = 106.136;
	vec3  lightlook;
	lightlook[0] = 0.0; lightlook[1] = 0.0; lightlook[2] = -1.0;
//	lightlook[0] = -0.967; lightlook[1] = -0.107; lightlook[2] = -0.228;
	mat4 shadowmat;

	mat4 lightmvp;
	beginShadow(shadowwidth, shadowheight, lightpos, lightlook, shadowmat, lightmvp);
	//	for(int i = 0; i < 16; i++)
	//		printf("lightmvp[%d]: %f \n", i, lightmvp[i]);
		setShaderConstant4x4f(minimalShader, "lightmvp", lightmvp);
		bindShader(minimalShader);
		drawScene(cena);
	endShadow();
	//for (int i = 0; i < 16; i++)
	//	printf("shadowmat[%d]: %f \n", i, shadowmat[i]);
	bindTexture(0, texid);
//	bindTexture(0, navetex);
	bindSamplerState(0, samplerstate);
//	bindSamplerState(1, samplerstate);
/*	shadowmat[0] = -0.322;
	shadowmat[1] = -0.665;
	shadowmat[2] = -0.975;
	shadowmat[3] = -0.972;
	shadowmat[4] = -0.075;
	shadowmat[5] = 1.117;
	shadowmat[6] = -0.151;
	shadowmat[7] = -0.151;
	shadowmat[8] = -0.979;
	shadowmat[9] = -0.122;
	shadowmat[10] = -0.178;
	shadowmat[11] = -0.178;
	shadowmat[12] = 297.884;
	shadowmat[13] = 297.876;
	shadowmat[14] = 586.006;
	shadowmat[15] = 585.859;*/
	setShaderConstant4x4f(shadowShader, "shadowmat", shadowmat);
//	setShaderConstant4x4f(shadowShader, "lightmvp" , lightmvp);
	bindShader(shadowShader);
	drawScene(cena);

	beginGUI(e); //TODO da pal se nao chamar o initializegui antes
		rect r;
		r.x = 10;
		r.y = 10;
		r.w  = 100;
		r.h = 100;
		rect rzoom;
		rzoom.x = 0;
		rzoom.y = 0;
		rzoom.w = 0;
		rzoom.h = 0;
		bindSamplerState(0, samplerstate);
		doTextureView(&r, texid, &rzoom, -1, 1.0, 0.0, 0, 1, 2, 3);

	endGUI();


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

	initPerlin();
	unsigned char* noise = malloc(sizeof (unsigned char) * 512*512*3);
	int l = 0;
	for(int i = 0; i < 512*3; i+=3)
		for (int j = 0; j < 512*3; j+=3){
			float x = (float)i/256.0*1.0;
			float y = (float)j/256.0*1.0;
//			printf("%f %f \n", x, y);
			float loko =  perlinNoise2(x, y, 24, 2, 2)*127+128;//turbulence2(x, y, 8)*127 + 128;//perlinNoise2(x, y, 12, 2, 2)*255;
			noise[512*i+j] =  (unsigned char)lerp(loko, 127, 0.1);
			noise[512*i+j+1] = (unsigned char) lerp(loko, 15, 0.5);
			noise[512*i+j+2] = (unsigned char) lerp(loko, 31, 0.5);
//			printf("%d %d %d \n", i, j, noise[512*i+j]);
			l++;
		}

/*	for(float a = -20.0; a < 20.0; a += 0.01){
		float loko = noise1(a);
		float loko2 = noise1(0.5*a);
		float loko3 = noise1(0.2*a);
		float loko4 = noise1(0.1*a);
		float loko5 = perlinNoise1(a, 8, 1.5, 2);
		float sum = loko  + loko2 +  loko3 + loko4 + loko5;
		printf("%f %f %f %f %f %f %f \n", a, loko, loko2, loko3, loko4, loko5, sum);
	}
*/
	stbi_write_tga("noise.tga", 512, 512, 3, noise);

	setWindowTitle("Mathfeel");
	//renderer* renderer  = initializeRenderer(app->width, app->height, 1.0, 1000.0, 45.0, TRACKBALL);
	mainrenderer  = initializeRenderer(app->width, app->height, 0.1, 10000.0, 45.0, TRACKBALL);
	initializeGame();
	mainloop(app, idle, render, cena );

	closeVideo();
	free(app);
	return 0;
}



