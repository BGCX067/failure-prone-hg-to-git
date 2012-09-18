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
#include "util/image.h"
#include <stdlib.h>

#define RESX 1920.0
#define RESY 1080.0
#define CELL 7

Scene* cena;
Shader *shdr;
Camera c;
BoundingBox bbox;
Light l;

Mesh* tMesh;
Triangles* tTris; 	

int menux, menuy;
MatrixData* data[34];

float cellsize = 0.8;

MatrixData* readMatrixFromFile(char* filename){

	MatrixData* m = malloc(sizeof(MatrixData));
	m->sizex = 24;
	m->sizey = 24;

	m->data = malloc(sizeof(float)*(m->sizex)*(m->sizey));

	FILE* fp = fopen(filename, "rt");
	if (!fp)
		printf("File not found: %s \n", filename);
	char line[160];


	int i = 0;
	 while(fgets(line, 160, fp) != NULL){
	//	printf("%s \n", line);
		 sscanf (line, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &m->data[0+i*24], &m->data[1+i*24], &m->data[2+i*24], &m->data[3+i*24], &m->data[4+i*24], &m->data[5+i*24], &m->data[6+i*24], &m->data[7+i*24], &m->data[8+i*24], &m->data[9+i*24], &m->data[10+i*24], &m->data[11+i*24], &m->data[12+i*24], &m->data[13+i*24], &m->data[14+i*24], &m->data[15+i*24], &m->data[16+i*24], &m->data[17+i*24], &m->data[18+i*24], &m->data[19+i*24], &m->data[20+i*24], &m->data[21+i*24], &m->data[22+i*24], &m->data[23+i*24]);

//printf ("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f \n", m->data[0+i*24], m->data[1+i*24], m->data[2+i*24], m->data[3+i*24], m->data[4+i*24], m->data[5+i*24], m->data[6+i*24], m->data[7+i*24], m->data[8+i*24], m->data[9+i*24], m->data[10+i*24], m->data[11+i*24], m->data[12+i*24], m->data[13+i*24], m->data[14+i*24], m->data[15+i*24], m->data[16+i*24], m->data[17+i*24], m->data[18+i*24], m->data[19+i*24], m->data[20+i*24], m->data[21+i*24], m->data[22+i*24], m->data[23+i*24]);


		i++;
   	}

	fclose(fp);

	return m;
}
void initializeGame(){

    menux = menuy = 0;
    tMesh = initMesh();
    tTris = addTris(tMesh);
    initializeGUI(RESX, RESY);

    cena = initializeScene();
    
    initCamera(&c, TRACKBALL);
    
    bbox.pmin[0] = 0.0;
    bbox.pmin[1] = 0.0;
    bbox.pmin[2] = 0.0;
    bbox.pmax[0] = 512.0;
    bbox.pmax[1] = 0.0;
    bbox.pmax[2] = 512.0;
    camerafit(&c, bbox, 45.0, RESX/RESY, 0.1, 10000.0);

	for (int i = 1; i <= 34; i++){
		char buffer[10];
		sprintf(buffer, "adj%d.txt", i );
		//printf("filename %s \n", buffer);
		data[i-1] = readMatrixFromFile(buffer);
	}


GLuint dims[2];
glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &dims[0]);
printf("Max viewport x %d y %d \n ", dims[0], dims[1]);

}

int Update(event* e, double* dt){
    cameraHandleEvent(&c, e);
    setupViewMatrix(&c);

}



int Render(event *e, double* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 bboxcenter;
    bboxcenter[0] = 256.0;
    bboxcenter[1] = 0.0;
    bboxcenter[2] = 256.0;
    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    fpMultMatrix(c.mvp, c.projection, c.modelview);
    setView(c.modelview);
    setProjection(c.projection);
    

    rect r,  r2, r3, r4, r5, r6, r7, r8, r9;
    beginGUI(e);
	rect rs;
	rs.y = 1000;
	rs.x = 10;
	doHorizontalSlider(1, &rs, &cellsize);

	int size = cellsize*10;
	int x = 10;
	int y = 780;
	int step = data[0]->sizex*size+24+8;
	for (int i = 0; i < 34; i++){
		if (x + data[i]->sizex*size+24+8 > RESX){
			x = 10;
			y = y - step;
		}
		doMatrix(data[i], x, y, size);
		x += step;

	}
    endGUI();

    glFlush();
}


int main(){
	setVideoMode(RESX, RESY, 0);
	warpmouse(0);
	setWindowTitle("Matrix Plots");
	initializeRenderer(RESX, RESY, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop();

	closeVideo();
	return 0;
}



