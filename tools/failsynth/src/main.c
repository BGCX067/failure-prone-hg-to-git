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
    int depth;
}MeshGrammar;

Scene* cena;
Camera c;
Shader *material;


//Coordinate axis
Shader *colorshdr;
Batch *coordaxis;

int drawCenterOfRoation = 1;

void createCoordAxis();

//TODO:
//0. max_depth por cada função
//1. More shapes
//4. camera functions for better visualization
MeshGrammar *loadFromFile(const char *path);
Scene *GenScnSynth(MeshGrammar *g);

void initializeGame(){
    srand(time(0));
    //CamInit(&c, GetScreenW(), GetScreenH(), FPS, PERSPECTIVE); 
    CamInit(&c, GetScreenW(), GetScreenH(), TRACKBALL, PERSPECTIVE); 
    SetZfar(&c, 1000.0);
    SetProjection(c.mprojection);
    //Setvf(c.pos, 0.0, 0.0, 3.0);
    c.zoom = 100.0;

    createCoordAxis();

    Light l; 
    l.pos[0]= 2.0; l.pos[1] = 1.92; l.pos[2] = 2.0;
    l.color[0] = 1.0; l.color[1] = 1.0; l.color[2] = 1.0; l.color[3] = 1.0;

    vec3 khakiAmb = {0.125, 0.1, 0.072};
    vec3 khakiDiff = {0.5, 0.41, 0.275};
    vec3 khakiSpec = {0.125, 0.125, 0.125};
    float khakiShininess = 12.5;
    material = PhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color); 

    MeshGrammar *g = loadFromFile("data/spiral.xml");
    //MeshGrammar *g = loadFromFile("data/2.xml");
    //MeshGrammar* g = loadFromFile("data/3.xml");
    cena = GenScnSynth(g);
}

int Render(event *e, double* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    DrawScene(cena);

    //Draw the coordinate axis
    glDisable(GL_DEPTH_TEST);
    if(drawCenterOfRoation) {
        Translatef(c.mview, c.pivot[0], c.pivot[1], c.pivot[2]);
        SetView(c.mview);
        BindShader(colorshdr);
        Draw(coordaxis);
    }
    glEnable(GL_DEPTH_TEST);

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
	InitializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop();
	CloseVideo();
	return 0;
}

void parseTransform(char *str, mat4 t) {
    printf("--parseTransform str: %s\n", str);
    Identity(t);

    for(char *tok = strtok(str, " "); tok; tok = strtok(NULL, " ")) {
        mat4 currt;
        Identity(currt);
        if(strcmp(tok, "tx") == 0) {
            tok = strtok(NULL, " ");
            Translatef(currt, atof(tok), 0.0, 0.0);
        } else if (strcmp(tok, "ty") == 0) {
            tok = strtok(NULL, " ");
            Translatef(currt, 0.0, atof(tok), 0.0);
        } else if (strcmp(tok, "tz") == 0) {
            tok = strtok(NULL, " ");
            Translatef(currt, 0.0, 0.0, atof(tok));
        } else if (strcmp(tok, "rx") == 0) {
            tok = strtok(NULL, " ");
            Rotatef(currt, DegToRad(atof(tok)), 1.0, 0.0, 0.0);
        } else if (strcmp(tok, "ry") == 0) {
            tok = strtok(NULL, " ");
            Rotatef(currt, DegToRad(atof(tok)), 0.0, 1.0, 0.0);
        } else if (strcmp(tok, "rz") == 0) {
            tok = strtok(NULL, " ");
            Rotatef(currt, DegToRad(atof(tok)), 0.0, 0.0, 1.0);
        } else if (strcmp(tok, "sx") == 0) {
            tok = strtok(NULL, " ");
            Scalef(currt, atof(tok), 1.0, 1.0);
        } else if (strcmp(tok, "sy") == 0) {
            tok = strtok(NULL, " ");
            Scalef(currt, 1.0, atof(tok), 1.0);
        } else if (strcmp(tok, "sz") == 0) {
            tok = strtok(NULL, " ");
            Scalef(currt, 1.0, 1.0, atof(tok));
        } else if (strcmp(tok, "sa") == 0) {
            tok = strtok(NULL, " ");
            float s = atof(tok);
            Scalef(currt, s, s, s);
        } else if (strcmp(tok, "s") == 0) {
            tok = strtok(NULL, " ");
            float sx = atof(tok);
            tok = strtok(NULL, " ");
            float sy = atof(tok);
            tok = strtok(NULL, " ");
            float sz = atof(tok);
            Scalef(currt, sx, sy, sz);
        } else {
            printf("!!ERROR!!\n");
        }

        Multm(t, t, currt);
    }
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
    g->depth = atof(ezxml_attr(gxml, "max_depth"));
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
            if(tstr)
                parseTransform(tstr, r->ctransform[j]);
            else
                Identity(r->ctransform[j]);
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

//FIXME usar API e parar de instanciar e inicializar coisas na mão
void processRule(Rule *r, mat4 t, MeshGrammar *g, Node *parent, int depth) {
    if(depth == 0)
        return;
    
    Node *n = malloc(sizeof(Node));
    memcpy(n->transform, t, sizeof(mat4));
    Identity(n->ltransform);
    n->parent = parent;
    DL_APPEND(parent->children, n);
    n->children = NULL;
    n->material = material;
    n->mesh = NULL;

    //Criar um node e usar a transformação
    if(r->instance == CUBE) {
        Mesh *m = CreateBox(1.0f, 1.0f, 1.0f);
        n->mesh = m;
        mat4 tr;
        Identity(tr);
        Translatef(tr, 0.5, 0.5, 0.5);
        Multm(tr, tr, r->itransform);
        Multm(n->ltransform, tr, n->ltransform);
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

Scene *GenScnSynth(MeshGrammar *g) {
    Scene *s = InitializeScene();

    mat4 m;
    Identity(m);
    processRule(g->entry, m, g, s->root, g->depth);
    return s;
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
