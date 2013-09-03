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

enum InstanceType { EMPTY, CUBE }; 

typedef struct Rule {
    //Model local transformation
    mat4 itransform;
    int instance;

    int ncalls;
    int *cid;
    //Child node transformations
    mat4 *ctransform;
    int *ccount;

    float weight;
    struct Rule *prev, *next;
}Rule;

typedef struct MeshGrammar {
    Rule *entry;
    int rcount;
    Rule **rlist;
}MeshGrammar;

Scene* cena;
Camera c;
MeshGrammar *g;
Shader *material;

Mesh* createBox(float x, float y, float z);
//TODO:
//1. More shapes
//4. camera functions for better visualization
MeshGrammar *loadFromFile(const char *path);
void processRule(Rule *r, mat4 t, MeshGrammar *g, Node *parent, int depth);

void initializeGame(){
    //CamInit(&c, GetScreenW(), GetScreenH(), FPS, PERSPECTIVE); 
    CamInit(&c, GetScreenW(), GetScreenH(), TRACKBALL, PERSPECTIVE); 

    SetProjection(c.mprojection);

    Setvf(c.pos, 0.0, 0.0, 50.0);

    cena = InitializeScene();
    Light l; 
    l.pos[0]= 1.0; l.pos[1] = 0.92; l.pos[2] = 1.0;
    l.color[0] = 1.0; l.color[1] = 1.0; l.color[2] = 1.0; l.color[3] = 1.0;

    vec3 khakiAmb = {0.125, 0.1, 0.072};
    vec3 khakiDiff = {0.5, 0.41, 0.275};
    vec3 khakiSpec = {0.125, 0.125, 0.125};
    float khakiShininess = 12.5;
    material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color); 

//    Mesh *shortBox = createBox(1.0, 1.0, 1.0);
//    Node *shortBoxNode = AddMesh(cena, shortBox);
//    shortBoxNode->material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color); 
    
    //g = loadFromFile("data/spiral.xml");
    g = loadFromFile("data/2.xml");
    //g = loadFromFile("data/3.xml");
    mat4 ident;
    Identity(ident);
    processRule(g->entry, ident, g, cena->root, 20);
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

    return 1;
}

