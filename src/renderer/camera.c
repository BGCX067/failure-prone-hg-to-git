#include "camera.h"
#include <stdio.h>

int initializedtrackball;
vec3 lastpos;
vec3 currpos;

void initCamera(Camera *c, CameraType t) {
    QUAT_IDENTITY(c->orientation);
    VEC3_ZERO(c->pos);
    c->up[0] = 0.0;
    c->up[1] = 1.0;
    c->up[0] = 0.0;

    VEC3_ZERO(c->viewDir);
    c->viewDir[2] = -1;

    fpIdentity(c->modelview);
    fpIdentity(c->projection);

    c->type = t;
    resettrackball();
}

//FIXME fazer um setupViewMatrix de verdade
void setupViewMatrix(Camera *c) {

	if (c->type == FIRSTPERSON){
		fpLookAt(c->modelview, c->pos, c->viewDir, c->up);
	}else if (c->type == TRACKBALL){
		fpIdentity(c->modelview);
		mat4 m, mvcpy;
		quatToMatrix(c->orientation, m);
		m[15] = 1.0;
		fptranslatef(c->modelview, 0.0 /*-c->pos[0]*/, 0.0/*-c->pos[1]*/, -c->pos[2]);
		memcpy(mvcpy, c->modelview, 16*sizeof(float));
		fpMultMatrix(c->modelview, mvcpy, m);
		c->modelview[15] = 1.0;
	}
    fpNormalMatrix(c->normalmatrix, c->modelview);
}


void resettrackball() {
    initializedtrackball = 0;
    VEC3_ZERO(lastpos);
    VEC3_ZERO(currpos);
}

