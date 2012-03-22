#include <stdio.h>
#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/glime.h"
#include "renderer/camera.h"
#include "util/colladaloader.h"

int idle(float ifps, event* e, Scene* s){
	return 1;
}

Shader *shdr;
Camera c;
BoundingBox bbox;

Framebuffer *fb;
Scene *cena;

void drawSceneOnce() {
    Camera c2;
    initCamera(&c2, TRACKBALL);

    Scene* cena2 = readColladaFile("../../data/models/duck_triangulate_deindexer.dae");
    char *vertshader = readTextFile("data/shaders/phong.vert");
    char *fragshader = readTextFile("data/shaders/phong.frag");
    Shader *shdr2 = initializeShader(vertshader, fragshader);

    camerafit(&c2, cena2->b, 45.0, 800/600, 0.1, 1000.0);

    bindFramebuffer(fb);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //setup matrixes
    setupViewMatrix(&c2);
    
    vec3 bboxcenter;
    bbcenter(cena2->b, bboxcenter);

    //translada para o centro
    fptranslatef(c2.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    fpMultMatrix(c2.mvp, c2.projection, c2.modelview);
    setShaderConstant4x4f(shdr2, "mvp", c2.mvp);
    setShaderConstant4x4f(shdr2, "modelview", c2.modelview);
    setShaderConstant3x3f(shdr2, "normalmatrix", c2.normalmatrix);
    setShaderConstant3f(shdr2, "eyepos", c2.pos);


    //Passa informações da luz pro shader
    Light *l = fplist_getdata(0, cena2->lightList);
    setShaderConstant3f(shdr2, "lightpos", l->pos);
    setShaderConstant3f(shdr2, "lightintensity", l->color);
    
    for(int i = 0; i < cena2->meshList->size; i++) {
	    Mesh *m = fplist_getdata(i, cena2->meshList);
        for(int j = 0; j < m->tris->size; j++) {
            Triangles *tri = fplist_getdata(j, m->tris);
            Material *mat = tri->material;
            //Passsa material pro shader
            setShaderConstant3f(shdr2, "ka", mat->ka);
            setShaderConstant3f(shdr2, "ks", mat->ks);
            setShaderConstant1f(shdr2, "shininess", mat->shininess);
            
            if(mat->diffsource == TEXTURE) {
                bindSamplerState(mat->diffmap->state, 0);
                bindTexture(mat->diffmap, 0);
            }
            bindShader(shdr2);
            drawIndexedVAO(tri->vaoId, tri->indicesCount, GL_TRIANGLES);
        }
    }

    bindFramebuffer(NULL);
}

void initializeGame(){
    initCamera(&c, TRACKBALL);

    char *vertshader = readTextFile("data/shaders/vertshader.vert");
    char *fragshader = readTextFile("data/shaders/fragshader.frag");
    shdr = initializeShader(vertshader, fragshader); 
    
    fb = initializeFramebuffer(512, 512);
    
    Mesh *m = initMesh();
    Triangles *t = addTris(m);

    float *vertices = malloc(sizeof(float)*9);
    vertices[0] = -0.8; vertices[1] = -0.8; vertices[2] = -1.0;
    vertices[3] = 0.8; vertices[4] = -0.8; vertices[5] = -1.0;
    vertices[6] = 0.0; vertices[7] = 0.8; vertices[8] = -1.0;

    float *texcoords = malloc(sizeof(float)*6);
    texcoords[0] = 0.0; texcoords[1] = 0.0;
    texcoords[2] = 1.0; texcoords[3] = 0.0;
    texcoords[4] = 0.5; texcoords[5] = 1.0;

    float *normals = malloc(sizeof(float)*9);
    normals[0] = 0.0; normals[1] = 0.0; normals[2] = 1.0;
    normals[3] = 0.0; normals[4] = 0.0; normals[5] = 1.0;
    normals[6] = 0.0; normals[7] = 0.0; normals[8] = 1.0;

    unsigned int *indices = malloc(sizeof(unsigned int)*3);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    
    addVertices(t, 9, 3, vertices);
    addIndices(t, 3, indices);
    addTexCoords(t, 6, 2, 0, texcoords);
    addNormals(t, 9, 3, normals);
    prepareMesh(m);

    cena = initializeScene();
    addMesh(cena, m);
    bbox.pmin[0] = -1.0;
    bbox.pmin[1] = -1.0;
    bbox.pmin[2] = -1.0;
    bbox.pmax[0] = 1.0;
    bbox.pmax[1] = 1.0;
    bbox.pmax[2] = 0.0;
    camerafit(&c, bbox, 45.0, 800/600, 0.1, 1000.0);

    drawSceneOnce();

    glClearColor(0.5, 0.5, 0.5, 1.0);
}

int render(float ifps, event *e, Scene *cena){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cameraHandleEvent(&c, e);
    setupViewMatrix(&c);

    vec3 bboxcenter;
    bbcenter(bbox, bboxcenter);

    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    fpMultMatrix(c.mvp, c.projection, c.modelview);
    setShaderConstant4x4f(shdr, "mvp", c.mvp);
    for(int i = 0; i < cena->meshList->size; i++) {
	    Mesh *m = fplist_getdata(i, cena->meshList);
        for(int j = 0; j < m->tris->size; j++) {
            Triangles *tri = fplist_getdata(j, m->tris);
            bindSamplerState(fb->tex->state, 0);
            bindTexture(fb->tex, 0);

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
	renderer *mainrenderer  = initializeRenderer(app->width, app->height, 0.1, 10000.0, 45.0, TRACKBALL);
	initializeGame();
	mainloop(app, idle, render, cena );

	closeVideo();
	free(app);
	return 0;
}



