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

//Polygon Triangulation Related
typedef struct vert {
    int id;
    double coord[2];
    bool ear;
    struct vert *next, *prev;
} VertexNode2D;

double TriangleArea2D(double a[2], double b[2], double c[2]) {
    return (b[0] - a[0])*(c[1] - a[1]) - (c[0] - a[0])*(b[1] - a[1]);
}

bool LeftPt2D(double a[2], double b[2], double c[2]) {
    return DGreater(TriangleArea2D(a, b, c), 0.0, EPS_FLOAT);
}

bool LeftOnPt2D(double a[2], double b[2], double c[2]) {
    double area = TriangleArea2D(a, b, c);
    return DAEqual(area, 0.0, EPS_FLOAT) || DGreater(area, 0.0, EPS_FLOAT);
}

bool CollinearPt2D(double a[2], double b[2], double c[2]) {
    return DEqual(TriangleArea2D(a, b, c), 0.0, EPS_FLOAT);
}

bool IntersectPropLine2D(double a[2], double b[2], double c[2], double d[2]) {
    //Checks collinearity
    if(CollinearPt2D(a, b, c) || CollinearPt2D(a, b, d) ||
       CollinearPt2D(c, d, a) || CollinearPt2D(c, d, b) )
        return false;

    return (!LeftPt2D(a, b, c) != !LeftPt2D(a, b, d)) && (!LeftPt2D(c, d, a) != !LeftPt2D(c, d, b));
}

bool BetweenPt2D(double a[2], double b[2], double c[2]) {
    if(!CollinearPt2D(a, b, c))
        return false;

    if(!DEqual(a[0], b[0], EPS_FLOAT)) {
        bool a0lec0 = DEqual(a[0], c[0], EPS_FLOAT) || DLesser(a[0], c[0], EPS_FLOAT);
        bool c0leb0 = DEqual(c[0], b[0], EPS_FLOAT) || DLesser(c[0], b[0], EPS_FLOAT);
        bool a0gec0 = DEqual(a[0], c[0], EPS_FLOAT) || DGreater(a[0], c[0], EPS_FLOAT);
        bool c0geb0 = DEqual(c[0], b[0], EPS_FLOAT) || DGreater(c[0], b[0], EPS_FLOAT);
        return (a0lec0 && c0leb0) || (a0gec0 && c0geb0);
    } else {
        bool a1lec1 = DEqual(a[1], c[1], EPS_FLOAT) || DLesser(a[1], c[1], EPS_FLOAT);
        bool c1leb1 = DEqual(c[1], b[1], EPS_FLOAT) || DLesser(c[1], b[1], EPS_FLOAT);
        bool a1gec1 = DEqual(a[1], c[1], EPS_FLOAT) || DGreater(a[1], c[1], EPS_FLOAT);
        bool c1geb1 = DEqual(c[1], b[1], EPS_FLOAT) || DGreater(c[1], b[1], EPS_FLOAT);
        return (a1lec1 && c1leb1) || (a1gec1 && c1geb1);
    }
//    if(a[0] != b[0])
//        return ((a[0] <= c[0]) && (c[0] <= b[0])) ||
//               ((a[0] >= c[0]) && (c[0] >= b[0]));
//    else 
//        return ((a[1] <= c[1]) && (c[1] <= b[1])) ||
//               ((a[1] >= c[1]) && (c[1] >= b[1]));
}

bool IntersectLine2D(double a[2], double b[2], double c[2], double d[2]) {
    if(IntersectPropLine2D(a, b, c, d))
        return true;
    else if(BetweenPt2D(a, b, c) || BetweenPt2D(a, b, d) ||
            BetweenPt2D(c, d, a) || BetweenPt2D(c, d, b))
        return true;
    return false;
}

bool Diagonalie2D(VertexNode2D *a, VertexNode2D *b) {
    VertexNode2D *c = a->next, *c1;

    do {
        c1 = c->next;
        //TODO comparar os ponteiros apenas
        if((c != a) && (c1 != a) &&
           (c != b) && (c1 != b) &&
           IntersectLine2D(a->coord, b->coord, c->coord, c1->coord))
            return false;
        c = c->next;
    } while(c != a->prev);
    return true;
}

bool InCone2D(VertexNode2D *a, VertexNode2D *b) {
    VertexNode2D *a0 = a->prev, *a1 = a->next;
    if(LeftOnPt2D(a->coord, a1->coord, a0->coord)) 
        return LeftPt2D(a->coord, b->coord, a0->coord) && 
               LeftPt2D(b->coord, a->coord, a1->coord);

    return !(LeftOnPt2D(a->coord, b->coord, a1->coord) && 
             LeftOnPt2D(b->coord, a->coord, a0->coord));
}

bool Diagonal(VertexNode2D *a, VertexNode2D *b) {
    return InCone2D(a, b) && InCone2D(b, a) && Diagonalie2D(a, b);
}

void earInit(VertexNode2D *verts) {
    VertexNode2D *v0, *v1, *v2;
    v1 = verts;
    do {
        v0 = v1->prev;
        v2 = v1->next;
        v1->ear = Diagonal(v0, v2);
        v1 = v1->next;
    } while(v1 != verts);
}

