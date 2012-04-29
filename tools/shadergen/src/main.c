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
#include "util/shadergen.h"

int idle(float ifps, event* e, Scene* s){
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

Scene* cena;
Camera c;
renderer *mainrenderer;

void initializeGame(){
    initCamera(&c, TRACKBALL);
	samplerstate = initializeSamplerState(CLAMP, LINEAR, LINEAR, 0);
    tex = initializeTexture("../../data/textures/duckCM.tga", TEXTURE_2D, RGB, RGB8, UNSIGNED_BYTE);

    //cena = initializeDae("../../data/models/duck_triangulate_deindexer.dae");
    cena = readColladaFile("../../data/models/duck_triangulate_deindexer.dae");
	//minimalShader = initializeShader( readTextFile("../../data/shaders/minimal.vert"), readTextFile("../../data/shaders/minimal.frag") );
    

	Light* l = malloc(sizeof(Light));
	l->pos[0] = 10;
	l->pos[1] = 100;
	l->pos[2] = 200;
	l->color[0] = 1.0;
	l->color[1] = 1.0;
	l->color[2] = 1.0;
	l->color[3] = 1.0;
	addLight(cena, l);
	setupScene(cena);
    
    //FIXME 
    camerafit(&c, cena->b, 45.0, 800.0/600.0, 1.0, 10000.0);

    shaderflags f;
    f.flags = PHONG | TEX;
    char *vertShader, *fragShader;
    shadergen(f, &vertShader, &fragShader);
    
    printf("VERTEX SHADER:\n");
    printf("%s\n\n", vertShader);

    printf("FRAG SHADER:\n");
    printf("%s\n\n", fragShader);

    minimalShader = initializeShader(vertShader, fragShader);
}

int render(float ifps, event *e, Scene *cena){

	beginRender(e);

    vec3 bcenter;
    bbcenter(cena->b, bcenter);

    mat4 mvcpy;
    fptranslatef(c.modelview, -bcenter[0], -bcenter[1], -bcenter[2]);
    fpMultMatrix(c.mvp, c.projection, c.modelview);

    float kd[4], ks[4], ka[4], shininess, pos[3], color[4];
    shininess  = 4.0;

    ks[0] = 0.3;
    ks[1] = 0.3;
    ks[2] = 0.3;
    ks[3] = 1.0;
    ka[0] = 0.1;
    ka[1] = 0.1;
    ka[2] = 0.1;
    ka[3] = 1.0;
    kd[0] = 0.6;
    kd[1] = 0.6;
    kd[2] = 0.6;
    kd[3] = 1.0;

    pos[0] = 10;
    pos[1] = 100;
    pos[2] = 200;

    color[0] = 0.4;
    color[1] = 1.0;
    color[2] = 0.1;
    color[3] = 1.0;

    setShaderConstant4f(minimalShader, "Kd", kd );
	setShaderConstant4f(minimalShader, "Ka", ka );
	setShaderConstant4f(minimalShader, "Ks", ks );
	setShaderConstant1f(minimalShader, "shininess", shininess);

	setShaderConstant3f(minimalShader, "LightPosition", pos);
	setShaderConstant4f(minimalShader, "LightColor", color);

	float ambient[4];
	ambient[0] = 0.1;
	ambient[1] = 0.1;
	ambient[2] = 0.1;
	ambient[3] = 0.1;
	setShaderConstant4f(minimalShader, "globalAmbient", ambient);
    
    bindSamplerState(0, samplerstate);
    bindTexture(0, tex);
	bindShader(minimalShader);
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
	warpmouse( app, 0);
	setWindowTitle("Mathfeel");
	mainrenderer  = initializeRenderer(app->width, app->height, 0.1, 10000.0, 45.0, TRACKBALL);
	initializeGame();
	mainloop(app, idle, render, cena );

	closeVideo();
	free(app);
	return 0;
}



