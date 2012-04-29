#include <stdio.h>
#include "glapp.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/camera.h"
#include "renderer/sprite.h"
#include "util/textfile.h"
#include "util/image.h"
#include <stdlib.h>

Shader *shdr;
renderer *mainrenderer;
Camera c;
sprite *sp;


/*Mesh *m;
unsigned int texIDs[2];*/

void initializeGame(){
    initCamera(&c, TRACKBALL);
    
    /*char *vertshader = readTextFile("data/shaders/phong.vert");
    char *fragshader = readTextFile("data/shaders/phong.frag");
    
    shdr = initializeShader(vertshader, fragshader); 
    
    m = initMesh();
    Triangles *t = addTris(m);

    float *vertices = malloc(sizeof(float)*12);
    vertices[0] = -1.0; vertices[1] = -1.0; vertices[2] = 0.0;
    vertices[3] = 1.0; vertices[4] = -1.0; vertices[5] = 0.0;
    vertices[6] = 1.0; vertices[7] = 1.0; vertices[8] = 0.0;
    vertices[9] = -1.0; vertices[10] = 1.0; vertices[11] = 0.0;

    float *texcoords = malloc(sizeof(float)*8);
    texcoords[0] = 0.0; texcoords[1] = 1.0;
    texcoords[2] = 1.0; texcoords[3] = 1.0;
    texcoords[4] = 1.0; texcoords[5] = 0.0;
    texcoords[6] = 0.0; texcoords[7] = 0.0;

    unsigned int *indices = malloc(sizeof(unsigned int)*6);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    addVertices(t, 12, 3, vertices);
    addIndices(t, 6, indices);
    addTexCoords(t, 8, 2, 0, texcoords);
    prepareMesh(m);

    glGenTextures(2, texIDs);
    int x, y, n;
    unsigned char *data = stbi_load("data/sprites/ex1.png", &x, &y, &n, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texIDs[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    stbi_image_free(data);

    int uniloc = glGetUniformLocation(shdr->progid, "tex");
    glUniform1i(uniloc, 0);

    data = stbi_load("data/sprites/normalmap.png", &x, &y, &n, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texIDs[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    stbi_image_free(data);

    uniloc = glGetUniformLocation(shdr->progid, "normalmap");
    glUniform1i(uniloc, 1);*/

    sp = initializeSprite();
    addSprites(sp, "data/sprites/ex%d.png", 1, 0.1);
}

int render(float ifps, event *e, Scene *cena){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    drawSprite(sp, 0, 0, 0.f, 0, FLIP_Y);
    //glBindTexture(gl)
    
    /*bindShader(shdr);
    Triangles *tri = m->tris->first->data;
    drawIndexedVAO(tri->vaoId, tri->indicesCount, GL_TRIANGLES);*/
    

    glFlush();
}

int idle(float ifps, event* e, Scene* s){
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
	warpmouse( app, 0);
	setWindowTitle("Mathfeel");
	mainrenderer  = initializeRenderer(app->width, app->height, 0.1, 10000.0, 45.0, TRACKBALL);
	initializeGame();
	mainloop(app, idle, render, NULL );

	closeVideo();
	free(app);
	return 0;
}



