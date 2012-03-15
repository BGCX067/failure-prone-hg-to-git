#include <stdio.h>
#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/glime.h"
#include "renderer/camera.h"
#include "util/shadergen.h"
#include "util/colladaloader.h"
#include "md5model.h"

//Animation info
typedef struct {
    int curr_frame;
    int next_frame;

    double last_time;
    double max_time;
}anim_info_t;

Scene* cena;
Shader *shdr;
renderer *mainrenderer;
Camera c;
BoundingBox bbox;
anim_info_t animinfo;
Mesh *md5mesh;
Joint *skeleton;
int animated = 1;

/**
 * Perform animation related computations.  Calculate the current and
 * next frames, given a delta time.
 */
void Animate(anim_info_t *animInfo, int maxFrames, double dt) {
    animInfo->last_time += dt;
    /* move to next frame */
    if (animInfo->last_time >= animInfo->max_time){
        animInfo->curr_frame++;
        animInfo->next_frame++;
        animInfo->last_time = 0.0;

        if (animInfo->curr_frame > maxFrames)
            animInfo->curr_frame = 0;
        if (animInfo->next_frame > maxFrames)
            animInfo->next_frame = 0;
    }
}

int idle(float ifps, event* e, Scene* s){
    Animate(&animinfo, md5mesh->anim->numFrames - 1, ifps);

    interpolateSkeletons(md5mesh->anim->skelFrames[animinfo.curr_frame], md5mesh->anim->skelFrames[animinfo.next_frame],
                         md5mesh->anim->numJoints, animinfo.last_time*md5mesh->anim->frameRate, skeleton);
    updateMesh(md5mesh, skeleton);
	return 1;
}

void initializeGame(){
    initCamera(&c, TRACKBALL);

    //Carrega a cena do collada temporariamente, apenas para ter luz e material na cena
    cena = readColladaFile("../../data/models/duck_triangulate_deindexer.dae");
    //remove o mesh do pato
    fplist_rmback(cena->meshList);

    md5mesh = readMD5Mesh("data/models/boblampclean.md5mesh");
    md5mesh->anim = readMD5Anim("data/models/boblampclean.md5anim");
    addMesh(cena, md5mesh);
    
    //Inicializar animinfo
    animinfo.curr_frame = 0;
    animinfo.next_frame = 1;

    animinfo.last_time = 0;
    animinfo.max_time = 1.0/md5mesh->anim->frameRate;

    skeleton = malloc(sizeof(Joint)*md5mesh->anim->numJoints);

    //FIXME pra testar, faz os meshes do md5 terem o mesmo material do pato
    for(int j = 0; j < md5mesh->tris->size; j++) {
        Triangles *tri = fplist_getdata(j, md5mesh->tris);
        tri->material = cena->materialList->first->data;
    }
    char *vertshader = readTextFile("data/shaders/vertshader.vert");
    char *fragshader = readTextFile("data/shaders/fragshader.frag");
    shdr = initializeShader(vertshader, fragshader); 


    memset(cena->b.pmin, 0, 3*sizeof(float));
    memset(cena->b.pmax, 0, 3*sizeof(float));
    
    //Calcula bounding box da cena
/*    for(int i = 0; i < cena->meshList->size; i++) {
	    Mesh *m = fplist_getdata(i, cena->meshList);
        for(int j = 0; j < m->tris->size; j++) {
            Triangles *t = fplist_getdata(j, m->tris); 
            bbunion(&m->b, m->b, t->b);
        }
        bbunion(&cena->b, cena->b, m->b);
    }*/
    
    cena->b.pmin[0] = -43.0;  cena->b.pmin[1] = -12.0; cena->b.pmin[2] = 0.0;
    cena->b.pmax[0] = 42.0;  cena->b.pmax[1] = 13.0; cena->b.pmax[2] = 67;

    camerafit(&c, cena->b, 45.0, 800/600, 0.1, 1000.0);
}

int render(float ifps, event *e, Scene *cena){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //handle camera events
    cameraHandleEvent(&c, e);
    //setup matrixes
    setupViewMatrix(&c);

    vec3 bboxcenter;
    bbcenter(cena->b, bboxcenter);

    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    fpMultMatrix(c.mvp, c.projection, c.modelview);
    setShaderConstant4x4f(shdr, "mvp", c.mvp);
    setShaderConstant4x4f(shdr, "modelview", c.modelview);
    setShaderConstant3x3f(shdr, "normalmatrix", c.normalmatrix);
    setShaderConstant3f(shdr, "eyepos", c.pos);


    //Passa informações da luz pro shader
    Light *l = fplist_getdata(0, cena->lightList);
    setShaderConstant3f(shdr, "lightpos", l->pos);
    setShaderConstant3f(shdr, "lightintensity", l->color);

    for(int i = 0; i < cena->meshList->size; i++) {
	    Mesh *m = fplist_getdata(i, cena->meshList);
        for(int j = 0; j < m->tris->size; j++) {
            Triangles *tri = fplist_getdata(j, m->tris);
            Material *mat = tri->material;
            //Passsa material pro shader
            setShaderConstant3f(shdr, "ka", mat->ka);
            setShaderConstant3f(shdr, "ks", mat->ks);
            setShaderConstant1f(shdr, "shininess", mat->shininess);
            
            if(mat->diffsource == TEXTURE) {
                bindSamplerState(mat->diffmap->state, 0);
                bindTexture(mat->diffmap, 0);
            }
            bindShader(shdr);
            drawIndexedVAO(tri->vaoId, tri->indicesCount, GL_TRIANGLES);
        }
    }
    
    glFlush();
    swapBuffers();
}


int main(){
	glapp* app = setVideoMode(800, 600, 0);
	if (!app){
		printf("Invalid Video Mode\n");
		return 1;
	}else{
		printf("Video mode: w:  %d h: %d depth: %d \n", app->width, app->height, app->depth);
	}
	warpmouse( app, 0);
	setWindowTitle("Mathfeel");
	mainrenderer  = initializeRenderer(app->width, app->height, 0.1, 10000.0, 45.0, TRACKBALL);
	initializeGame();
	mainloop(app, idle, render, cena );

	closeVideo();
	free(app);
	return 0;
}



