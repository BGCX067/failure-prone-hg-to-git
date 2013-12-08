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
#include "util/textfile.h"
#include "util/ezxml.h"
#include "renderer/material.h"
#include "renderer/glime.h"
#include <stdbool.h>
#include "math/mathutil.h"
#include <math.h>
#include "halfedge.h"

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

//Helper functions
Mesh *HEMeshToMesh(HEMesh *hem);

void initializeGame(){
    //InitializeGUI(GetScreenW(), GetScreenH());
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
    
    HEMesh *hem = HECreateCubeExtrude();
    Mesh *heobj = HEMeshToMesh(hem);
    Node *heobjNode = AddMesh(cena, heobj);
    heobjNode->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);
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
    
    
    /*Rect r1 = { 10.0, 575.0, 0.0, 0.0};
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
    EndGUI();*/

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
    sprintf(cposstr, "Pos: (%f, %f, %f)", c.pos[0], c.pos[1], c.pos[2]);
    /**
     *  Centro de rotação da câmera
     **/
    sprintf(cpivotstr, "Pivot: (%f, %f, %f)", c.pivot[0], c.pivot[1], c.pivot[2]);
    /**
     *  Lookat da camera, exatamente ao vetor -Z que pode ser obtido (veja abaixo)
     *  da matriz VIEW, apenas calculado utilizando a posição da camera e o pivot.
     **/
    sprintf(lookatstr, "Lookat: (%f, %f, %f)", lookat[0], lookat[1], lookat[2]);
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
    sprintf(mv0str, "MV0: (%f, %f, %f)", c.mview[0], c.mview[4], c.mview[8]);
    sprintf(mv1str, "MV1: (%f, %f, %f)", c.mview[1], c.mview[5], c.mview[9]);
    sprintf(mv2str, "MV2: (%f, %f, %f)", c.mview[2], c.mview[6], c.mview[10]);
    /**
     *  Informações de projeção só pra constar
     **/
    sprintf(fovystr, "fovy: %f", c.fovy);
    sprintf(znearstr, "znear: %f", c.znear);
    sprintf(zfarstr, "zfar: %f", c.zfar);

    SetView(c.mview);

    //if(e->type & MOUSE_BUTTON_PRESS) {
    //   printf("wheel: %d\n", e->wheel); 
    //}

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

bool debug = false;

int  countVerticesFace(HEHalfEdge *h) {
    int nvf = 0;
    HEHalfEdge *he = h;
    do {
        nvf++;
        HEVertex *v = he->vertex;
        if(debug)
            printf("\tvisitando vertice: v%d\n", v->id);
    } while((he = he->next) != h);
    return nvf;
}

void countVerticesIndices(HEMesh *hem, int *nv, int *ni) {
    HEFace *fit;
    DL_FOREACH(hem->faces, fit) {
        if(debug)
            printf("visitando a face: f%d\n", fit->id);
        HELoop *l = fit->lout;
        int nvf = countVerticesFace(l->hedge);
        *nv += nvf;
        //number of triangles created in that face is nvf - 2
        *ni += 3*(nvf - 2);
    }
}

Mesh *HEMeshToMesh(HEMesh *hem) {
    debug = false;
    //number of vertices (same as number of normals) and number of indices
    int nv = 0, ni = 0;
    countVerticesIndices(hem, &nv, &ni);
    
    float *vertices = malloc(sizeof(float)*nv*3);
    float *normals = malloc(sizeof(float)*nv*3);
    unsigned int *indices = malloc(sizeof(unsigned int)*ni*3);
    
    debug = true;
    if(debug)
        printf("number of vertices: %d\nnumber of indices: %d\n", nv, ni); 
    
    int vc = 0, tc = 0, indexacc = 0;
    HEFace *fit;
    DL_FOREACH(hem->faces, fit) {
        HELoop *l = fit->lout;
        debug = false;
        int nvf = countVerticesFace(l->hedge);
        debug = true;
        if(debug)
            printf("visitando a face: f%d, nvf: %d\n", fit->id, nvf);
        //com a linha poligonal, triangulariza a face
        double *polyface = malloc(sizeof(double)*nvf*3);
        int i = 0;
        HEHalfEdge *he = l->hedge;
        do {
            HEVertex *v = he->vertex;
            polyface[3*i] = v->coord[0];
            polyface[3*i + 1] = v->coord[1];
            polyface[3*i + 2] = v->coord[2];

            vertices[3*vc] = v->coord[0];
            vertices[3*vc + 1] = v->coord[1];
            vertices[3*vc + 2] = v->coord[2];
            
            if(debug)
                printf("\tvisitando vertice: v%d\n", v->id);

            i++; vc++;
        } while((he = he->next) != l->hedge);

        int *tris = PolygonTriangulation(polyface, nvf);
        for(int j = 0; j < nvf - 2; j++) {
            indices[3*tc] = indexacc + tris[3*j];
            indices[3*tc + 1] = indexacc + tris[3*j + 1];
            indices[3*tc + 2] = indexacc + tris[3*j + 2];
            tc++;
        }
        indexacc += nvf;
        free(polyface);
        free(tris);
    }
    if(debug)
        printf("tc: %d, vc: %d, indexacc: %d\n", tc, vc, indexacc);

    if(debug)
        for(int i = 0; i < tc; i++) 
            printf("t%d: (%d, %d, %d)\n", i, indices[3*i], indices[3*i + 1], indices[3*i + 2]);
    
    Mesh *m = InitMesh();
    for(int i = 0; i < 3*vc; i++)
        normals[i] = 0.0;
    SetNormals(indices, vertices, normals, tc, vc);
    AddVertices(m, 3*vc, 3, vertices);
    AddNormals(m, 3*vc, 3, normals);
    AddIndices(m, 3*tc, indices);
    PrepareMesh(m);
    
    free(vertices);
    free(normals);
    free(indices);

    return m;
}
