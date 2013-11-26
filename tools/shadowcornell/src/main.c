#include <stdio.h>
#include <stdlib.h>
#include "glapp.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/camera.h"
#include "renderer/gui.h"
#include "renderer/material.h"
#include "util/utlist.h"
#include "util/textfile.h"
#include <string.h>

Scene* cena;
Renderer *mainrenderer;
Camera c;
Light l;

Shader *shdr;
//Shadow-related
Framebuffer *shadowfb;
Shader *firstPassShader;

void drawFullscreenQuad(Texture *tex, Shader *shdr); 
void initializeShadowmap(unsigned int fbwidth, unsigned int fbheight); 
void renderShadowmap(Node *casters, Node *recievers, vec3 lightPos, vec3 lightDir);
void shadowFirstPass(Framebuffer *sfb, Node *casters, mat4 lightView, mat4 lightProj);
void shadowSecondPass(Framebuffer *sfb, Node *recievers, mat4 lightMVP);

typedef struct _NodeIterator {
    Node *n;
    mat4 transform;
    struct _NodeIterator *next;
} NodeIterator;

void NodeTraverseDepth(Node *root);
NodeIterator *LinearizeNodes(Node *root);
NodeIterator *newIterator(Node* n, mat4 t);

void initializeGame(){
    CamInit(&c, GetScreenW(), GetScreenH(), TRACKBALL, PERSPECTIVE); 
    SetProjection(c.mprojection);
    c.zoom = 5.0;

    cena = InitializeScene();

    l.pos[0]= 0.0; l.pos[1] = 0.92; l.pos[2] = 0.0;
    l.dir[0] = 0.0; l.dir[1] = -1.0; l.dir[2] = 0.0;
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
    leftWallNode->material = ShadowPhongMaterial(halveRedAmb, halveRedDiff, halveRedSpec, halveRedShininess, l.pos, l.color);
    Translatef(leftWallNode->ltransform, -0.975, 0.0, 0.025);

    Mesh *rightWall = CreateBox(0.05, 1.95, 1.95);
    Node *rightWallNode = AddMesh(cena, rightWall);
    rightWallNode->material = ShadowPhongMaterial(darkGreenAmb, darkGreenDiff, darkGreenSpec, darkGreenShininess, l.pos, l.color); 
    Translatef(rightWallNode->ltransform, 0.975, 0.0, 0.025);

    Mesh *backWall = CreateBox(2.0, 1.95, 0.05);
    Node *backWallNode = AddMesh(cena, backWall);
    backWallNode->material = ShadowPhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);
    Translatef(backWallNode->ltransform, 0.0, 0.0, -0.975);


    Mesh *floor = CreateBox(2.0, 0.05, 2.0);
    Node *floorNode = AddMesh(cena, floor);
    floorNode->material = ShadowPhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);
    Translatef(floorNode->ltransform, 0.0, -1.0, 0.0);

    Mesh *ceiling = CreateBox(2.0, 0.05, 2.0);
    Node *ceilingNode = AddMesh(cena, ceiling);
    ceilingNode->material = ShadowPhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);
    Translatef(ceilingNode->ltransform, 0.0, 1.0, 0.0);
    
    Mesh *tallBox = CreateBox(0.7, 1.16, 0.7);
    Node *tallBoxNode = AddMesh(cena, tallBox);
    tallBoxNode->material = ShadowPhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color);
    Translatef(tallBoxNode->ltransform, -0.20, -(1.0 - 0.58), -0.5);
    Rotatef(tallBoxNode->transform, 3.14159265358979323846/6.0, 0.0, 1.0, 0.0);

    Mesh *shortBox = CreateBox(0.7, 0.56, 0.7);
    Node *shortBoxNode = AddMesh(cena, shortBox);
    shortBoxNode->material = ShadowPhongMaterial(khakiAmb, khakiDiff, khakiSpec, khakiShininess, l.pos, l.color); 
    Translatef(shortBoxNode->ltransform, 0.4, -(1.0 - 0.28), 0.3);
    Rotatef(shortBoxNode->ltransform, -3.14159/12.0, 0.0, 1.0, 0.0);
        
    initializeShadowmap(256, 256);

    //char *vertshader = readTextFile("data/shaders/vertshader.vert");
    //char *fragshader = readTextFile("data/shaders/fragshader.frag");
    //shdr = initializeShader(vertshader, fragshader);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}