int* EarClipTriangulate(VertexNode2D *vertices, int n) {
    int ntriangles = n - 2;
    int tindex = 0;
    int *triangles = malloc(sizeof(int)*ntriangles*3);
    earInit(vertices);
    while(n > 3) {
        VertexNode2D *v2 = vertices;
        do  {
            if(v2->ear) {
                VertexNode2D *v0, *v1, *v3, *v4;
                v3 = v2->next;
                v4 = v3->next;
                v1 = v2->prev;
                v0 = v1->prev;

                //printDiagonal(v1, v3)
                printf("diagonal: (%d, %d)\n", v1->id, v3->id);
                v1->ear = Diagonal(v0, v3);
                v3->ear = Diagonal(v1, v4);
                triangles[tindex] = v1->id;
                triangles[tindex + 1] = v2->id;
                triangles[tindex + 2] = v3->id;
                tindex += 3;

                //remove v2
                v1->next = v3;
                v3->prev = v1;
                vertices = v3;
                n--;
                break;
            }
            v2 = v2->next;
        }while(v2 != vertices);
    }
    printf("ultimo triangle\n");
    //os 3 últimos vertices representam o utlimo triangulo
    triangles[tindex] = vertices->id;
    triangles[tindex + 1] = vertices->next->id;
    triangles[tindex + 2] = vertices->next->next->id;
    return triangles;
}

int* PolygonTriangulation(double *vertices, int nverts) {
    //Projeta os vertices em um plano
    //1. encontrar 2 vetores linearmente independentes no plano
    double iv[3] = {0.0, 0.0, 0.0}, jv[3] = {0.0, 0.0, 0.0}, o[3] = {0.0, 0.0, 0.0};
    for(int i = 1; i < nverts; i++) {
        int prev = i - 1;
        int next = (i + 1)%nverts;
        double v0[3] = { vertices[3*prev], vertices[3*prev + 1], vertices[3*prev + 2]};
        double v1[3] = { vertices[3*i], vertices[3*i + 1], vertices[3*i + 2]};
        double v2[3] = { vertices[3*next], vertices[3*next + 1], vertices[3*next + 2]};

        iv[0] = v2[0] - v1[0]; iv[1] = v2[1] - v1[1]; iv[2] = v2[2] - v1[2];
        jv[0] = v0[0] - v1[0]; jv[1] = v0[1] - v1[1]; jv[2] = v0[2] - v1[2];

        //verifica se os vetores são linearmente independentes
        if((iv[0]/jv[0] != iv[1]/jv[1]) || (iv[0]/jv[0] != iv[2]/jv[2])) {
            double ivlen = sqrt(iv[0]*iv[0] + iv[1]*iv[1] + iv[2]*iv[2]);
            iv[0] /= ivlen; iv[1] /= ivlen; iv[2] /= ivlen;
            double jvlen = sqrt(jv[0]*jv[0] + jv[1]*jv[1] + jv[2]*jv[2]);
            jv[0] /= jvlen; jv[1] /= jvlen; jv[2] /= jvlen;
            o[0] = v1[0]; o[1] = v1[1]; o[2] = v1[2];
            printf("iv: (%f, %f, %f), jv: (%f, %f, %f)\n", iv[0], iv[1], iv[2], jv[0], jv[1], jv[2]);
            printf("o: (%f, %f, %f)\n", o[0], o[1], o[2]);
            break;
        }
    }

    //2. escrever todos os pontos do poligono em função destes vetores
    double vertices2D[2*nverts];
    for(int i = 0; i < nverts; i++) {
        double vec[3] = { vertices[3*i] - o[0], vertices[3*i + 1] - o[1], vertices[3*i + 2] - o[2]};
        vertices2D[2*i] = iv[0]*vec[0] + iv[1]*vec[1] + iv[2]*vec[2];
        vertices2D[2*i + 1] = jv[0]*vec[0] + jv[1]*vec[1] + jv[2]*vec[2];
    }

    //Constroi lista de vertices
    VertexNode2D *vlist = NULL;
    for(int i = nverts - 1; i  >= 0; i--) {
        VertexNode2D *v = malloc(sizeof(VertexNode2D));
        v->id = i;
        v->coord[0] = vertices2D[2*i];
        v->coord[1] = vertices2D[2*i + 1];
        v->ear = false;
        CDL_PREPEND(vlist, v); 
    }
    //VertexNode2D *v;
    //printf("poligono:\n");
    //CDL_FOREACH(vlist, v) {
    //    printf("%d: (%f, %f), prev: %d, next: %d\n", v->id, v->coord[0], v->coord[1], v->prev->id, v->next->id);
    //}
        
    return EarClipTriangulate(vlist, nverts);
}

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
    Mesh *shortBox = CreateBox(1.0, 1.0, 1.0);
    //Node *shortBoxNode = AddMesh(cena, shortBox);
    //shortBoxNode->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);

    //int nverts = 10;
    //double vertices[30] = { 0.0, 0.5, 0.0, 0.3, 0.0, 0.0, 0.5, 0.5, 0.0, 0.7, 0.0, 0.0, 1.0, 0.8, 0.0, 0.75, 0.75, 0.0, 0.45, 1.0, 0.0, 0.31, 0.5, 0.0, 0.25, 0.55, 0.0, 0.29, 0.8, 0.0};
    //int nverts = 3;
    //double vertices[9] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    //int *triangles = PolygonTriangulation(vertices, nverts);
    //for(int i = 0; i < nverts - 2; i++)
    //    printf("triangle %d: %d, %d, %d\n", i, triangles[3*i], triangles[3*i + 1], triangles[3*i + 2]);
    
    HEMesh *hem = HECreateCube();
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
        //do {
        //    nvf++;
        //    HEVertex *v = he->vertex;
        //    printf("\tvisitando vertice: v%d\n", v->id);
        //} while((he = he->next) != l->hedge);
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
