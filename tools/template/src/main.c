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

Scene* cena;
Shader *shdr;
renderer *mainrenderer;
Camera c;
BoundingBox bbox;

void initializeGame(){
    initCamera(&c, TRACKBALL);
    
    cena = readColladaFile("../../data/models/duck_triangulate_deindexer.dae");

    char *vertshader = readTextFile("data/shaders/vertshader.vert");
    char *fragshader = readTextFile("data/shaders/fragshader.frag");
    shdr = initializeShader(vertshader, fragshader); 

    camerafit(&c, cena->b, 45.0, 800/600, 0.1, 1000.0);
}

int render(float ifps, event *e, Scene *cena){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //handle camera events
    cameraHandleEvent(&c, e);
    //setup matrixes
    setupViewMatrix(&c);

    vec3 bboxcenter;
    bbcenter(cena->b, bboxcenter);

    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    fpMultMatrix(c.mvp, c.projection, c.modelview);
    setShaderConstant4x4f(shdr, "mvp", c.mvp);
    setShaderConstant4x4f(shdr, "modelview", c.modelview);
    setShaderConstant3x3f(shdr, "normalmatrix", c.normalmatrix);
    setShaderConstant3f(shdr, "eyepos", c.pos);


    //Passa informações da luz pro shader
    Light *l = fplist_getdata(0, cena->lightList);
    setShaderConstant3f(shdr, "lightpos", l->pos);
    setShaderConstant3f(shdr, "lightintensity", l->color);

    for(int i = 0; i < cena->meshList->size; i++) {
	    Mesh *m = fplist_getdata(i, cena->meshList);
        for(int j = 0; j < m->tris->size; j++) {
            Triangles *tri = fplist_getdata(j, m->tris);
            Material *mat = tri->material;
            //Passsa material pro shader
            setShaderConstant3f(shdr, "ka", mat->ka);
            setShaderConstant3f(shdr, "ks", mat->ks);
            setShaderConstant1f(shdr, "shininess", mat->shininess);
            
            if(mat->diffsource == TEXTURE) {
                bindSamplerState(mat->diffmap->state, 0);
                bindTexture(mat->diffmap, 0);
            }
            bindShader(shdr);
            drawIndexedVAO(tri->vaoId, tri->indicesCount, GL_TRIANGLES);
        }
    }

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



