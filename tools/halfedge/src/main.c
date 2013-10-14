#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glapp.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/camera.h"
#include "renderer/gui.h"
#include "util/utlist.h"
#include "util/ezxml.h"
#include "renderer/material.h"
#include "renderer/glime.h"

Scene* cena;
Camera c;
Shader *material;

//Coordinate axis
Shader *colorshdr;
Batch *coordaxis;

int drawCenterOfRoation = 1;
void createCoordAxis();


//GUI
char cposstr[64];
char cpivotstr[64];
char mv0str[64], mv1str[64], mv2str[64];
char lookatstr[64];
char fovystr[16];
char zfarstr[16];
char znearstr[16];

void initializeGame(){
    InitializeGUI(GetScreenW(), GetScreenH());
    //CamInit(&c, GetScreenW(), GetScreenH(), FPS, PERSPECTIVE); 
    CamInit(&c, GetScreenW(), GetScreenH(), TRACKBALL, PERSPECTIVE); 
    SetZfar(&c, 1000.0);
    SetProjection(c.mprojection);
    //Setvf(c.pos, 0.0, 0.0, 3.0);
    c.zoom = 3.0;

    createCoordAxis();

    Light l; 
    l.pos[0]= 2.0; l.pos[1] = 1.92; l.pos[2] = 2.0;
    l.color[0] = 1.0; l.color[1] = 1.0; l.color[2] = 1.0; l.color[3] = 1.0;

    vec3 khakiAmb = {0.125, 0.1, 0.072};
    vec3 khakiDiff = {0.5, 0.41, 0.275};
    vec3 khakiSpec = {0.125, 0.125, 0.125};
    float khakiShininess = 12.5;
    material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color); 

    cena = InitializeScene();
    Mesh *shortBox = CreateBox(1.0, 1.0, 1.0);
    Node *shortBoxNode = AddMesh(cena, shortBox);
    shortBoxNode->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);
}


int gamebutton = 0;
float menux1 = 0.0, menux2 = 0.0;
int Render(event *e, double* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    DrawScene(cena);

    //Draw the coordinate axis
    glDisable(GL_DEPTH_TEST);
    //BindShader(colorshdr);
    //Draw(coordaxis);

    if(drawCenterOfRoation) {
        Translatef(c.mview, c.pivot[0], c.pivot[1], c.pivot[2]);
        SetView(c.mview);
        BindShader(colorshdr);
        Draw(coordaxis);
    }
    glEnable(GL_DEPTH_TEST);
    
    
    Rect r1 = { 10.0, 575.0, 0.0, 0.0};
    BeginGUI(e);
        DoToggleButton(1, &r1, "Camera", &gamebutton);
      
        if(gamebutton == 1 ) {
            BeginMenu(6, 10, 223, 370, 350, &menux1, &menux2, "Game", NULL);
            //posição
            DoLabel(20, 550, cposstr);
            //pivot
            DoLabel(20, 530, cpivotstr);
            //lookat vec
            DoLabel(20, 510, lookatstr);
            //c.mview
            DoLabel(20, 490, mv0str);
            DoLabel(20, 470, mv1str);
            DoLabel(20, 450, mv2str);
            //
            DoLabel(20, 430, fovystr);
            DoLabel(20, 410, znearstr);
            DoLabel(20, 390, zfarstr);
            EndMenu(6, 10, 223, 370, 350, &menux1, &menux2);        
        }
    EndGUI();

    glFlush();
    return 1;
}

//TODO automatizar o uso da camera
int Update(event* e, double *dt){
    c.update(&c, e, dt);

    /**
     *  Extrair a posição da matriz VIEW da camera
     **/
    mat4 m;
    ToMatrixq(c.orientation, m);
    //A Intersa não é realmente necessária, como a matriz é
    //ortonormal, a transposta já seria igual à inversa.
    Inverse(m, m);
    float cpos[4] = {-c.mview[12], -c.mview[13], -c.mview[14], 1.0};
    float newpos[4];
    Multmv(m, cpos, newpos);
    /**
     *  Calculando lookat como pivot - pos.
     *  Esse vetor pode ser obtido da matriz VIEW como mostrado abaixo
     **/
    vec3 lookat = {c.pivot[0] - newpos[0], c.pivot[1] - newpos[1], c.pivot[2] - newpos[2] };
    Normalizev(lookat);
    c.pos[0] = newpos[0];
    c.pos[1] = newpos[1];
    c.pos[2] = newpos[2];
    
    
    /**
     *  Posição da câmera calculada a partir da matriz VIEW.
     **/
    sprintf(cposstr, "Pos: (%f, %f, %f)\0", c.pos[0], c.pos[1], c.pos[2]);
    /**
     *  Centro de rotação da câmera
     **/
    sprintf(cpivotstr, "Pivot: (%f, %f, %f)\0", c.pivot[0], c.pivot[1], c.pivot[2]);
    /**
     *  Lookat da camera, exatamente ao vetor -Z que pode ser obtido (veja abaixo)
     *  da matriz VIEW, apenas calculado utilizando a posição da camera e o pivot.
     **/
    sprintf(lookatstr, "Lookat: (%f, %f, %f)\0", lookat[0], lookat[1], lookat[2]);
    /**
     *  MV[0,1,2] guarda as linhas da matriz VIEW da camera
     *  0 -> Vetor X da camera
     *  1 -> Vetor Y da camera
     *  2 -> Vetor Z da camera
     *
     *  VIEW^-1 = VIEW^t
     *  Pensando na camera como um objeto, sua matriz seria VIEW^t,
     *  então podemos pegar as linhas da matrix VIEW ao invés das
     *  colunas.
     **/
    sprintf(mv0str, "MV0: (%f, %f, %f)\0", c.mview[0], c.mview[4], c.mview[8]);
    sprintf(mv1str, "MV1: (%f, %f, %f)\0", c.mview[1], c.mview[5], c.mview[9]);
    sprintf(mv2str, "MV2: (%f, %f, %f)\0", c.mview[2], c.mview[6], c.mview[10]);
    /**
     *  Informações de projeção só pra constar
     **/
    sprintf(fovystr, "fovy: %f\0", c.fovy);
    sprintf(znearstr, "znear: %f\0", c.znear);
    sprintf(zfarstr, "zfar: %f\0", c.zfar);

    SetView(c.mview);


    if(e->type & MOUSE_BUTTON_PRESS) {
       printf("wheel: %d\n", e->wheel); 
    }

    return 1;
}

