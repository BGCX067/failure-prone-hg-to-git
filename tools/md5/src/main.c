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

void initializeGame(){
    initCamera(&c, TRACKBALL);

    cena = initializeScene();

    //md5mesh = readMD5Mesh("data/models/zfat.md5mesh");
    md5mesh = readMD5Mesh("data/models/boblampclean.md5mesh");
    SkeletalAnim *anim1 = readMD5Anim("data/models/boblampclean.md5anim");
    //SkeletalAnim *anim1 = readMD5Anim("data/models/walk1.md5anim");
    //SkeletalAnim *anim2 = readMD5Anim("data/models/walk2.md5anim");
    //SkeletalAnim *anim3 = readMD5Anim("data/models/walk3.md5anim");
    //SkeletalAnim *anim4 = readMD5Anim("data/models/walk4.md5anim");
    //SkeletalAnim *anim2 = readMD5Anim("data/models/pipeattack1.md5anim");
    addAnim(md5mesh, anim1);
    //addAnim(md5mesh, anim2);
    //addAnim(md5mesh, anim3);
    //addAnim(md5mesh, anim4);
    addMesh(cena, md5mesh);
    md5mesh->currAnim = 0;
    
    //Inicializar animinfo
    animinfo.curr_frame = 0;
    animinfo.next_frame = 1;

    animinfo.last_time = 0;
    animinfo.max_time = 1.0/anim1->frameRate;

    skeleton = malloc(sizeof(Joint)*anim1->numJoints);

    char *vertshader = readTextFile("data/shaders/vertshader.vert");
    char *fragshader = readTextFile("data/shaders/fragshader.frag");
    shdr = initializeShader(vertshader, fragshader); 


    memset(cena->b.pmin, 0, 3*sizeof(float));
    memset(cena->b.pmax, 0, 3*sizeof(float));
    
    //Calcula bounding box da cena
    for(int i = 0; i < cena->meshList->size; i++) {
	    Mesh *m = fplist_getdata(i, cena->meshList);
        for(int j = 0; j < m->tris->size; j++) {
            Triangles *t = fplist_getdata(j, m->tris); 
            bbunion(&m->b, m->b, t->b);
        }
        bbunion(&cena->b, cena->b, m->b);
    }
    
    camerafit(&c, cena->b, 45.0, 800/600, 0.1, 1000.0);

    Light *l = malloc(sizeof(Light));
    l->pos[0] = 0.0;
    l->pos[1] = 300.0;
    l->pos[2] = 100.0;
    l->color[0] = 1.0;
    l->color[1] = 1.0;
    l->color[2] = 1.0;
    l->color[3] = 1.0;

    addLight(cena, l);
    glPolygonMode(GL_BACK, GL_LINE);
}

int Update(event* e, double *dt){
    //Animate(&animinfo, anim->numFrames - 1, ifps);
    SkeletalAnim *anim = getCurrentAnim(md5mesh);
    animinfo.last_time += *dt;
    /* move to next frame */
    if (animinfo.last_time >= animinfo.max_time){
        animinfo.curr_frame++;
        animinfo.next_frame++;
        animinfo.last_time = 0.0;

        if (animinfo.curr_frame > anim->numFrames - 1) {
            //md5mesh->currAnim = (md5mesh->currAnim + 1)%4;
            md5mesh->currAnim = md5mesh->currAnim;
            animinfo.curr_frame = 0;
            animinfo.max_time = 1.0/anim->frameRate;
        }
        if (animinfo.next_frame > anim->numFrames - 1)
            animinfo.next_frame = 0;
    }

    anim = getCurrentAnim(md5mesh);

    interpolateSkeletons(anim->skelFrames[animinfo.curr_frame], anim->skelFrames[animinfo.next_frame],
                         anim->numJoints, animinfo.last_time*anim->frameRate, skeleton);
    updateMesh(md5mesh, skeleton);

    //handle camera events
    cameraHandleEvent(&c, e);
    //setup matrixes
    setupViewMatrix(&c);
    vec3 bboxcenter;
    bbcenter(cena->b, bboxcenter);
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    
    setView(c.modelview);
    setProjection(c.projection); //TODO isso so precisaria ser calculado/setado 1x
    setShaderConstant3f(shdr, "eyepos", c.pos);
	return 1;
}



int Render(event *e, double *dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
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
}


int main(){
    setVideoMode(800, 600, 0);
	warpmouse( 0);
	setWindowTitle("Hello World");
	initializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop();
	closeVideo();
	return 0;
}