void cameraHandleEvent(Camera *c, event *e) {
    //TODO pegar o tamanho da janela
    static int i = 0;
    if(c->type == FIRSTPERSON) {
        if(e->type & MOUSE_MOTION_EVENT) {
            //FIXME tamanho da janela hardcoded
            float dx = 400 - e->x;
            float dy = 300 - e->y;


            float mouseSensitivity = 0.001;
            float rotationUp = dx*mouseSensitivity;
            float rotationRight = dy*mouseSensitivity;

            if(rotationUp > 0.1)
                rotationUp = 0.1;
            if(rotationRight > 0.1)
                rotationRight = 0.1;

            // Rotação com quaternion e rotacionando os vetores para manter os eixos
            // TODO: tirar os eixos da matriz                                       
            quaternion q1, q2;
            vec3 oldView;
            oldView[0] = c->viewDir[0];
            oldView[1] = c->viewDir[1];
            oldView[2] = c->viewDir[2];
            fromAxisAngle(c->up, rotationUp, q2);
            rotateVec(c->viewDir, q2, c->viewDir);
            vecNormalize(c->viewDir);

            vec3 right;
            cross(c->viewDir, c->up, right);
            //cross(right, c->viewDir, c->up);
            fromAxisAngle(right, rotationRight, q1);
            rotateVec(c->viewDir, q1, c->viewDir);
            vecNormalize(c->viewDir);
            cross(right, oldView, c->up);

            c->up[0] = 0.0;
            c->up[1] = 1.0;
            c->up[2] = 0.0;
            cross(c->viewDir, c->up, right);
            vecNormalize(right);
            cross(right, c->viewDir, c->up);
            vecNormalize(c->up);

            mult(q2, q1, q2);
            mult(q2, c->orientation, c->orientation);
            quatNormalize(c->orientation);
        }
    } else if (c->type == TRACKBALL) {
        if(e->type & MOUSE_MOTION_EVENT && e->buttonLeft ) {
            //printf("c->type == TRACKBALL && e->type & MOUSE_BUTTON_PRESS\n");
            if(!initializedtrackball) {
                //Mapeia de coordenadas de janela para a trackball [-1, 1]
                //FIXME hardcoded o tamanho da janela (800, 600)
                //printf("\tinicializando trackball, escrevendo em lastpos\n");
                float x = e->x/400.0 - 1.0;
                float y = 1.0 - e->y/300.0;
                float z = sqrt(1.0 - x*x - y*y);
                lastpos[0] = x;
                lastpos[1] = y;
                lastpos[2] = z;
                vecNormalize(lastpos);
                initializedtrackball = 1;
            } else {
                //Mapeia de coordenadas de janela para a trackball [-1, 1]
                //FIXME hardcoded o tamanho da janela (800, 600)
                //printf("\ttrackball inicializada, arrastando mouse\n");
                float x = e->x/400.0 - 1.0;
                float y = 1.0 - e->y/300.0;
                float z = sqrt(1.0 - x*x - y*y);
                currpos[0] = x;
                currpos[1] = y;
                currpos[2] = z;
                
                vecNormalize(currpos);
                //eixo de rotação
                vec3 rotaxis;
                cross(currpos, lastpos, rotaxis);
                vecNormalize(rotaxis);
                
                float phi = vecAngle(currpos, lastpos);
                lastpos[0] = currpos[0];
                lastpos[1] = currpos[1];
                lastpos[2] = currpos[2];
                if(phi != 0.0) {
                    //printf("CAMERA - phi: %f\n", phi);
                    quaternion q;
                    fromAxisAngle(rotaxis, phi, q);
                    //printf("CAMERA - c->orientation: %f, %f, %f, %f\n", c->orientation[0], c->orientation[1], c->orientation[2], c->orientation[3]);
                    //mult(q, c->orientation, c->orientation);
                    mult(c->orientation, q, c->orientation);
                    quatNormalize(c->orientation);
                }
            }
        } else if(e->type & MOUSE_BUTTON_RELEASE) {
            //printf("c->type == TRACKBALL && e->type & MOUSE_BUTTON_RELEASE\n");
            initializedtrackball = 0;
            VEC3_ZERO(lastpos);
            VEC3_ZERO(currpos);
        }
    }
    if(e->type & KEYBOARD_EVENT) {
        if(e->keys[KEY_UP]) {
            vec3 dir; 
            vecMult(c->viewDir, 1, dir);
            vecAdd(c->pos, dir, c->pos);
        }
        if(e->keys[KEY_DOWN]) {
            vec3 dir; 
            vecMult(c->viewDir, -1, dir);
            vecAdd(c->pos, dir, c->pos);
        }
        if(e->keys[KEY_RIGHT]) {
            vec3 dir;
            cross(c->viewDir, c->up, dir);
            vecNormalize(dir);
            vecMult(dir, 1, dir);
            vecAdd(c->pos, dir, c->pos);

            //FIXME não tá funcionando direito:
            //a esfera parece se aproximar enquanto nos movemos para
            //a direita
            //Atualiza o viewDir
            //vec3 viewPoint;
            //vecAdd(c->viewDir, c->pos, viewPoint);
            //vecAdd(viewPoint, dir, viewPoint);
            //vecSub(viewPoint, c->pos, c->viewDir);
            //vecNormalize(c->viewDir);
        }
        if(e->keys[KEY_LEFT]) {
            vec3 dir;
            cross(c->viewDir, c->up, dir);
            vecNormalize(dir);
            vecMult(dir, -1, dir);
            vecAdd(c->pos, dir, c->pos);
        }
    }
}


/* assume um fit na "frente do modelo" */
void camerafit(Camera *c, BoundingBox b, float fovy, float ratio, float znear, float zfar) {
    QUAT_IDENTITY(c->orientation);
    c->up[0] = 0.0;
    c->up[1] = 1.0;
    c->up[0] = 0.0;

    vec3 bcenter;
    bbcenter(b, bcenter);

    c->pos[0] = bcenter[0];
    c->pos[1] = bcenter[1];
    c->pos[2] = bcenter[2] + 4*(b.pmax[2] - b.pmin[2]) + ((b.pmax[1] - bcenter[1])/tan(0.5*fovy));
    printf("c->pos: %f, %f, %f\n", c->pos[0], c->pos[1], c->pos[2]); 
    printf("bcenter: %f, %f, %f\n", bcenter[0], bcenter[1], bcenter[2]); 

    c->viewDir[0] = 0;
    c->viewDir[1] = 0;
    c->viewDir[2] = -1;

    fpIdentity(c->modelview);
    fpperspective(c->projection, fovy, ratio, znear, zfar);
}

