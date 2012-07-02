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
#include <string.h>

typedef struct {
    unsigned char isovalue;
    float r, g, b, a;
} TransferNode;

typedef struct {
    unsigned char *voldata;
    unsigned char *transferdata; //sempre tamanho 256?
    int xdim, ydim, zdim;
} VolumetricScene;



Scene* cena;
Shader *shdr;
Camera c;
BoundingBox bbox;
Light l;

Triangles *t;

int menux, menuy;
float alpha = 0.9;
float samples = 0.5;

char *cenasComboBox[] = {"Bonsai", "Head"};
char *scenePath[] = {"data/scene1.txt\0", "data/scene2.txt\0"};
int comboboxState = 0;
int currScene = 0;
int prevScene = 0;

VolumetricScene *currVolScene;

char volWidth[50];
char volHeight[50];
char volDepth[50];


unsigned char* loadVolumeFromFile(const char *filename, int xdim, int ydim, int zdim) {
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

unsigned char* loadTransferFunction1D(const char* filename) { 
    unsigned char *data = malloc(256*sizeof(unsigned char)*4);
    
    FILE *fp = fopen(filename, "rb");
    if(fp == NULL) {
        printf("Problema ao ler o arquivo: %s\n", filename);
        exit(0);
    }

    char buff[512];
    TransferNode *colors;
    TransferNode *alphas;
    int nColors;
    int nAlphas;
    while (!feof (fp)) {
        fgets (buff, sizeof (buff), fp);
        if (strncmp(buff, "\%Color", 6) == 0) {
            fgets (buff, sizeof (buff), fp);
            sscanf(buff, "%d", &nColors);
            colors = malloc(nColors*sizeof(TransferNode));
            for(int i = 0; i < nColors; i++) {
                fgets (buff, sizeof (buff), fp);
                sscanf(buff, "%d %f %f %f", &(colors[i].isovalue), &(colors[i].r), &(colors[i].g), &(colors[i].b));
            }
        } else if (strncmp(buff, "\%Alpha", 6) == 0) {
            fgets (buff, sizeof (buff), fp);
            sscanf(buff, "%d", &nAlphas);
            alphas = malloc(nAlphas*sizeof(TransferNode));
            for(int i = 0; i < nAlphas; i++) {
                fgets (buff, sizeof (buff), fp);
                sscanf(buff, "%d %f", &alphas[i].isovalue, &alphas[i].a);
            }
        }
    }

    //TODO interpolar os valores entre os intervalos
    for(int i = 0; i < nColors - 1; i++) {
        for(int j = colors[i].isovalue; j <= colors[i + 1].isovalue; j++) {
            data[4*j] = colors[i].r*255;
            data[4*j + 1] = colors[i].g*255;
            data[4*j + 2] = colors[i].b*255;
        }
    }
    
    for(int i = 0; i < nAlphas - 1; i++) {
        for(int j = alphas[i].isovalue; j <= alphas[i + 1].isovalue; j++) {
            data[4*j + 3] = alphas[i].a*255;
        }
    }

    free(colors);
    free(alphas);
    fclose(fp);
    return data;
}

VolumetricScene* loadVolumeScene(const char *filename, Triangles *box) {
    FILE *fp = fopen(filename, "rb");
    if(fp == NULL) {
        printf("Problema ao ler o arquivo: %s\n", filename);
        return 0;
    }
    
    VolumetricScene *vs = malloc(sizeof(VolumetricScene));
    char buff[256];
    while (!feof (fp)) {
        fgets (buff, sizeof (buff), fp);
        if (strncmp(buff, "\%VolumeData", 11) == 0) {
            int xdim, ydim, zdim;
            //Ler path
            fgets (buff, sizeof (buff), fp);
            char path[128];
            strcpy(path, buff);
            for(int i = 0; i < 128; i++)
                if(path[i] == '\n')
                    path[i] = '\0';
            fgets (buff, sizeof (buff), fp);
            sscanf(buff, "xdim %d", &xdim);
            fgets (buff, sizeof (buff), fp);
            sscanf(buff, "ydim %d", &ydim);
            fgets (buff, sizeof (buff), fp);
            sscanf(buff, "zdim %d", &zdim);
            
            vs->voldata = loadVolumeFromFile(path, xdim, ydim, zdim);
            vs->xdim = xdim;
            vs->ydim = ydim;
            vs->zdim = zdim;
        }
        if (strncmp(buff, "\%TransferData", 13) == 0) {
            int useTransferFunc;
            fgets (buff, sizeof (buff), fp);
            sscanf(buff, "%d", &useTransferFunc);
            if(!useTransferFunc) {
                vs->transferdata = NULL;
            } else {
                fgets (buff, sizeof (buff), fp);
                char path[128];
                strcpy(path, buff);
                for(int i = 0; i < 128; i++)
                    if(path[i] == '\n')
                        path[i] = '\0';
                vs->transferdata = loadTransferFunction1D(path);
            }

        }
    }
    fclose(fp);

    Texture *tex = initialize3DTexture(vs->voldata, vs->xdim, vs->ydim, vs->zdim, GL_LUMINANCE, GL_INTENSITY, GL_UNSIGNED_BYTE);

    //Inicializar transfer function
    if(vs->transferdata) {
        Texture *tex2 = initializeTextureFromMemory(vs->transferdata, 255, 0, GL_TEXTURE_1D, GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE);
        box->material = volumeMaterialTransfer(tex, tex2);
    } else {
        box->material = volumeMaterial(tex);
    }

    return vs;
}

void initializeGame(){
    menux = menuy = 0;
    initializeGUI(800, 600);

    cena = initializeScene();

    Mesh *m = initMesh();
    t = addTris(m);
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
    //const char* initScene = "data/scene2.txt\0";
    currVolScene = loadVolumeScene(scenePath[currScene], t);
    if(!currVolScene) {
        printf("Erro ao ler a cena: %s", scenePath[currScene]);
        exit(0);
    }

    sprintf(volWidth, "Volume width: %d", currVolScene->xdim);
    sprintf(volHeight, "Volume height: %d", currVolScene->ydim);
    sprintf(volDepth, "Volume depth: %d", currVolScene->zdim);


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

    vec3 asdf;
    asdf[0] = alpha;
    asdf[1] = 0.0;
    asdf[2] = 0.0;
    //Desenhar cena
    if (cena->meshList){
        Mesh* m = NULL;
        for( int i = 0; i < cena->meshList->size; i++){ // para da mesh da cena
            m = fplist_getdata(i, cena->meshList);
            if (m->tris){
                Triangles* tri = NULL;
                for( int k = 0; k < m->tris->size; k++){ //para cada chunk de triangles do mesh
                    vec3 steps;
                    steps[0] = 100*samples;
                    steps[1] = 0;
                    steps[2] = 0;

                    tri = fplist_getdata(k, m->tris);
                    setShaderConstant3f(tri->material->shdr, "eyepos", c.pos); 
                    setShaderConstant3f(tri->material->shdr, "alpha", asdf); 
                    setShaderConstant3f(tri->material->shdr, "steps", steps);
                    setShaderConstant4x4f(tri->material->shdr, "invModelview", invModelview); 
                    bindMaterial(tri->material, &l);
                    bindShader(tri->material->shdr);
                    drawIndexedVAO(tri->vaoId, tri->indicesCount, GL_TRIANGLES);
                }
            }
        }
    }
    glDisable(GL_BLEND);

    int newScene = currScene;   
    rect r,  r2, r3, r4, r5, r6, r7, r8, r9;
    beginGUI(e);
	beginMenu(1, 200, 300, 250, 150, &menux, &menuy, "RayCast" );
		r.x = 210, r.y = 430;
		doLabel(&r, "Alpha");
		r6.x = 310; r6.y = 430;
		doHorizontalSlider(3, &r6, &alpha);
		r2.x = 210; r2.y = 410;
		doLabel(&r2, "Samples");
		r7.x = 310; r7.y = 410;
		doHorizontalSlider(4, &r7, &samples);
        r5.x = 210; r5.y = 390;
        doLabel(&r5, volWidth);
        r7.x = 210; r7.y = 370;
        doLabel(&r7, volHeight);
        r8.x = 210; r8.y = 350;
        doLabel(&r8, volDepth);
	endMenu(1, 200, 300, 200, 150, &menux, &menuy);
        r3.x = 20; r3.y = 580;
        doLabel(&r3, "Cena");
        r4.x = 80; r4.y = 575;
        doComboBox(5, &r4, 2, cenasComboBox, &newScene, &comboboxState);
    endGUI();
    if(currScene != newScene) {
        prevScene = currScene;
        currScene = newScene;
        printf("currscene: %d; prevScene: %d\n", currScene, prevScene);
        //deleta o volume anterior
        free(currVolScene->voldata);
        if(currVolScene->transferdata)
            free(currVolScene->transferdata);
        free(currVolScene);
        currVolScene = loadVolumeScene(scenePath[currScene], t);
        prevScene = currScene;

        sprintf(volWidth, "Volume width: %d", currVolScene->xdim);
        sprintf(volHeight, "Volume height: %d", currVolScene->ydim);
        sprintf(volDepth, "Volume depth: %d", currVolScene->zdim);
    }

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
