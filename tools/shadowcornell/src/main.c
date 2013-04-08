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
renderer *mainrenderer;
Camera c;
Light l;

Framebuffer *shadowfb;
Shader *shdr;
Shader *firstPassShader;

mat4 lightView;
mat4 lightProj;

//GUI-related
int menux, menuy;
char* salasComboBox[] = {"Sala 1", "Sala 2", "Sala 3", "Sala 4"};
int comboboxState = 0;
int currRoom = 0;
int prevRoom = 0;

Mesh* createBox(float x, float y, float z);
void firstPass();
void secondPass();

void drawFullscreenQuad(Texture *tex, Shader *shdr) {
    static Mesh *m = NULL;
    if(!m) {
        printf("inicializando quad\n");
        m = initMesh();

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
    
    bindSamplerState(tex->state, 0);
    bindTexture(tex, 0);
    bindShader(shdr);
    drawIndexedVAO(m->vaoId, m->indicesCount, GL_TRIANGLES);
}

void initializeGame(){
    menux = menuy = 0;
    //initializeGUI(800, 600);
    initCamera(&c, TRACKBALL);
   
    cena = initializeScene();

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
    fptranslatef(leftWall->transform, -0.975, 0.0, 0.025);
    leftWall->material = colorMaterialShadowPhong(halveRedAmb, halveRedDiff, halveRedSpec, halveRedShininess);
    addMesh(cena, leftWall);

    Mesh *rightWall = createBox(0.05, 1.95, 1.95);
    fptranslatef(rightWall->transform, 0.975, 0.0, 0.025);
    rightWall->material = colorMaterialShadowPhong(darkGreenAmb, darkGreenDiff, darkGreenSpec, darkGreenShininess);
    addMesh(cena, rightWall);

    Mesh *backWall = createBox(2.0, 1.95, 0.05);
    fptranslatef(backWall->transform, 0.0, 0.0, -0.975);
    backWall->material = colorMaterialShadowPhong(khakiAmb, khakiDiff, khakiSpec, khakiShininess);
    addMesh(cena, backWall);

    Mesh *floor = createBox(2.0, 0.05, 2.0);
    fptranslatef(floor->transform, 0.0, -1.0, 0.0);
    floor->material = colorMaterialShadowPhong(khakiAmb, khakiDiff, khakiSpec, khakiShininess);
    addMesh(cena, floor);

    Mesh *ceiling = createBox(2.0, 0.05, 2.0);
    ceiling->material = colorMaterialShadowPhong(khakiAmb, khakiDiff, khakiSpec, khakiShininess);
    fptranslatef(ceiling->transform, 0.0, 1.0, 0.0);
    addMesh(cena, ceiling);
    
    Mesh *tallBox = createBox(0.7, 1.16, 0.7);
    tallBox->material = colorMaterialShadowPhong(khakiAmb, khakiDiff, khakiSpec, khakiShininess);
    fptranslatef(tallBox->transform, -0.4, -(1.0 - 0.58), -0.5);
    fpRotatef(tallBox->transform, 3.14159265358979323846/6.0, 0.0, 1.0, 0.0);
    addMesh(cena, tallBox);

    Mesh *shortBox = createBox(0.7, 0.56, 0.7);
    shortBox->material = colorMaterialShadowPhong(khakiAmb, khakiDiff, khakiSpec, khakiShininess);
    fptranslatef(shortBox->transform, 0.4, -(1.0 - 0.28), 0.3);
    fpRotatef(shortBox->transform, -3.14159265358979323846/12.0, 0.0, 1.0, 0.0);
    addMesh(cena, shortBox);
    
    l.pos[0]= 0.0; l.pos[1] = 0.92; l.pos[2] = 0.0;
    l.color[0] = 1.0;
    l.color[1] = 1.0;
    l.color[2] = 1.0;
    l.color[3] = 1.0;
    camerafit(&c, cena->b, 75.0, 800.0f/600.0f, 1.0, 10.0);
    
    
    shadowfb = initializeFramebuffer(512, 512);
    
    char *vertshader = readTextFile("data/shaders/vertshader.vert");
    char *fragshader = readTextFile("data/shaders/fragshader.frag");
    shdr = initializeShader(vertshader, fragshader);
    
    firstPassShader = initializeShader(readTextFile("data/shaders/shadowfirstpass.vert"), 
                            readTextFile("data/shaders/shadowfirstpass.frag"));
}

float menux1, menux2 = 0;
int gamebutton = 0;
int Render(event *e, double* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    firstPass();

    glFlush();

    secondPass();
    
    //drawFullscreenQuad(shadowfb->tex, shdr);
    //Mesh *m;
    //DL_FOREACH(cena->meshList, m) {
    //    setModel(m->transform);
    //    bindMaterial(m->material, &l);
    //    bindShader(m->material->shdr);
    //    drawIndexedVAO(m->vaoId, m->indicesCount, GL_TRIANGLES);
    //}

    /*int newRoom = prevRoom;
    rect r1,  r2, r3, r4, r5, r6, r7, r8, r9;
    beginGUI(e);
	r1.x = 10; r1.y = 575;
	r2.x = 60; r2.y = 575;
	r3.x = 120; r3.y = 575;
	r4.x = 220; r4.y = 575;
	r5.x = 285; r5.y = 575;

	doToggleButton(1, &r1, "Game", &gamebutton);
	doToggleButton(2, &r2, "Tower", &gamebutton);
	doToggleButton(3, &r3, "Characters", &gamebutton);
	doToggleButton(4, &r4, "Agenda", &gamebutton);
	doToggleButton(5, &r5, "Spells", &gamebutton);

	if (gamebutton == 1 ) {
		beginMenu(6, 10, 223, 250, 350, &menux1, &menux2, "Game", NULL);
			r6.x = 100; r6.y = 540;
        	if(doButton(1, &r6, "Save"))
                printf("Save button pressed\n");
			r2.x = 100; r2.y = 500;
        	if(doButton(2, &r2, "Load"))
                printf("Load button pressed\n");
		endMenu(6, 10, 223, 250, 150, &menux1, &menux2);
	}else if (gamebutton == 2 ) {
        beginMenu(6, 60, 223, 250, 350, &menux1, &menux2, "Tower", NULL);
 			doLabel(80, 543, "Sala");
        	r7.x = 120; r7.y = 540;
        	doComboBox(5, &r7, 4, salasComboBox, &newRoom, &comboboxState);
		endMenu(6, 60, 223, 250, 150, &menux1, &menux2);
	} else if (gamebutton == 3){
		beginMenu(6, 120, 223, 250, 350, &menux1, &menux2, "Characters", NULL);
		endMenu(6, 120, 223, 250, 150, &menux1, &menux2);
	} else if (gamebutton == 4){
		beginMenu(6, 220, 223, 250, 350, &menux1, &menux2, "Agenda", NULL);
		endMenu(6, 220, 223, 250, 150, &menux1, &menux2);
	} else if (gamebutton == 5){
		beginMenu(6, 285, 223, 250, 350, &menux1, &menux2, "Spells", NULL);
		endMenu(6, 285, 223, 250, 150, &menux1, &menux2);
	}
    endGUI();

    if(currRoom != newRoom) {
        prevRoom = currRoom;
        currRoom = newRoom;
        printf("prevRoom: %d\ncurrRoom: %d\n", prevRoom, currRoom);
        prevRoom = currRoom;
    }*/

    glFlush();
    return 1;
}

int Update(event* e, double *dt){
    cameraHandleEvent(&c, e);
    setupViewMatrix(&c);
    vec3 bboxcenter;
    bbcenter(cena->b, bboxcenter);
    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    setView(c.modelview);
    setProjection(c.projection); //TODO isso so precisaria ser calculado/setado 1x
    return 1;
}

int main(){
	setVideoMode(800, 600, 0);
	warpmouse(0);
	setWindowTitle("Cornell Box");
	mainrenderer = initializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop();
	closeVideo();
	return 0;
}


Mesh* createBox(float w, float h, float l) {
    Mesh *m = initMesh();
    
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
    addVertices(m, 36*3, 3, vertices);
    addNormals(m, 36*3, 3, normals);
    addColors(m, 36*3, colors);
    addIndices(m, 36, indices);
    prepareMesh(m);

    return m;
}

void firstPass() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    vec3 lightDir = {0.0f, -1.0f, 0.0f}; 
    vec3 lightUp = {0.0f, 0.0f, -1.0f}; 
    vec3 lightPos = {l.pos[0], l.pos[1], l.pos[2]};
    vec3 center;
    vecAdd(lightPos, lightDir, center);

    bindFramebuffer(shadowfb);
    fpLookAt(lightView, lightPos, lightDir, lightUp);
    fpperspective(lightProj, 100.0f, 512.0f/512.0f, 1.0f, 100.0f); 
    //fpOrtho(lightProj, 4.0f, -4.0f, -10.0f, 10.0f, 0.1f, 7.0f);
    setView(lightView);
    setProjection(lightProj);
    Mesh *m;
    DL_FOREACH(cena->meshList, m) {
        setModel(m->transform);
        bindShader(firstPassShader);
        drawIndexedVAO(m->vaoId, m->indicesCount, GL_TRIANGLES);
    }

    bindFramebuffer(NULL);
    //glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);   
}

void secondPass() {
    setView(c.modelview);
    setProjection(c.projection); //TODO isso so precisaria ser calculado/setado 1x
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //TODO calcula shadow matrix S = bias*lproj*lview*model;
    mat4 bias;
    fpIdentity(bias);
    bias[0] = bias[5] = bias[10] = bias[12] = bias[13] = bias[14] = 0.5;
    mat4 lightMVP, bpv;
    fpMultMatrix(lightMVP, lightProj, lightView);
    fpMultMatrix(bpv, bias, lightMVP);
    Mesh *m;
    DL_FOREACH(cena->meshList, m) {
        setModel(m->transform);
        bindMaterial(m->material, &l);
        mat4 shadowMatrix;
        fpMultMatrix(shadowMatrix, bpv, m->transform);
        setShaderConstant4x4f(m->material->shdr, "shadowMatrix", shadowMatrix);
        bindSamplerState(shadowfb->tex->state, 0);
        bindTexture(shadowfb->tex, 0);
        bindShader(m->material->shdr);
        drawIndexedVAO(m->vaoId, m->indicesCount, GL_TRIANGLES);
    }
}