void UpdateStack(NodeIterator **stack, NodeIterator **it, int *size) {
    NodeIterator *it2;
    int count;
    LL_COUNT(*stack, it2, count);
    printf("\tsize antes: %d, count antes: %d\n", *size, count);
    //NodeIterator *i = *it;
    //NodeIterator *s = *stack;

    LL_DELETE(*stack, *it);
    (*size)--;
    Node *child;
    DL_FOREACH((*it)->n->children, child) {
        printf("\tadicionando no filho\n");
        mat4 tres;
        Multm(tres, (*it)->transform, (*it)->n->transform);
        NodeIterator *newit = newIterator(child, tres);
        LL_PREPEND(*stack, newit);
        (*size)++;
    }
    LL_COUNT(*stack, it2, count);
    printf("\tsize depois: %d, count depois: %d\n", *size, count);
    /*if(!(*it)) {
        printf("*it eh null\n");
        return;
    } else
        printf("*it nao eh NULL\n");

    if((*it)->n == NULL) {
        printf("tem iterador, mas nao tem node\n");
        return;
    } else {
        printf("tem ate node\n");
    }*/
    (*it) = (*stack);
    if(*it == NULL)
        return;
    printf("\tstack: %p, it: %p\n", stack, it);
    printf("\t*stack: %p, *it: %p\n", *stack, *it);
    printf("\tatualizando o no atual\n");
    mat4 lres;
    printf("\tcalculando a transformação resultante\n");
    Multm(lres, (*it)->n->transform, (*it)->n->ltransform);
    Multm(lres, (*it)->transform, lres); 
    memcpy((*it)->transform, lres, sizeof(mat4));
    printf("fim\n");
}


int Render(event *e, double* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    printf("comeco de Render()\n");
    
    static int k = 0;
    
    //renderShadowmap(cena->root->children, cena->root->children, l.pos, l.dir);

    //DrawScene(cena);
    //NodeTraverseDepth(cena->root);
    if(k == 0) {
    mat4 i;
    Identity(i);
    /*NodeIterator *nlist = LinearizeNodes(cena->root);
    NodeIterator *it;
    LL_FOREACH(nlist, it) {
        SetModel(it->transform);
        if(it->n->material)
            BindShader(it->n->material);

        if(it->n->mesh)
            DrawIndexedVAO(it->n->mesh->vaoId, it->n->mesh->indicesCount, GL_TRIANGLES);
    }*/

    NodeIterator *stack = NULL;
    LL_APPEND(stack, newIterator(cena->root, i));
    //NodeIterator *it = stack;
    //for(int size = 1, i = 0; size > 0; UpdateStack(&stack, &it, &size), i++) {
    int size =1;
    for(NodeIterator *it = stack; it; UpdateStack(&stack, &it, &size)) {
        //printf("i: %d\n", i);
        SetModel(it->transform);
        if(it->n->material)
            BindShader(it->n->material);
        if(it->n->mesh) {
            printf("\tdesenha alguma coisa de fato\n");
            DrawIndexedVAO(it->n->mesh->vaoId, it->n->mesh->indicesCount, GL_TRIANGLES);
        }
    }
    //k++;
    }

    //DrawScene(cena);
    
    
    printf("fim de Render()\n");
    //drawFullscreenQuad(shadowfb->tex, shdr);
    glFlush();
    return 1;
}

int Update(event* e, double *dt){
    c.update(&c, e, dt);
    SetView(c.mview);
    printf("\n\nUpdate()\n\n");
    return 1;
}

