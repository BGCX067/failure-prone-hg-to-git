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

Camera c;
Scene* cena;

Shader *fshdr;
Mesh *mfaces;
Mesh *medges;
Mesh *mverts;

HEMesh *hem;

//Coordinate axis
Shader *colorshdr;
Batch *coordaxis;

void createCoordAxis();

//Helper functions
static Mesh *HEMeshToMesh(HEMesh *hem);
static Mesh *HEMeshToEdges(HEMesh *hem);
static Mesh *HEMeshToVertices(HEMesh *hem);

void initializeGame(){
    InitializeGUI(GetScreenW(), GetScreenH());
    //CamInit(&c, GetScreenW(), GetScreenH(), FPS, PERSPECTIVE); 
    CamInit(&c, GetScreenW(), GetScreenH(), TRACKBALL, PERSPECTIVE); 
    SetZfar(&c, 1000.0);
    SetProjection(c.mprojection);
    //Setvf(c.pos, 0.0, 0.0, 3.0);
    c.zoom = 3.0;

    char* vertsrc = ReadTextFile("data/shaders/color.vert");
    char* fragsrc = ReadTextFile("data/shaders/color.frag"); 
    colorshdr = InitializeShader(NULL, vertsrc, fragsrc);
    free(vertsrc);
    free(fragsrc);

    createCoordAxis();

    Light l; 
    l.pos[0]= 2.0; l.pos[1] = 1.92; l.pos[2] = 2.0;
    l.color[0] = 1.0; l.color[1] = 1.0; l.color[2] = 1.0; l.color[3] = 1.0;

    vec3 khakiAmb = {0.1, 0.1, 0.1};
    vec3 khakiDiff = {0.472, 0.484, 0.41};
    vec3 khakiSpec = {0.125, 0.125, 0.125};
    float khakiShininess = 12.5;

    cena = InitializeScene();
    
    //HEMesh *hem = HECreateCube();
    //Node *heobjNode = AddMesh(cena, heobj);
    //heobjNode->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);
    
    hem = HECreateQuadXZ(-1.0, -1.0, 1.0, 1.0);
    fshdr = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);
    mfaces = HEMeshToMesh(hem);
    medges = HEMeshToEdges(hem);
    mverts = HEMeshToVertices(hem);

    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glLineWidth(2.0);
    glPointSize(9.0);
    //glClearColor(1.0, 1.0, 1.0, 1.0); 
}

