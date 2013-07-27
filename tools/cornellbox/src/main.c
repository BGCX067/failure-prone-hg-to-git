#include <stdio.h>
#include <stdlib.h>
#include "glapp.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/camera.h"
#include "renderer/gui.h"
#include "util/utlist.h"

Scene* cena;
Renderer *mainrenderer;
Camera c;
Light l; 

Mesh* createBox(float x, float y, float z);

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

    Mesh *leftWall = createBox(0.05, 1.95, 1.95);
    Translatef(leftWall->transform, -0.975, 0.0, 0.025);
    printf("antes de PhongMaterial halveRed\n");
    leftWall->material = PhongMaterial(halveRedAmb, halveRedDiff, halveRedSpec, halveRedShininess, l.pos, l.color);
    AddMesh(cena, leftWall);

    Mesh *rightWall = createBox(0.05, 1.95, 1.95);
    Translatef(rightWall->transform, 0.975, 0.0, 0.025);
    printf("antes de PhongMaterial darkGreen\n");
    rightWall->material = PhongMaterial(darkGreenAmb, darkGreenDiff, darkGreenSpec, darkGreenShininess, l.pos, l.color); 
    AddMesh(cena, rightWall);

    Mesh *backWall = createBox(2.0, 1.95, 0.05);
    Translatef(backWall->transform, 0.0, 0.0, -0.975);
    printf("antes de PhongMaterial khaki\n");
    backWall->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color); 
    AddMesh(cena, backWall);

    Mesh *floor = createBox(2.0, 0.05, 2.0);
    Translatef(floor->transform, 0.0, -1.0, 0.0);
    printf("antes de PhongMaterial khaki\n");
    floor->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color); 
    AddMesh(cena, floor);

    Mesh *ceiling = createBox(2.0, 0.05, 2.0);
    printf("antes de PhongMaterial khaki\n");
    ceiling->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color); 
    Translatef(ceiling->transform, 0.0, 1.0, 0.0);
    AddMesh(cena, ceiling);
    
    Mesh *tallBox = createBox(0.7, 1.16, 0.7);
    printf("antes de PhongMaterial khaki\n");
    tallBox->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color); 
    Translatef(tallBox->transform, -0.4, -(1.0 - 0.58), -0.5);
    Rotatef(tallBox->transform, 3.14159265358979323846/6.0, 0.0, 1.0, 0.0);
    AddMesh(cena, tallBox);

    Mesh *shortBox = createBox(0.7, 0.56, 0.7);
    printf("antes de PhongMaterial khaki\n");
    shortBox->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color); 
    Translatef(shortBox->transform, 0.4, -(1.0 - 0.28), 0.3);
    Rotatef(shortBox->transform, -3.14159/12.0, 0.0, 1.0, 0.0);
    AddMesh(cena, shortBox);
    
    
    //glPolygonMode(GL_BACK, GL_LINE);
}

int Render(event *e, double* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Mesh *m;
    DL_FOREACH(cena->meshList, m) {
        SetModel(m->transform);
        //BindMaterial(m->material, &l);
        BindShader(m->material);
        DrawIndexedVAO(m->vaoId, m->indicesCount, GL_TRIANGLES);
    }

    glFlush();
    return 1;
}

//TODO automatizar o uso da camera
int Update(event* e, double *dt){
    c.update(&c, e, dt);
    SetView(c.mview);
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

Mesh* createBox(float w, float h, float l) {
    Mesh *m = InitMesh();
    
    float x = w*0.5;
    float y = h*0.5;
    float z = l*0.5;

    float vertices[] = { -x, -y, z, x, -y, z, -x, y, z,  //Front1
                         -x, y, z, x, -y, z, x, y, z, //Front2
                         -x, -y, -z, -x, y, -z, x, -y, -z, //Back1
                         -x, y, -z, x, y, -z, x, -y, -z, //Back2
                         x, -y, z, x, -y, -z, x, y, z, //Right1
                         x, y, z, x, -y, -z, x, y, -z, //Right2
                         -x, -y, z, -x, y, z, -x, -y, -z, //Left1
                         -x, y, z, -x, y, -z, -x, -y, -z, //Left2
                         -x, y, z, x, y, z, -x, y, -z, //Top1
                         -x, y, -z, x, y, z, x, y, -z, //Top2
                         -x, -y, z, -x, -y, -z, x, -y, z, //Bot1
                         -x, -y, -z, x, -y, -z, x, -y, z, //Bot2
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
                       0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, //Bot2
                       0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, //Bot2
                      };
    float colors[36*3];
    for(int i = 0; i < 36; i++) {
        colors[3*i] = 1.0;
        colors[3*i + 1] = 0.82;
        colors[3*i + 2] = 0.55;
    }
    AddVertices(m, 36*3, 3, vertices);
    AddNormals(m, 36*3, 3, normals);
    AddColors(m, 36*3, colors);
    AddIndices(m, 36, indices);
    PrepareMesh(m);

    return m;
}