int main(){
	SetVideoMode(800, 600, 0);
	WarpMouse(0);
	SetWindowTitle("Cornell Box");
	mainrenderer = InitializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop();
	CloseVideo();
	return 0;
}

void initializeShadowmap(unsigned int fbwidth, unsigned int fbheight) {
    shadowfb = InitializeFramebufferDepth(fbwidth, fbheight);
    firstPassShader = InitializeShader(NULL, ReadTextFile("data/shaders/shadowfirstpass.vert"), 
                            ReadTextFile("data/shaders/shadowfirstpass.frag"));
}

void renderShadowmap(Node *casters, Node *receivers, vec3 lightPos, vec3 lightDir) {
    mat4 lightView, lightProj, lightMVP;
    //TODO calcular vetor up?
    vec3 lightUp = {1.0f, 1.0f, 1.0f}; 
    LookAt(lightView, lightPos, lightDir, lightUp);
    //TODO near e far melhores
    Perspective(lightProj, 100.0f, (float)shadowfb->width/(float)shadowfb->height, 0.1f, 10.0f); 
    //fpOrtho(lightProj, 4.0f, -4.0f, -10.0f, 10.0f, 0.1f, 7.0f);
    shadowFirstPass(shadowfb, casters, lightView, lightProj);
    Multm(lightMVP, lightProj, lightView);
    glFlush();
    shadowSecondPass(shadowfb, receivers, lightMVP);
}

void shadowFirstPass(Framebuffer *sfb, Node *casters, mat4 lightView, mat4 lightProj) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    BindFramebuffer(sfb);
    SetView(lightView);
    SetProjection(lightProj);
    
    Node *n;
    //TODO considerar passar outra matriz além da identidade
    mat4 m;
    Identity(m);
    DL_FOREACH(casters, n) {
        renderNodesFirstPass(n, firstPassShader, m);
    }

    BindFramebuffer(NULL);
    glCullFace(GL_BACK);   
}

void shadowSecondPass(Framebuffer *sfb, Node *recievers, mat4 lightMVP) {
    SetView(c.mview);
    SetProjection(c.mprojection);
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //shadow matrix S = bias*lproj*lview*model;
    mat4 bias, bpv;
    Identity(bias);
    bias[0] = bias[5] = bias[10] = bias[12] = bias[13] = bias[14] = 0.5;
    Multm(bpv, bias, lightMVP);

    Node *n;
    mat4 m;
    Identity(m);
    DL_FOREACH(recievers, n) {
        renderNodesSecondPass(n, bpv, m);
    }
    /*Mesh *m;
    DL_FOREACH(recievers, m) {
        //SetModel(m->transform);
        //BindMaterial(m->material, &l);
        mat4 shadowMatrix;
        //MultMatrix(shadowMatrix, bpv, m->transform);
        //SetShaderConstant4x4f(m->material->shdr, "shadowMatrix", shadowMatrix);
        BindSamplerState(sfb->tex->state, 0);
        BindTexture(sfb->tex, 0);
        //BindShader(m->material->shdr);
        DrawIndexedVAO(m->vaoId, m->indicesCount, GL_TRIANGLES);
    }*/
}

void drawFullscreenQuad(Texture *tex, Shader *shdr) {
    static Mesh *m = NULL;
    if(!m) {
        printf("inicializando quad\n");
        m = InitMesh();
        float vertices[12] = {-1.0, -1.0, 0.0,
                               1.0, -1.0, 0.0,
                               1.0, 1.0, 0.0,
                              -1.0, 1.0, 0.0};
        float texcoords[8] = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0};
        unsigned int indices[6] = {0, 1, 2, 0, 2, 3};
        AddVertices(m, 12, 3, vertices);
        AddIndices(m, 6, indices);
        AddTexCoords(m, 8, 2, 0, texcoords);
        PrepareMesh(m);   
    }
    BindSamplerState(tex->state, 0);
    BindTexture(tex, 0);
    BindShader(shdr);
    DrawIndexedVAO(m->vaoId, m->indicesCount, GL_TRIANGLES);
}