int drawEdges = 1;
int drawVertices = 1;
int drawRotCenter = 0;
float menux1 = 0.0, menux2 = 0.0;
int menubutton = 0;
int Render(event *e, double* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //DrawScene(cena);
    //mat4 identity;
    //Identity(identity);
    //SetModel(iden)

    BindShader(fshdr);
    DrawIndexedVAO(mfaces->vaoId, mfaces->indicesCount, GL_TRIANGLES);
    

    //Draw edges and vertices
    BindShader(colorshdr);
    if(drawEdges)
        DrawIndexedVAO(medges->vaoId, medges->indicesCount, GL_LINES);
    if(drawVertices)
    DrawArraysVAO(mverts->vaoId, GL_POINTS, mverts->verticesCount/3);

    //Draw the coordinate axis
    glDisable(GL_DEPTH_TEST);
    if(drawRotCenter) {
        //Translatef(c.mview, c.pivot[0], c.pivot[1], c.pivot[2]);
        //SetView(c.mview);
        BindShader(colorshdr);
        Draw(coordaxis);
    }
    glEnable(GL_DEPTH_TEST);
    
    BeginGUI(e);
        Rect rb1 = { 10.0, 575.0, 0.0, 0.0};
        Rect rb2 = { 77.0, 575.0, 0.0, 0.0};
        Rect rb3 = { 150.0, 575.0, 0.0, 0.0};
        Rect rb4 = { 230.0, 575.0, 0.0, 0.0};
        DoToggleButton(1, &rb1, "Render", &menubutton);
        DoToggleButton(2, &rb2, "Extrude", &menubutton);
        DoToggleButton(3, &rb3, "Split X", &menubutton);
        DoToggleButton(4, &rb4, "Split Y", &menubutton);
        if(menubutton == 1) {
            BeginMenu(5, rb1.x, 500, 100, 75, &menux1, &menux2, "Draw", NULL);
                Rect r1 = { rb1.x + 5.0, 550.0, 0.0, 0.0};
                Rect r2 = { rb1.x + 5.0, 530.0, 0.0, 0.0};
                Rect r3 = { rb1.x + 5.0, 510.0, 0.0, 0.0};
                DoCheckButton(6, &r1, "Edges", &drawEdges);
                DoCheckButton(7, &r2, "Vertices", &drawVertices);
                DoCheckButton(8, &r3, "Axis", &drawRotCenter);
            EndMenu(4, rb1.x, 500, 100, 75, &menux1, &menux2);
        } else if(menubutton == 2) {
            BeginMenu(5, rb2.x, 425, 200, 150, &menux1, &menux2, "Extrude", NULL);
                static char facestr[3] = "0";
                static char dxstr[5] = "0.0";
                static char dystr[5] = "2.0";
                static char dzstr[5] = "0.0";
                DoLabel(rb2.x + 5.0, rb2.y - 25.0, "Face:" );
                Rect r1 = {rb2.x + 75.0, rb2.y - 25.0, 0.0, 0.0};
                DoLineEdit(6,  &r1, facestr, 3);

                DoLabel(rb2.x + 5.0, rb2.y - 50.0, "dx:");
                Rect r2 = {rb2.x + 75.0, rb2.y - 50.0, 0.0, 0.0};
                DoLineEdit(7,  &r2, dxstr, 4);

                DoLabel(rb2.x + 5.0, rb2.y - 75.0, "dy:" );
                Rect r3 = {rb2.x + 75.0, rb2.y - 75.0, 0.0, 0.0};
                DoLineEdit(8,  &r3, dystr, 4);

                DoLabel(rb2.x + 5.0, rb2.y - 100.0, "dz:" );
                Rect r4 = {rb2.x + 75.0, rb2.y - 100.0, 0.0, 0.0};
                DoLineEdit(9,  &r4, dzstr, 4);

                Rect r5 = {rb2.x + 80, rb2.y - 135, 0.0, 0.0};
                if(DoButton(10, &r5, "Ok")) {
                    ExtrudeFace(hem, atoi(facestr), atof(dxstr), atof(dystr), atof(dzstr));
                    mfaces = HEMeshToMesh(hem);
                    medges = HEMeshToEdges(hem);
                    mverts = HEMeshToVertices(hem);

                    HEFace *fit;
                    DL_FOREACH(hem->faces, fit) {
                        printf("f%d (", fit->id);
                        HEHalfEdge *he = fit->lout->hedge;
                        do {
                            HEVertex *v = he->vertex;
                            printf("%d  ", v->id);
                        } while((he = he->next) != fit->lout->hedge);
                        printf(")\n");
                    }

                }
            EndMenu(5, rb2.x, 425, 200, 150, &menux1, &menux2);
        } else if(menubutton == 3) {
            BeginMenu(5, rb3.x, 425, 200, 150, &menux1, &menux2, "Split X", NULL);
                static char facestr[3] = "3";
                static char p1str[5] = "0.1";
                static char p2str[5] = "0.8";
                static char p3str[5] = "0.1";
                DoLabel(rb3.x + 5.0, rb3.y - 25.0, "Face:" );
                Rect r1 = {rb3.x + 75.0, rb3.y - 25.0, 0.0, 0.0};
                DoLineEdit(6,  &r1, facestr, 3);

                DoLabel(rb3.x + 5.0, rb3.y - 50.0, "p1:");
                Rect r2 = {rb3.x + 75.0, rb3.y - 50.0, 0.0, 0.0};
                DoLineEdit(7,  &r2, p1str, 4);

                DoLabel(rb3.x + 5.0, rb3.y - 75.0, "p2:" );
                Rect r3 = {rb3.x + 75.0, rb3.y - 75.0, 0.0, 0.0};
                DoLineEdit(8,  &r3, p2str, 4);

                DoLabel(rb3.x + 5.0, rb3.y - 100.0, "p3:" );
                Rect r4 = {rb3.x + 75.0, rb3.y - 100.0, 0.0, 0.0};
                DoLineEdit(9,  &r4, p3str, 4);

                Rect r5 = {rb3.x + 80, rb3.y - 135, 0.0, 0.0};
                if(DoButton(10, &r5, "Ok")) {
                    SplitEdge(hem, 1, 0.1); //vertice 8
                    int nv1 = hem->numvertices - 1; 
                    SplitEdge(hem, 1, 0.8/0.9); //vertice 9
                    int nv2 = hem->numvertices - 1; 
                    //SplitEdge(hem, 1, 0.5); //vertice 10

                    SplitEdge(hem, 8, 0.1); //vertice 10
                    int nv3 = hem->numvertices - 1; 
                    SplitEdge(hem, 8, 0.8/0.9); //vertice 11
                    int nv4 = hem->numvertices - 1; 
                    //SplitEdge(hem, 8, 0.5); //vertice 13
                    SplitFace(hem, atoi(facestr), nv1, nv3);
                    SplitFace(hem, atoi(facestr), nv2, nv4);
                    //SplitFace(hem, 3, 10, 13);
                    HEFace *fit;
                    DL_FOREACH(hem->faces, fit) {
                        printf("f%d (", fit->id);
                        HEHalfEdge *he = fit->lout->hedge;
                        do {
                            HEVertex *v = he->vertex;
                            printf("%d  ", v->id);
                        } while((he = he->next) != fit->lout->hedge);
                        printf(")\n");
                    }

                    mfaces = HEMeshToMesh(hem);
                    medges = HEMeshToEdges(hem);
                    mverts = HEMeshToVertices(hem);
                    //printf(SplitFace(hem, atoi(facestr), ); 
                }
            EndMenu(5, rb3.x, 425, 200, 150, &menux1, &menux2);
        } else if(menubutton == 4) {
            BeginMenu(5, rb4.x, 375, 200, 200, &menux1, &menux2, "Split X", NULL);
                static char facestr[3] = "7";
                static char p1str[5] = "0.25";
                static char p2str[5] = "0.25";
                static char p3str[5] = "0.25";
                static char p4str[5] = "0.25";
                DoLabel(rb4.x + 5.0, rb4.y - 25.0, "Face:" );
                Rect r1 = {rb4.x + 75.0, rb4.y - 25.0, 0.0, 0.0};
                DoLineEdit(6,  &r1, facestr, 3);

                DoLabel(rb4.x + 5.0, rb4.y - 50.0, "p1:");
                Rect r2 = {rb4.x + 75.0, rb4.y - 50.0, 0.0, 0.0};
                DoLineEdit(7,  &r2, p1str, 4);

                DoLabel(rb4.x + 5.0, rb4.y - 75.0, "p2:" );
                Rect r3 = {rb4.x + 75.0, rb4.y - 75.0, 0.0, 0.0};
                DoLineEdit(8,  &r3, p2str, 4);

                DoLabel(rb4.x + 5.0, rb4.y - 100.0, "p3:" );
                Rect r4 = {rb4.x + 75.0, rb4.y - 100.0, 0.0, 0.0};
                DoLineEdit(9,  &r4, p3str, 4);

                DoLabel(rb4.x + 5.0, rb4.y - 100.0, "p4:" );
                Rect r5 = {rb4.x + 75.0, rb4.y - 125.0, 0.0, 0.0};
                DoLineEdit(9,  &r5, p4str, 4);

                Rect r6 = {rb4.x + 80, rb4.y - 160, 0.0, 0.0};
                if(DoButton(10, &r6, "Ok")) {
                    SplitEdge(hem, 16, 0.25); //vertice 12
                    int nv1 = hem->numvertices - 1;
                    SplitEdge(hem, 16, 0.333); //vertice 13
                    int nv2 = hem->numvertices - 1;
                    SplitEdge(hem, 16, 0.5); //vertice 14
                    int nv3 = hem->numvertices - 1;

                    SplitEdge(hem, 17, 0.25); //vertice 15
                    int nv4 = hem->numvertices - 1;
                    SplitEdge(hem, 17, 0.333); //vertice 16
                    int nv5 = hem->numvertices - 1;
                    SplitEdge(hem, 17, 0.5); //vertice 17
                    int nv6 = hem->numvertices - 1;

                    SplitFace(hem, atoi(facestr), nv1, nv4);
                    SplitFace(hem, atoi(facestr), nv2, nv5);
                    SplitFace(hem, atoi(facestr), nv3, nv6);
                    HEFace *fit;
                    DL_FOREACH(hem->faces, fit) {
                        printf("f%d (", fit->id);
                        HEHalfEdge *he = fit->lout->hedge;
                        do {
                            HEVertex *v = he->vertex;
                            printf("%d  ", v->id);
                        } while((he = he->next) != fit->lout->hedge);
                        printf(")\n");
                    }
                    medges = HEMeshToEdges(hem);
                    mverts = HEMeshToVertices(hem);
                    mfaces = HEMeshToMesh(hem);
                    
                    
                }
            EndMenu(5, rb4.x, 375, 200, 200, &menux1, &menux2);
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
    //sprintf(cposstr, "Pos: (%f, %f, %f)", c.pos[0], c.pos[1], c.pos[2]);
    /**
     *  Centro de rotação da câmera
     **/
    //sprintf(cpivotstr, "Pivot: (%f, %f, %f)", c.pivot[0], c.pivot[1], c.pivot[2]);
    /**
     *  Lookat da camera, exatamente ao vetor -Z que pode ser obtido (veja abaixo)
     *  da matriz VIEW, apenas calculado utilizando a posição da camera e o pivot.
     **/
    //sprintf(lookatstr, "Lookat: (%f, %f, %f)", lookat[0], lookat[1], lookat[2]);
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
    //sprintf(mv0str, "MV0: (%f, %f, %f)", c.mview[0], c.mview[4], c.mview[8]);
    //sprintf(mv1str, "MV1: (%f, %f, %f)", c.mview[1], c.mview[5], c.mview[9]);
    //sprintf(mv2str, "MV2: (%f, %f, %f)", c.mview[2], c.mview[6], c.mview[10]);
    /**
     *  Informações de projeção só pra constar
     **/
    //sprintf(fovystr, "fovy: %f", c.fovy);
    //sprintf(znearstr, "znear: %f", c.znear);
    //sprintf(zfarstr, "zfar: %f", c.zfar);

    SetView(c.mview);

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

int countVerticesFace(HEHalfEdge *h) {
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

static Mesh *HEMeshToMesh(HEMesh *hem) {
    debug = false;
    //number of vertices (same as number of normals) and number of indices
    int nv = 0, ni = 0;
    countVerticesIndices(hem, &nv, &ni);
    
    float *vertices = malloc(sizeof(float)*nv*3);
    float *normals = malloc(sizeof(float)*nv*3);
    unsigned int *indices = malloc(sizeof(unsigned int)*ni*3);
    
    debug = false;
    if(debug)
        printf("number of vertices: %d\nnumber of indices: %d\n", nv, ni); 
    
    int vc = 0, tc = 0, indexacc = 0;
    HEFace *fit;
    DL_FOREACH(hem->faces, fit) {
        HELoop *l = fit->lout;
        debug = false;
        int nvf = countVerticesFace(l->hedge);
        debug = false;
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
                printf("\tvisitando vertice: v%d: (%f, %f, %f)\n", v->id, v->coord[0], v->coord[1], v->coord[2]);

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
        
    Mesh *m = InitMesh();
    for(int i = 0; i < 3*vc; i++)
        normals[i] = 0.0;
    SetNormals(indices, vertices, normals, tc, vc);
    AddVertices(m, 3*vc, 3, vertices);
    AddNormals(m, 3*vc, 3, normals);
    AddIndices(m, 3*tc, indices);
    PrepareMesh(m);
    
    debug = false;
    if(debug)
        printf("tc: %d, vc: %d, indexacc: %d, ni: %d, nv: %d\n", tc, vc, indexacc, ni, nv);

    if(debug) {
        for(int i = 0; i < tc; i++) {
            printf("t%d: (%d, %d, %d)\t", i, indices[3*i], indices[3*i + 1], indices[3*i + 2]);
            int ni = indices[3*i];
            printf("n: (%f, %f, %f)\n", normals[3*ni], normals[3*ni + 1], normals[3*ni + 2]);
        }
    }
    
    free(vertices);
    free(normals);
    free(indices);

    return m;
}


static Mesh* HEMeshToEdges(HEMesh *hem) {
    float *vertices = malloc(sizeof(float)*hem->numvertices*3);
    unsigned int *indices = malloc(sizeof(unsigned int)*hem->numedges*2);
    float *colors = malloc(sizeof(float)*hem->numvertices*4);

    for(int i = 0; i < hem->numvertices; i++) {
        colors[4*i] = 0.0;
        colors[4*i + 1] = 0.0;
        colors[4*i + 2] = 0.0;
        colors[4*i + 3] = 1.0;
    }

    
    HEVertex *vit;
    DL_FOREACH(hem->vertices, vit) {
        vertices[3*vit->id] = vit->coord[0];
        vertices[3*vit->id + 1] = vit->coord[1];
        vertices[3*vit->id + 2] = vit->coord[2];
        printf("v%d: (%f, %f, %f)\n", vit->id, vit->coord[0], vit->coord[1],vit->coord[2]);
    }
    HEEdge *eit;
    DL_FOREACH(hem->edges, eit) {
        indices[2*eit->id] = eit->he1->vertex->id;
        indices[2*eit->id + 1] = eit->he2->vertex->id;
        printf("e%d: (%d, %d)\n", eit->id, eit->he1->vertex->id, eit->he2->vertex->id);
    }

    Mesh *m  = InitMesh();
    AddVertices(m, 3*hem->numvertices, 3, vertices);
    AddIndices(m, 2*hem->numedges, indices);
    AddColors(m, 4*hem->numvertices, colors);
    PrepareMesh(m);

    free(vertices);
    free(colors);
    free(indices);

    return m;
}

static Mesh* HEMeshToVertices(HEMesh *hem) {
    float *vertices = malloc(sizeof(float)*hem->numvertices*3);
    float *colors = malloc(sizeof(float)*hem->numvertices*4);

    for(int i = 0; i < hem->numvertices; i++) {
        colors[4*i] = 0.0;
        colors[4*i + 1] = 0.0;
        colors[4*i + 2] = 0.0;
        colors[4*i + 3] = 1.0;
    }
    
    HEVertex *vit;
    DL_FOREACH(hem->vertices, vit) {
        vertices[3*vit->id] = vit->coord[0];
        vertices[3*vit->id + 1] = vit->coord[1];
        vertices[3*vit->id + 2] = vit->coord[2];
    }

    Mesh *m  = InitMesh();
    AddVertices(m, 3*hem->numvertices, 3, vertices);
    AddColors(m, 4*hem->numvertices, colors);
    PrepareMesh(m);

    free(vertices);
    free(colors);

    return m;   
}
