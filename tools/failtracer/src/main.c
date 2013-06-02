#include <stdio.h>
#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/glime.h"
#include "renderer/camera.h"
#include "util/colladaloader.h"

Shader *shdr;
Camera c;
BoundingBox bbox;

Scene *cena;
float time;
float t;

void initializeGame(){
    initCamera(&c, TRACKBALL);

    char *vertshader = readTextFile("data/shaders/vertshader.vert");
    char *fragshader = readTextFile("data/shaders/fragshader.frag");
    shdr = initializeShader(vertshader, fragshader); 
	time = 0;

    glClearColor(0.5, 0.5, 0.5, 1.0);
}

void drawFullscreenQuad() {
    static Mesh *m = NULL;
    if(!m) {
        printf("inicializando quad\n");
        m = initMesh();
//        Triangles *t = addTris(m);

        float *vertices = malloc(sizeof(float)*12);
        vertices[0] = -1.0; vertices[1] = -1.0; vertices[2] = 0.0;
        vertices[3] = 1.0; vertices[4] = -1.0; vertices[5] = 0.0;
        vertices[6] = 1.0; vertices[7] = 1.0; vertices[8] = 0.0;
        vertices[9] = -1.0; vertices[10] = 1.0; vertices[11] = 0.0;

        float *texcoords = malloc(sizeof(float)*8);
        texcoords[0] = 0.0; texcoords[1] = 0.0;
        texcoords[2] = 1.0; texcoords[3] = 0.0;
        texcoords[4] = 1.0; texcoords[5] = 1.0;
        texcoords[6] = 0.0; texcoords[7] = 1.0;

        unsigned int *indices = malloc(sizeof(unsigned int)*6);
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 0;
        indices[4] = 2;
        indices[5] = 3;

        addVertices(m, 12, 3, vertices);
        addIndices(m, 6, indices);
        addTexCoords(m, 8, 2, 0, texcoords);
        prepareMesh(m);   

    }
    
   setShaderConstant1f(shdr, "time", time);
    bindShader(shdr);

    drawIndexedVAO(m->vaoId, m->indicesCount, GL_TRIANGLES);
}

int Render(event *e, double *dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	time += *dt;
	printf("%f cos %f \n ", time, cos(time/1.0));
    drawFullscreenQuad();

    glFlush();
}

int Update(event* e, double *dt){

	//time += *dt;

}

int main(){
	setVideoMode(800, 600, 0);
	warpmouse(  0);
	setWindowTitle("Ray of Fail");
	initializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop();

	closeVideo();
	return 0;
}