int main(){
	SetVideoMode(800, 600, 0);
	WarpMouse(0);
	SetWindowTitle("Camera");
	InitializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop();
	CloseVideo();
	return 0;
}

//Creates a small coordinate axis to help testing some stuff
void createCoordAxis() {
    char* vertsrc = ReadTextFile("data/shaders/color.vert");
    char* fragsrc = ReadTextFile("data/shaders/color.frag"); 
    colorshdr = InitializeShader(NULL, vertsrc, fragsrc);
    free(vertsrc);
    free(fragsrc);
    coordaxis = InitializeBatch();
    Begin(coordaxis, GL_LINES, 18, 0);
        Color4f(coordaxis, 1.0, 0.0, 0.0, 0.0);
        Vertex3f(coordaxis, 0.0, 0.0, 0.0);
        Color4f(coordaxis, 1.0, 0.0, 0.0, 0.0);
        Vertex3f(coordaxis, 1.0, 0.0, 0.0);
        Color4f(coordaxis, 1.0, 0.0, 0.0, 0.0);
        Vertex3f(coordaxis, 0.85, 0.15, 0.0);
        Color4f(coordaxis, 1.0, 0.0, 0.0, 0.0);
        Vertex3f(coordaxis, 1.0, 0.0, 0.0);
        Color4f(coordaxis, 1.0, 0.0, 0.0, 0.0);
        Vertex3f(coordaxis, 0.85, -0.15, 0.0);
        Color4f(coordaxis, 1.0, 0.0, 0.0, 0.0);
        Vertex3f(coordaxis,1.0, 0.0, 0.0);


        Color4f(coordaxis, 0.0, 1.0, 0.0, 0.0);
        Vertex3f(coordaxis, 0.0, 0.0, 0.0);
        Color4f(coordaxis, 0.0, 1.0, 0.0, 0.0);
        Vertex3f(coordaxis, 0.0, 1.0, 0.0);
        Color4f(coordaxis, 0.0, 1.0, 0.0, 0.0);
        Vertex3f(coordaxis, 0.15, 0.85, 0.0);
        Color4f(coordaxis, 0.0, 1.0, 0.0, 0.0);
        Vertex3f(coordaxis, 0.0, 1.0, 0.0);
        Color4f(coordaxis, 0.0, 1.0, 0.0, 0.0);
        Vertex3f(coordaxis, -0.15, 0.85, 0.0);
        Color4f(coordaxis, 0.0, 1.0, 0.0, 0.0);
        Vertex3f(coordaxis, 0.0, 1.0, 0.0);

        Color4f(coordaxis, 0.0, 0.0, 1.0, 0.0);
        Vertex3f(coordaxis, 0.0, 0.0, 0.0);
        Color4f(coordaxis, 0.0, 0.0, 1.0, 0.0);
        Vertex3f(coordaxis, 0.0, 0.0, 1.0);
        Color4f(coordaxis, 0.0, 0.0, 1.0, 0.0);
        Vertex3f(coordaxis, 0.15, 0.0, 0.85);
        Color4f(coordaxis, 0.0, 0.0, 1.0, 0.0);
        Vertex3f(coordaxis, 0.0, 0.0, 1.0);
        Color4f(coordaxis, 0.0, 0.0, 1.0, 0.0);
        Vertex3f(coordaxis, -0.15, 0.0, 0.85);
        Color4f(coordaxis, 0.0, 0.0, 1.0, 0.0);
        Vertex3f(coordaxis, 0.0, 0.0, 1.0);
    End(coordaxis);   
}
