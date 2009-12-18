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

void cameraHandleEvent(camera *c, event e) {
    //TODO pegar o tamanho da janela
    static int i = 0;

    if(e.type == MOUSE_MOVE) {
        float dx = 400 - e.x;
        float dy = 300 - e.y;

        printf("i: %d\n", i);
        printf("\tdx: %f\n", dx);
        printf("\tdy: %f\n", dy);
        i++;
        
        float mouseSensitivity = 0.001;
        float rotationUp = dx*mouseSensitivity;
        float rotationRight = dy*mouseSensitivity;

        if(rotationUp > 1.0)
            rotationUp = 1.0;
        if(rotationRight > 1.0)
            rotationRight = 1.0;
        quaternion q;
        //Rotaciona ao redor do eixo up
        fromAxisAngle(c->up, rotationUp, q);
        rotateVec(c->viewDir, q, c->viewDir);
        vecNormalize(c->viewDir);
        
        //Rotaciona ao redor do eixo right
        vec3 right;
        cross(c->viewDir, c->up, right);
        fromAxisAngle(right, rotationRight, q);
        rotateVec(c->viewDir, q, c->viewDir);
        vecNormalize(c->viewDir);
        //O up deve ter mudado, encontra o novo
        cross(right, c->viewDir, c->up);
    }
    if(e.type == KEY_UP) {
        vec3 dir; 
        vecMult(c->viewDir, 0.1, dir);
        vecAdd(c->pos, dir, c->pos);
    }
    if(e.type == KEY_DOWN) {
        vec3 dir; 
        vecMult(c->viewDir, -0.1, dir);
        vecAdd(c->pos, dir, c->pos);
    }
    if(e.type == KEY_RIGHT) {
        vec3 dir;
        cross(c->viewDir, c->up, dir);
        vecNormalize(dir);
        vecMult(dir, 0.1, dir);
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
    if(e.type == KEY_LEFT) {
        vec3 dir;
        cross(c->viewDir, c->up, dir);
        vecNormalize(dir);
        vecMult(dir, -0.1, dir);
        vecAdd(c->pos, dir, c->pos);
    }
}