int main(){
	SetVideoMode(800, 600, 0);
	WarpMouse(0);
	SetWindowTitle("Fail Synth");
	//initializeRenderer(1024, 768, 0.1, 10000.0, 45.0);
	//Renderer *mainrenderer = InitializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	InitializeRenderer(800, 600, 0.1, 10000.0, 45.0);
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
    //float x = w;
    //float y = h;
    //float z = l;

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
/*    float vertices[] = { 0.0, 0.0, z, x, 0.0, z, 0.0, y, z,  //Front1
                         0.0, y, z, x, 0.0, z, x, y, z, //Front2
                         0.0, 0.0, 0.0, 0.0, y, 0.0, x, 0.0, 0.0, //Back1
                         0.0, y, 0.0, x, y, 0.0, x, 0.0, 0.0, //Back2
                         x, 0.0, z, x, 0.0, 0.0, x, y, z, //Right1
                         x, y, z, x, 0.0, 0.0, x, y, 0.0, //Right2
                         0.0, 0.0, z, 0.0, y, z, 0.0, 0.0, 0.0, //Left1
                         0.0, y, z, 0.0, y, 0.0, 0.0, 0.0, 0.0, //Left2
                         0.0, y, z, x, y, z, 0.0, y, 0.0, //Top1
                         0.0, y, 0.0, x, y, z, x, y, 0.0, //Top2
                         0.0, 0.0, z, 0.0, 0.0, 0.0, x, 0.0, z, //Bot1
                         0.0, 0.0, 0.0, x, 0.0, 0.0, x, 0.0, z, //Bot2
                       };*/

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



enum TransformStates {TRANSLATE, ROTATE, SCALE, SCALEALL, NONE };

void getState(char *s, int *state, int *index) {
    if(strcmp(s, "tx") == 0) {
        *state = TRANSLATE;
        *index = 0;
    } else if(strcmp(s, "ty") == 0) {
        *state = TRANSLATE;
        *index = 1;
    } else if(strcmp(s, "tz") == 0) {
        *state = TRANSLATE;
        *index = 2;
    } else if(strcmp(s, "rx") == 0) {
        *state = ROTATE;
        *index = 0;
    } else if(strcmp(s, "ry") == 0) {
        *state = ROTATE;
        *index = 1;
    } else if(strcmp(s, "rz") == 0) {
        *state = ROTATE;
        *index = 2;
    } else if(strcmp(s, "sa") == 0) {
        *state = SCALEALL;
        *index = 0;
    } else if(strcmp(s, "s") == 0) {
        *state = SCALE;
        *index = 0;
    }
}

void parseTransform(char *str, mat4 t) {
    printf("--parseTransform str: %s\n", str);
    vec3 trs[3] = { {0.0f} };
    //inicializa escala com {1, 1, 1}
    trs[2][0] = trs[2][1] = trs[2][2] = 1.0;
    char *tok;
    tok = strtok(str, " ");
    int state = NONE;
    int index;
    while(tok) {
        printf("%s\n", tok);
        //if(state < SCALEALL) { //Translate or rotate
        //printf("state: %d\n", state);
        if((state == TRANSLATE) || (state == ROTATE)) {
            printf("translate or rotate\n");
            float value = atof(tok);
            trs[state][index] = value;
            state = NONE;
        } else if (state == SCALE) {
            printf("scale\n");
            trs[2][0] = atof(tok);
            tok = strtok(NULL, " ");
            trs[2][1] = atof(tok);
            tok = strtok(NULL, " ");
            trs[2][2] = atof(tok);
            state = NONE;
        } else if (state == SCALEALL) {
            printf("scale all\n");
            float value = atof(tok);
            trs[2][0] = trs[2][1] = trs[2][2] = value;    
            state = NONE;
        } else //NONE
            getState(tok, &state, &index);
        tok = strtok(NULL, " ");
    }
    mat4 translate, rotatex, rotatey, rotatez, scale;
    Identity(translate); 
    Identity(rotatex); 
    Identity(rotatey); 
    Identity(rotatez); 
    Identity(scale);
    Translatef(translate, trs[0][0], trs[0][1], trs[0][2]);
    Rotatef(rotatex, DegToRad(trs[1][0]), 1.0, 0.0, 0.0);
    Rotatef(rotatey, DegToRad(trs[1][1]), 0.0, 1.0, 0.0);
    Rotatef(rotatez, DegToRad(trs[1][2]), 0.0, 0.0, 1.0);
    Scalef(scale, trs[2][0], trs[2][1], trs[2][2]);
    
    Identity(t);
    Multm(t, scale, t);
    Multm(t, rotatez, t); 
    Multm(t, rotatey, t); 
    Multm(t, rotatex, t); 
    Multm(t, translate, t); 

    printf("translate: (%.1f, %.1f, %.1f)\n", trs[0][0], trs[0][1], trs[0][2]);
    printf("rotate: (%.1f, %.1f, %.1f)\n", trs[1][0], trs[1][1], trs[1][2]);
    printf("scale: (%.1f, %.1f, %.1f)\n", trs[2][0], trs[2][1], trs[2][2]);
}


void addRuleName(char **ruleid, int n, char *name) {
    int i;
    for(i = 0; (ruleid[i] != NULL) && (i < n); i++)
        if(strcmp(ruleid[i], name) == 0)
            return;
    ruleid[i] = name;
}

int getRuleId(char **ruleid, int n, char *name) {
    for(int i = 0; i < n; i++) 
        if(strcmp(ruleid[i], name) == 0) 
            return i;
    return -1;
}

MeshGrammar *loadFromFile(const char *path) {
    ezxml_t gxml = ezxml_parse_file(path);
    if (!gxml){
		printf("File not found: %s \n", path);
		return NULL;
	}

    MeshGrammar *g = malloc(sizeof(MeshGrammar));
    //Contar quantas rules diferentes da tag tem e associar o nome a um int
    int rulescount = 0;
	for (ezxml_t rtag = ezxml_child(gxml, "rule" ); rtag; rtag = rtag->next ){
        if(strcmp(ezxml_attr(rtag, "name"), "entry") != 0) {
            rulescount++;
        }
    }

    char *ruleid[rulescount];
    for(int i = 0; i < rulescount; i++)
        ruleid[i] = NULL;

	for (ezxml_t rtag = ezxml_child(gxml, "rule" ); rtag; rtag = rtag->next){
        char *rname = (char *)ezxml_attr(rtag, "name");
        if(strcmp(rname, "entry") != 0)
            addRuleName(ruleid, rulescount, rname); 
    }

    int newcount = 0;
    for(int i = 0; i < rulescount; i++)
        if(ruleid[i] != NULL)
            newcount++;

    g->rcount = newcount;
    g->rlist = malloc(sizeof(Rule*)*newcount);
    for(int i = 0; i < newcount; i++)
        g->rlist[i] = NULL;

    for (ezxml_t rtag = ezxml_child(gxml, "rule" ); rtag; rtag = rtag->next){
        Rule *r = malloc(sizeof(Rule));
        
        char *rname = (char *)ezxml_attr(rtag, "name");
        if(strcmp(rname, "entry") == 0) {
            //TODO não usar ponteiro para entry, apenas escrever em g->entry
            g->entry = r;
        } else { 
            //colocar a regra no lugar correto na lista
            int id = getRuleId(ruleid, newcount, rname);
            if(id != -1)
                DL_APPEND(g->rlist[id], r);
        }
        
        int callcount = 0;
        for (ezxml_t ctag = ezxml_child(rtag, "call"); ctag; ctag = ctag->next){
            callcount++;
        }
        r->ncalls = callcount;
        r->cid = malloc(sizeof(int)*callcount);
        r->ctransform = malloc(sizeof(mat4)*callcount);
        r->ccount = malloc(sizeof(int)*callcount);
        int j = 0;
        for (ezxml_t ctag = ezxml_child(rtag, "call"); ctag; ctag = ctag->next, j++){
            char *crname = (char*)ezxml_attr(ctag, "rule"); 
            r->cid[j] = getRuleId(ruleid, newcount, crname);
            char *tstr = (char *)ezxml_attr(ctag, "transforms");
            parseTransform(tstr, r->ctransform[j]);
            char *countstr = (char *) ezxml_attr(ctag, "count");
            if(countstr)
                r->ccount[j] = atoi(countstr);
            else
                r->ccount[j] = 1;
        }
        
        //FIXME: 1. ler tipo da instance
        //       2. tem mais de 1 instance?
        ezxml_t itag = ezxml_child(rtag, "instance");
        if(itag) {
            r->instance = CUBE;
            //Ler transformação?
            char *itstr = (char *)ezxml_attr(itag, "transforms");
            if(itstr)
                parseTransform(itstr, r->itransform);
            else
                Identity(r->itransform);

        } else
            r->instance = EMPTY;

        char *wstr = (char *)ezxml_attr(rtag, "weight");
        if(wstr)
            r->weight = atof(wstr);
        else
            r->weight = 1.0;
        //DEBUG:
        //printf("rname: %s\n", rname);
        //printf("r->ncalls = %d\n", r->ncalls);
        //for(int i = 0; i < r->ncalls; i++)
        //    printf("\t<call ruleid: %d>\n", r->cid[i]);
        //printf("r->instance = %d\n", r->instance);
    }
    //normaliza os weights
    for(int i = 0; i < g->rcount; i++) {
        float wsum = 0.0;
        Rule *it;
        DL_FOREACH(g->rlist[i], it) {
            wsum += it->weight;
        }
        DL_FOREACH(g->rlist[i], it) {
            it->weight /= wsum;
        }
    }

    ezxml_free(gxml);
    return g;    
}

Rule *pickRule(MeshGrammar *g, int i) {
    //Gera numero aleatório r [0.0, 1.0]
    float r = (float)rand()/(float)RAND_MAX;

    Rule *list = g->rlist[i];
    Rule *it;
    float weights = 0.0;
    DL_FOREACH(list, it) {
        if((r >= weights) && (r <= (weights + it->weight)))
            return it;
        weights += it->weight;
    }
    return NULL;
}

//DEBUG
void printMat(mat4 m) {
    for(int i = 0; i < 4; i++) {
        printf("%.1f %.1f %.1f %.1f\n", m[i], m[i + 4], m[i + 8], m[i + 12]);
    }
    printf("\n\n\n\n");
}

void processRule(Rule *r, mat4 t, MeshGrammar *g, Node *parent, int depth) {
    if(depth == 0)
        return;
    
    Node *n = malloc(sizeof(Node));
    memcpy(n->transform, t, sizeof(mat4));
    n->parent = parent;
    DL_APPEND(parent->children, n);
    n->children = NULL;
    n->material = material;
    n->mesh = NULL;

    //Criar um node e usar a transformação
    if(r->instance == CUBE) {
        Mesh *m = createBox(1.0f, 1.0f, 1.0f);
        n->mesh = m;
        Multm(n->transform, n->transform, r->itransform);
    }

    for(int i = 0; i < r->ncalls; i++) {
        mat4 m;
        Identity(m);
        for(int j = 0; j < r->ccount[i]; j++) {
            Rule *newr = pickRule(g, r->cid[i]);
            Multm(m, r->ctransform[i], m);
            processRule(newr, m, g, n, depth - 1);
        }
    }
}