void renderNodesFirstPass(Node *n, Shader *material, mat4 t) {
    mat4 tres, lres;
    Multm(tres, t, n->transform);
    Multm(lres, n->transform, n->ltransform);
    Multm(lres, t, lres);
    SetModel(lres);
    BindShader(material);
    if(n->mesh)
        DrawIndexedVAO(n->mesh->vaoId, n->mesh->indicesCount, GL_TRIANGLES);
    Node *it;
    DL_FOREACH(n->children, it) {
        renderNodesFirstPass(it, material, tres);
    }
}

void renderNodesSecondPass(Node *n, mat4 bpv, mat4 t) {
    mat4 tres, lres;
    Multm(tres, t, n->transform);
    Multm(lres, n->transform, n->ltransform);
    Multm(lres, t, lres);
    SetModel(lres);

    BindShader(n->material);
    mat4 shadowMatrix;
    Multm(shadowMatrix, bpv, lres);
    BindSamplerState(shadowfb->tex->state, 0);
    BindTexture(shadowfb->tex, 0);
    SetShaderConstant(n->material, "shadowMatrix", shadowMatrix);
    if(n->mesh)
        DrawIndexedVAO(n->mesh->vaoId, n->mesh->indicesCount, GL_TRIANGLES);
    Node *it;
    DL_FOREACH(n->children, it) {
        renderNodesSecondPass(it, bpv, tres);
    }
}


NodeIterator *newIterator(Node* n, mat4 t) {
    NodeIterator *it = malloc(sizeof(NodeIterator));
    it->n = n;
    memcpy(it->transform, t, sizeof(mat4));
    return it;
}

void NodeTraverseDepth(Node *root) {
    NodeIterator *stack = NULL;
    
    mat4 ident;
    Identity(ident);
    NodeIterator *rit = newIterator(root, ident);
    LL_APPEND(stack, rit);
    int size = 1;

    while(size > 0) {
        NodeIterator *it = stack;
        LL_DELETE(stack, it);
        size--;
        mat4 lres;
        Multm(lres, it->n->transform, it->n->ltransform);
        Multm(lres, it->transform, lres);
        SetModel(lres);
        if(it->n->material)
            BindShader(it->n->material);

        if(it->n->mesh)
            DrawIndexedVAO(it->n->mesh->vaoId, it->n->mesh->indicesCount, GL_TRIANGLES);
        
        Node *child;
        DL_FOREACH(it->n->children, child) {
            mat4 tres;
            Multm(tres, it->transform, it->n->transform);
            NodeIterator *newit = newIterator(child, tres);
            LL_PREPEND(stack, newit);
            size++;
        }
        free(it);
    }
}

/*void addNodesToStack(NodeIterator *stack, Node *parent) {
    Node *child;
    DL_FOREACH(parent, child) {
        mat4 tres;
        Multm(tres, it->transform, it->n->transform);
        NodeIterator *newit = newIterator(child, tres);
        LL_APPEND(stack, newit);
        size++;
    }
}*/

NodeIterator *LinearizeNodes(Node *root) {
    NodeIterator *stack = NULL;
    NodeIterator *retstack = NULL;
    mat4 i;
    Identity(i);
    NodeIterator *rit = newIterator(root, i);
    LL_APPEND(stack, rit);
    int size = 1;
    while(size > 0) {
        NodeIterator *it = stack;
        LL_DELETE(stack, it);
        size--;
        
        LL_APPEND(retstack, it);

        Node *child;
        DL_FOREACH(it->n->children, child) {
            mat4 tres;
            Multm(tres, it->transform, it->n->transform);
            NodeIterator *newit = newIterator(child, tres);
            LL_PREPEND(stack, newit);
            size++;
        }
        mat4 lres;
        Multm(lres, it->n->transform, it->n->ltransform);
        Multm(lres, it->transform, lres); 
        memcpy(it->transform, lres, sizeof(mat4));
    }
    return retstack;
}

