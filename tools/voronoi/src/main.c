#include <stdio.h>
#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/camera.h"
#include "renderer/gui.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

Scene* cena;
Shader *shdr;
Camera c;
BoundingBox bbox;
Light l;
Triangles *t;

int menux, menuy;
char *cenasComboBox[] = {"Ladrilho", "Dist", "Dist2 - Dist 1", "Dist2*Dist1", "1 - Color"};
int comboboxState = 0;
int currScene = 0;
int prevScene = 0;


typedef struct {
    float x, y;
    float r, g, b;
}VorSite;


float squarePointDist2Df(float x1, float y1, float x2, float y2) {
    return sqrtf((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

float wrapDist(float x1, float x2) {
  float d = fabs(x2 - x1);
  return d < 1.0f - d ? d : 1.0f - d; //min(d, 1.0f - d);
}

float pointDist2Df(float x1, float y1, float x2, float y2) {
    float wrapx = wrapDist(x1, x2);
    float wrapy = wrapDist(y1, y2);
    return sqrtf(wrapx*wrapx + wrapy*wrapy);
}

float* voronoiTexture(VorSite *point, int npoints, int width, int height) {
    float *data = malloc(sizeof(float)*width*height*3);
    
    for(int j = 0; j < height; j++) {
        for(int i = 0; i < width; i++) {
            int minDistIndex = 0;
            float x = (float)i/(float)width;
            float y = (float)j/(float)height;
            float minDist = squarePointDist2Df(x, y, point[0].x, point[0].y);
            float minDist2 = squarePointDist2Df(x, y, point[1].x, point[1].y);
            for(int k = 1; k < npoints; k++) {
                float dist = squarePointDist2Df(x, y, point[k].x, point[k].y);
                if(dist < minDist) {
                    minDist2 = minDist;
                    minDist = dist;
                    minDistIndex = k;
                } else if(dist < minDist2) {
                    minDist2 = dist;
                }
            }
            //pintar esse pixel da cor da célula k!
            int index = i + j*width;
            data[3*index] = point[minDistIndex].r;
            data[3*index + 1] = point[minDistIndex].g;
            data[3*index + 2] = point[minDistIndex].b;

            if(fabs(minDist2 - minDist) < 0.005) {
                data[3*index] = 0.0f;
                data[3*index + 1] = 0.0f;
                data[3*index + 2] = 0.0f;

            }

        }
    }
    
    //pinta os centros de preto
    for(int k = 0; k < npoints; k++) {
        int i = point[k].x*width, j = point[k].y*height;
        int index = i + width*j;
        data[3*index] = 0.0;
        data[3*index + 1] = 0.0;
        data[3*index + 2] = 0.0;
        
        if((i == width - 1) || (j == height - 1))
            continue;

        index = i + 1 + width*j;
        data[3*index] = 0.0;
        data[3*index + 1] = 0.0;
        data[3*index + 2] = 0.0;
        
        index = i + width*(j + 1);
        data[3*index] = 0.0;
        data[3*index + 1] = 0.0;
        data[3*index + 2] = 0.0;

        if(i == 0 || j == 0)
            continue;
        
        index = i - 1  + width*j;
        data[3*index] = 0.0;
        data[3*index + 1] = 0.0;
        data[3*index + 2] = 0.0;
        
        index = i + width*(j + 1);
        data[3*index] = 0.0;
        data[3*index + 1] = 0.0;
        data[3*index + 2] = 0.0;

        index = i + width*(j - 1);
        data[3*index] = 0.0;
        data[3*index + 1] = 0.0;
        data[3*index + 2] = 0.0;

    }
    return data;
}

float* voronoiTexture2(VorSite *point, int npoints, int width, int height) {
    float *data = malloc(sizeof(float)*width*height*3);
    float distBuffer[width*height];
    int pointBuffer[width*height]; //armazena, pra cada pixel, o ponto mais proximo
    float mindist = 999, maxdist = 0;
    for(int j = 0; j < height; j++) {
        for(int i = 0; i < width; i++) {
            float x = (float)i/(float)width;
            float y = (float)j/(float)height;
            float distance = pointDist2Df(x, y, point[0].x, point[0].y);
            pointBuffer[i + j*width] = 0;
            for(int k = 1; k < npoints; k++) {
                float dist = pointDist2Df(x, y, point[k].x, point[k].y);
                if(dist < distance) {
                    distance = dist;
                    pointBuffer[i + j*width] = k;
                }
                 
            }
            distBuffer[i + j*width] = distance;
            if(distance < mindist)
                mindist = distance;
            if(distance > maxdist)
                maxdist = distance;
        }
    }
    for(int j = 0; j < height; j++)
        for(int i = 0; i < width; i++) {
            int index = i + j*width;
            float color = (distBuffer[index] - mindist)/(maxdist - mindist);
            data[3*index] = color;
            data[3*index + 1] = color;
            data[3*index + 2] = color;
        }
    return data;
}

float* voronoiTexture3(VorSite *point, int npoints, int width, int height) {
    float *data = malloc(sizeof(float)*width*height*3);
    float distBuffer[width*height];
    float distBuffer2[width*height];
    int pointBuffer[width*height]; //armazena, pra cada pixel, o ponto mais proximo
    float mindist = 999, maxdist = 0;
    for(int j = 0; j < height; j++) {
        for(int i = 0; i < width; i++) {
            float x = (float)i/(float)width;
            float y = (float)j/(float)height;
            float distance = pointDist2Df(x, y, point[0].x, point[0].y);
            float distance2 = pointDist2Df(x, y, point[1].x, point[1].y);
            pointBuffer[i + j*width] = 0;
            for(int k = 1; k < npoints; k++) {
                float dist = pointDist2Df(x, y, point[k].x, point[k].y);
                if(dist < distance) {
                    distance2 = distance;
                    distance = dist;
                    pointBuffer[i + j*width] = k;
                } else if(dist < distance2) {
                    distance2 = dist;
                }
                 
            }
            distBuffer[i + j*width] = distance2 - distance;
            distBuffer2[i + j*width] = distance2;
        }
    }

    for(int i = 0; i < width*height; i++) {
        if(mindist > distBuffer[i])
            mindist = distBuffer[i];
        if(maxdist < distBuffer[i])
            maxdist = distBuffer[i];
    }

    for(int j = 0; j < height; j++)
        for(int i = 0; i < width; i++) {
            int index = i + j*width;
            float color = (distBuffer[index] - mindist)/(maxdist - mindist);
            data[3*index] = color;
            data[3*index + 1] = color;
            data[3*index + 2] = color;
        }
    return data;
}


float* voronoiTexture4(VorSite *point, int npoints, int width, int height) {
    float *data = malloc(sizeof(float)*width*height*3);
    float distBuffer[width*height];
    float distBuffer2[width*height];
    int pointBuffer[width*height]; //armazena, pra cada pixel, o ponto mais proximo
    float mindist = 999, maxdist = 0;
    for(int j = 0; j < height; j++) {
        for(int i = 0; i < width; i++) {
            float x = (float)i/(float)width;
            float y = (float)j/(float)height;
            float distance = pointDist2Df(x, y, point[0].x, point[0].y);
            float distance2 = pointDist2Df(x, y, point[1].x, point[1].y);
            pointBuffer[i + j*width] = 0;
            for(int k = 1; k < npoints; k++) {
                float dist = pointDist2Df(x, y, point[k].x, point[k].y);
                if(dist < distance) {
                    distance2 = distance;
                    distance = dist;
                    pointBuffer[i + j*width] = k;
                } else if(dist < distance2) {
                    distance2 = dist;
                }
                 
            }
            distBuffer[i + j*width] = distance2*distance;
            distBuffer2[i + j*width] = distance2;
        }
    }

    for(int i = 0; i < width*height; i++) {
        if(mindist > distBuffer[i])
            mindist = distBuffer[i];
        if(maxdist < distBuffer[i])
            maxdist = distBuffer[i];
    }

    for(int j = 0; j < height; j++)
        for(int i = 0; i < width; i++) {
            int index = i + j*width;
            float color = (distBuffer[index] - mindist)/(maxdist - mindist);
            data[3*index] = color;
            data[3*index + 1] = color;
            data[3*index + 2] = color;
        }
    return data;
}

float* voronoiTexture5(VorSite *point, int npoints, int width, int height) {
    float *data = malloc(sizeof(float)*width*height*3);
    float distBuffer[width*height];
    int pointBuffer[width*height]; //armazena, pra cada pixel, o ponto mais proximo
    float mindist = 999, maxdist = 0;
    for(int j = 0; j < height; j++) {
        for(int i = 0; i < width; i++) {
            float x = (float)i/(float)width;
            float y = (float)j/(float)height;
            float distance = pointDist2Df(x, y, point[0].x, point[0].y);
            pointBuffer[i + j*width] = 0;
            for(int k = 1; k < npoints; k++) {
                float dist = pointDist2Df(x, y, point[k].x, point[k].y);
                if(dist < distance) {
                    distance = dist;
                    pointBuffer[i + j*width] = k;
                }
                 
            }
            distBuffer[i + j*width] = distance;
            if(distance < mindist)
                mindist = distance;
            if(distance > maxdist)
                maxdist = distance;
        }
    }
    for(int j = 0; j < height; j++)
        for(int i = 0; i < width; i++) {
            int index = i + j*width;
            float color = (distBuffer[index] - mindist)/(maxdist - mindist);
            data[3*index] = 1.0f - color;
            data[3*index + 1] = 1.0f - color;
            data[3*index + 2] = 1.0f - color;
        }
    return data;
}


Material* generateMaterial(int voronoiType) {
    float colortable[9][3] = { {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0},
                              {1.0, 1.0, 0.0}, {1.0, 0.0, 1.0}, {1.0, 0.5, 0.5},
                              {0.5, 0.5, 0.5}, {0.5, 0.0, 0.0}, {1.0, 0.5, 0.0}};
    
    int numpoints = 32;
    VorSite points[numpoints];
    for(int i = 0; i < numpoints; i++) {
        points[i].x = (rand()%100)/99.0; points[i].y = (rand()%100)/99.0;
        int color = rand()%9;
        points[i].r = colortable[color][0]; points[i].g = colortable[color][1]; points[i].b = colortable[color][2];
    }

    int texWidth = 256, texHeight = 256; 
    float *cellularTexture;
    if(voronoiType == 0)
        cellularTexture = voronoiTexture(points, numpoints, texWidth, texHeight);
    if(voronoiType == 1)
        cellularTexture = voronoiTexture2(points, numpoints, texWidth, texHeight);
    if(voronoiType == 2)
        cellularTexture = voronoiTexture3(points, numpoints, texWidth, texHeight);
    if(voronoiType == 3)
        cellularTexture = voronoiTexture4(points, numpoints, texWidth, texHeight);
    if(voronoiType == 4)
        cellularTexture = voronoiTexture5(points, numpoints, texWidth, texHeight);

    Texture *tex = initializeTextureFromMemory(cellularTexture, texWidth, texHeight, GL_TEXTURE_2D, GL_RGB, GL_RGB8, GL_FLOAT);

    free(cellularTexture);
    return texturedMaterialDir(tex);
}

void initializeGame(){
    menux = menuy = 0;
    initializeGUI(800, 600);

    srand(time(0));
    //srand48(time(0));
    cena = initializeScene();

    Mesh *m = initMesh();
    //Triangles *t = addTris(m);
    t = addTris(m);
    //Quad (-1, -1) -> (1, 1)
    float vertices[] = {-1.0, -1.0, 0.0, 1.0, -1.0, 0.0, -1.0, 1.0, 0.0,
                        -1.0, 1.0, 0.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0};
    float texcoords[] = {0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
                         0.0, 1.0, 1.0, 0.0, 1.0, 1.0};
    unsigned int indices[] = {0, 1, 2, 3, 4, 5};
    
    addVertices(t, 6*3, 3, vertices);
    addIndices(t, 6, indices);
    addTexCoords(t, 6*2, 2, 0, texcoords);
    prepareMesh(m);
    addMesh(cena, m);
    
    t->material = generateMaterial(currScene);

    
    initCamera(&c, TRACKBALL);
    
    bbox.pmin[0] = -1.0;
    bbox.pmin[1] = -1.0;
    bbox.pmin[2] = 0.0;
    bbox.pmax[0] = 1.0;
    bbox.pmax[1] = 1.0;
    bbox.pmax[2] = 0.0;
    camerafit(&c, bbox, 45.0, 800/600, 0.1, 10000.0);

    l.pos[0]= 256.0; l.pos[1] = 10.0; l.pos[2] = 256.0;
    l.color[0] = 1.0;
    l.color[1] = 1.0;
    l.color[2] = 1.0;
    l.color[3] = 1.0;
}

int Update(event* e, double* dt){
    cameraHandleEvent(&c, e);
    setupViewMatrix(&c);
}

int Render(event *e, double* dt){
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vec3 bboxcenter;
    bboxcenter[0] = 0.0;
    bboxcenter[1] = 0.0;
    bboxcenter[2] = 0.0;
    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    fpMultMatrix(c.mvp, c.projection, c.modelview);
    setView(c.modelview);
    setProjection(c.projection);
    
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
                    bindMaterial(tri->material, &l);
                    bindShader(tri->material->shdr);
                    drawIndexedVAO(tri->vaoId, tri->indicesCount, GL_TRIANGLES);
                }
            }
        }
    }

    int newScene = currScene;
    rect r3, r4;
    beginGUI(e);
        r3.x = 20; r3.y = 580;
        doLabel(&r3, "Cena");
        r4.x = 80; r4.y = 575;
        doComboBox(5, &r4, 5, cenasComboBox, &newScene, &comboboxState);
    endGUI();
    if(newScene != currScene) {
        prevScene = currScene;
        currScene = newScene;
        t->material = generateMaterial(currScene);
    }

    glFlush();
}


int main(){
	setVideoMode(800, 600, 0);
	warpmouse(0);
	setWindowTitle("Voronoi");
	initializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop();

	closeVideo();
	return 0;
}

