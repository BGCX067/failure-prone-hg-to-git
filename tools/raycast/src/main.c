#include <stdio.h>
#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/glime.h"
#include "renderer/camera.h"
#include "renderer/gui.h"
#include "util/shadergen.h"
#include "util/colladaloader.h"
#include "util/sdnoise1234.h"
#include "util/image.h"
#include <stdlib.h>

Scene* cena;
Shader *shdr;
Camera c;
BoundingBox bbox;
Light l;

unsigned char* loadVolumeFromFile(const char *filename) {
    int xdim, ydim, zdim; //pegar do nome do arquivo
    xdim = ydim = zdim = 256;

    const int size = xdim*ydim*zdim;
    unsigned char *data = malloc(size*sizeof(unsigned char));

    FILE *fp = fopen(filename, "rb");
    if(fp == NULL) {
        printf("Problema ao ler o arquivo: %s\n", filename);
        exit(0);
    }

    fread(data, sizeof(unsigned char), size, fp);
    
    fclose(fp);
    return data;
}

void initializeGame(){
    initializeGUI(800, 600);

    cena = initializeScene();

    Mesh *m = initMesh();
    Triangles *t = addTris(m);
    float vertices[] = {0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, //Front1
                        0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, //Front2
                        0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, //Back1
                        1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, //Back2
                        1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, //Right1
                        1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, //Right2
                        0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, //Left1
                        0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, //Left2
                        0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, //Top1
                        0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, //Top2
                        0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, //Bot1
                        0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, //Bot2
                        };
    unsigned int indices[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
    float normals[] = {0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, //Front1
                       0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, //Front2
                       0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, //Back1
                       0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, //Back2
                       1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, //Right1
                       1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, //Right2
                       -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, //Left1
                       -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, //Left2
                       0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, //Top1
                       0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, //Top2
                       0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, //Top2
                       0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, //Top2
                      };
    float texcoord[] = { 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, //Front1
                         0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, //Front2
                         0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, //Back1
                         1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, //Back2
                         1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, //Right1
                         1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, //Right2
                         0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, //Left1
                         0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, //Left2
                         0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, //Top1
                         0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, //Top2
                         0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, //Bot1
                         0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, //Bot2
                        };
    addVertices(t, 36*3, 3, vertices);
    addNormals(t, 36*3, 3, normals);
    addIndices(t, 36, indices);
    addTexCoords(t, 36*3, 3, 0, texcoord);
    prepareMesh(m);
    addMesh(cena, m);

    //Ler o volume do arquivo
    //unsigned char *volData = loadVolumeFromFile("data/nucleon_41_41_41.raw");
    unsigned char *volData = loadVolumeFromFile("data/bonsai.raw");
    Texture *tex = initialize3DTexture(volData, 256, 256, 256, GL_LUMINANCE, GL_INTENSITY, GL_UNSIGNED_BYTE);
    //Texture *tex = initialize3DTexture(volData, 41, 41, 41, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE);
    free(volData);
    t->material = volumeMaterial(tex);
    
    initCamera(&c, TRACKBALL);
    
    bbox.pmin[0] = 0.0;
    bbox.pmin[1] = 0.0;
    bbox.pmin[2] = 0.0;
    bbox.pmax[0] = 1.0;
    bbox.pmax[1] = 1.0;
    bbox.pmax[2] = 1.0;
    camerafit(&c, bbox, 45.0, 800/600, 0.1, 10000.0);

    l.pos[0]= 0.0; l.pos[1] = 0.0; l.pos[2] = 4.0;
    l.color[0] = 1.0;
    l.color[1] = 1.0;
    l.color[2] = 1.0;
    l.color[3] = 1.0;
    
    //glPolygonMode(GL_BACK, GL_LINE);
}

int Update(event* e, double* dt){
    cameraHandleEvent(&c, e);
    setupViewMatrix(&c);
}


int Render(event *e, double* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 bboxcenter;
    bboxcenter[0] = 0.5;
    bboxcenter[1] = 0.5;
    bboxcenter[2] = 0.5;
    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    fpMultMatrix(c.mvp, c.projection, c.modelview);
    setView(c.modelview);
    setProjection(c.projection);

    //printf("%f, %f, %f\n", c.pos[0], c.pos[1], c.pos[2]);

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mat4 invModelview;
    fpInverse(invModelview, c.modelview); 

    //Desenhar cena
    if (cena->meshList){
        Mesh* m = NULL;
        for( int i = 0; i < cena->meshList->size; i++){ // para da mesh da cena
            m = fplist_getdata(i, cena->meshList);
            if (m->tris){
                Triangles* tri = NULL;
                for( int k = 0; k < m->tris->size; k++){ //para cada chunk de triangles do mesh
                    tri = fplist_getdata(k, m->tris);
                    setShaderConstant3f(tri->material->shdr, "eyepos", c.pos); 
                    setShaderConstant4x4f(tri->material->shdr, "invModelview", invModelview); 
                    bindMaterial(tri->material, &l);
                    bindShader(tri->material->shdr);
                    drawIndexedVAO(tri->vaoId, tri->indicesCount, GL_TRIANGLES);
                }
            }
        }
    }
    glDisable(GL_BLEND);
    glFlush();
}


int main(){
	setVideoMode(800, 600, 0);
	warpmouse(0);
	setWindowTitle("RAY CAST");
	initializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop();

	closeVideo();
	return 0;
}
