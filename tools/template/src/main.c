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

Scene* cena;
Shader *shdr;
renderer *mainrenderer;
Camera c;
BoundingBox bbox;

void initializeGame(){
    initCamera(&c, TRACKBALL);
    
    cena = readColladaFile("../../data/models/duck_triangulate_deindexer.dae");

    char *vertshader = readTextFile("data/shaders/phong.vert");
    char *fragshader = readTextFile("data/shaders/phong.frag");
    shdr = initializeShader(vertshader, fragshader); 
    free(vertshader);
    free(fragshader);

    camerafit(&c, cena->b, 45.0, 800/600, 0.1, 1000.0);
}

int Render(event *e, double* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 bboxcenter;
    bbcenter(cena->b, bboxcenter);

    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    setView(c.modelview);
    setProjection(c.projection); //TODO isso so precisaria ser calculado/setado 1x
    setShaderConstant3x3f(shdr, "normalmatrix", c.normalmatrix);
    setShaderConstant3f(shdr, "eyepos", c.pos);


    //Passa informações da luz pro shader
    Light *l = fplist_getdata(0, cena->lightList);

    for(int i = 0; i < cena->meshList->size; i++) {
	    Mesh *m = fplist_getdata(i, cena->meshList);
        for(int j = 0; j < m->tris->size; j++) {
            Triangles *tri = fplist_getdata(j, m->tris);
            Material *mat = tri->material;
	    mat->shdr = shdr; //TODO isso devia ser setaod em outro lugar
	    bindMaterial(mat, l);
            bindShader(shdr);
            drawIndexedVAO(tri->vaoId, tri->indicesCount, GL_TRIANGLES);
        }
    }

    glFlush();
}

int Update(event* e, double *dt){
    cameraHandleEvent(&c, e);
    setupViewMatrix(&c);

}

int main(){
	setVideoMode(800, 600, 0);
	warpmouse( 0);
	setWindowTitle("Hello World");
	initializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop();

	closeVideo();
	return 0;
}



