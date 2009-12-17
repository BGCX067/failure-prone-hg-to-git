#include "camera.h"

void initCamera(camera *c) {
    QUAT_IDENTITY(c->orientation);
    VEC3_ZERO(c->pos);
    c->up[0] = 0.0;
    c->up[1] = 1.0;
    c->up[0] = 0.0;

    VEC3_ZERO(c->viewDir);
    c->viewDir[2] = -1;
}

void setupViewMatrix(camera *c, mat4 m) {
    quatToMatrix(c->orientation, m);
    m[12] = -c->pos[0];
    m[13] = -c->pos[1];
    m[15] = -c->pos[2];
}

void cameraHandleEvent(camera *c, keyboard k) {
    if(k.keys[KEY_UP]) {
        vec3 dir; 
        vecMult(c->viewDir, 0.001, dir);
        vecAdd(c->pos, dir, c->pos);
    }
    if(k.keys[KEY_DOWN]) {
        vec3 dir; 
        vecMult(c->viewDir, -0.001, dir);
        vecAdd(c->pos, dir, c->pos);
    }
    if(k.keys[KEY_RIGHT]) {
        vec3 dir;
        cross(c->viewDir, c->up, dir);
        //FIXME produto vetorial de vetores unitários já é
        //unitário, não precisa normalizar
        vecNormalize(dir);
        vecMult(dir, 0.001, dir);
        vecAdd(c->pos, dir, c->pos);
        
        //FIXME não tá funcionando direito:
        //a esfera parece se aproximar enquanto nos movemos para
        //a direita
        //Atualiza o viewDir
        vec3 viewPoint;
        vecAdd(c->viewDir, c->pos, viewPoint);
        vecAdd(viewPoint, dir, viewPoint);
        vecSub(viewPoint, c->pos, c->viewDir);
        vecNormalize(c->viewDir);
    }
    if(k.keys[KEY_LEFT]) {
        vec3 dir;
        cross(c->viewDir, c->up, dir);
        vecNormalize(dir);
        vecMult(dir, -0.001, dir);
        vecAdd(c->pos, dir, c->pos);
        
        //FIXME não tá funcionando direito
        //mesmo problema do movimento para direita
        //Atualiza o viewDir
        vec3 viewPoint;
        vecAdd(c->viewDir, c->pos, viewPoint);
        vecAdd(viewPoint, dir, viewPoint);
        vecSub(viewPoint, c->pos, c->viewDir);
        vecNormalize(c->viewDir);
    }

}
