#include <stdio.h>
#include <stdlib.h>
#include "glapp.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/camera.h"
#include "renderer/gui.h"
#include "util/utlist.h"
#include "renderer/material.h"

Scene* cena;
Renderer *mainrenderer;
Camera c;
Light l; 

//Node *shortBoxNode, *shortestBoxNode;

void initializeGame(){
    //CamInit(&c, GetScreenW(), GetScreenH(), FPS, PERSPECTIVE); 
    CamInit(&c, GetScreenW(), GetScreenH(), TRACKBALL, PERSPECTIVE); 

    SetProjection(c.mprojection);

    Setvf(c.pos, 0.0, 0.0, 5.0);

    cena = InitializeScene();
    l.pos[0]= 0.0; l.pos[1] = 0.92; l.pos[2] = 0.0;
    l.color[0] = 1.0; l.color[1] = 1.0; l.color[2] = 1.0; l.color[3] = 1.0;

    vec3 halveRedAmb = { 0.082, 0.0, 0.0 };
    vec3 halveRedDiff = { 0.35, 0.0, 0.0 };
    vec3 halveRedSpec = { 0.125, 0.125, 0.125 };
    float halveRedShininess = 12.5;

    vec3 darkGreenAmb = { 0.0, 0.05, 0.0 };
    vec3 darkGreenDiff = { 0.0, 0.2, 0.0 };
    vec3 darkGreenSpec = { 0.125, 0.125, 0.125 };
    float darkGreenShininess = 12.5;

    vec3 khakiAmb = {0.125, 0.1, 0.072};
    vec3 khakiDiff = {0.5, 0.41, 0.275};
    vec3 khakiSpec = {0.125, 0.125, 0.125};
    float khakiShininess = 12.5;

    Mesh *leftWall = CreateBox(0.05, 1.95, 1.95);
    Node *leftWallNode = AddMesh(cena, leftWall);
    leftWallNode->material = PhongMaterial(halveRedAmb, halveRedDiff, halveRedSpec, 12.5, l.pos, l.color);
    Translatef(leftWallNode->transform, -0.975, 0.0, 0.025);

    Mesh *rightWall = CreateBox(0.05, 1.95, 1.95);
    Node *rightWallNode = AddMesh(cena, rightWall);
    rightWallNode->material = PhongMaterial(darkGreenAmb, darkGreenDiff, darkGreenSpec, darkGreenShininess, l.pos, l.color); 
    Translatef(rightWallNode->transform, 0.975, 0.0, 0.025);

    Mesh *backWall = CreateBox(2.0, 1.95, 0.05);
    Node *backWallNode = AddMesh(cena, backWall);
    backWallNode->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);
    Translatef(backWallNode->transform, 0.0, 0.0, -0.975);


    Mesh *floor = CreateBox(2.0, 0.05, 2.0);
    Node *floorNode = AddMesh(cena, floor);
    floorNode->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);
    Translatef(floorNode->transform, 0.0, -1.0, 0.0);

    Mesh *ceiling = CreateBox(2.0, 0.05, 2.0);
    Node *ceilingNode = AddMesh(cena, ceiling);
    ceilingNode->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);
    Translatef(ceilingNode->transform, 0.0, 1.0, 0.0);
    
    Mesh *tallBox = CreateBox(0.7, 1.16, 0.7);
    Node *tallBoxNode = AddMesh(cena, tallBox);
    tallBoxNode->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);
    Translatef(tallBoxNode->transform, -0.4, -(1.0 - 0.58), -0.5);
    Rotatef(tallBoxNode->transform, 3.14159265358979323846/6.0, 0.0, 1.0, 0.0);

    Mesh *shortBox = CreateBox(0.7, 0.56, 0.7);
    Node *shortBoxNode = AddMesh(cena, shortBox);
    //shortBoxNode = AddMesh(cena, shortBox);
    shortBoxNode->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color); 
    Translatef(shortBoxNode->transform, 0.4, -(1.0 - 0.28), 0.3);
    Rotatef(shortBoxNode->transform, -3.14159/12.0, 0.0, 1.0, 0.0);

    /*Mesh *shortestBox = CreateBox(0.35, 0.28, 0.35);
    shortestBoxNode = AddMesh(cena, shortestBox);
    shortestBoxNode->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color); 
    Translatef(shortestBoxNode->transform, 0.4, -(1.0 - 0.70), 0.3);*/
    
    //glPolygonMode(GL_BACK, GL_LINE);
}

int Render(event *e, double* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    DrawScene(cena);

    glFlush();
    return 1;
}

//TODO automatizar o uso da camera
int Update(event* e, double *dt){
    c.update(&c, e, dt);
    SetView(c.mview);


/*    if(e->keys[KEY_z]) { 
        AddChildNode(shortBoxNode, shortestBoxNode);
        Identity(shortestBoxNode->transform);
        Translatef(shortestBoxNode->transform, 0.0, 0.42, 0.0);
    } else if (e->keys[KEY_x]) {
        AddChildNode(cena->root, shortestBoxNode);
        Identity(shortestBoxNode->transform);
        Translatef(shortestBoxNode->transform, 0.4, -(1.0 - 0.70), 0.3);
    } else if (e->keys[KEY_c]) {
        Translatef(shortBoxNode->transform, -0.05, 0.0, 0.0);
    } else if (e->keys[KEY_v]) {
        Translatef(shortBoxNode->transform, +0.05, 0.0, 0.0);
    }*/


    return 1;
}

int main(){
	SetVideoMode(800, 600, 0);
	WarpMouse(0);
	SetWindowTitle("Cornell Box");
	//initializeRenderer(1024, 768, 0.1, 10000.0, 45.0);
	mainrenderer = InitializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop();
	CloseVideo();
	return 0;
}

