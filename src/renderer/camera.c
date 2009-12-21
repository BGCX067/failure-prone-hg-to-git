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
    //m[12] = -c->pos[0];
    //m[13] = -c->pos[1];
    //m[14] = -c->pos[2];
}

void cameraHandleEvent(camera *c, event *e) {
    //TODO pegar o tamanho da janela
    static int i = 0;
    if(e->type & MOUSE_MOTION_EVENT) {
        float dx = 400 - e->x;
        float dy = 300 - e->y;

        float mouseSensitivity = 0.001;
        float rotationUp = dx*mouseSensitivity;
        float rotationRight = dy*mouseSensitivity;

        if(rotationUp > 0.1)
            rotationUp = 0.1;
        if(rotationRight > 0.1)
            rotationRight = 0.1;
        
        /* Rotação com quaternion e rotacionando os vetores para manter os eixos
         * TODO: tirar os eixos da matriz                                       */
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


        /*cross(right, c->viewDir, c->up);
        fromAxisAngle(c->up, rotationUp, q2);
        rotateVec(c->viewDir, q2, c->viewDir);
        vecNormalize(c->viewDir);*/
        mult(q2, q1, q2);
        mult(q2, c->orientation, c->orientation);
        quatNormalize(c->orientation);
        
        /* Rotação na marra
        vec3 right;
        cross(c->viewDir, c->up, right);
        c->viewDir[0] = c->viewDir[0]*cos(rotationRight*PIdiv180) + c->up[0]*sin(rotationRight*PIdiv180);
        c->viewDir[1] = c->viewDir[1]*cos(rotationRight*PIdiv180) + c->up[1]*sin(rotationRight*PIdiv180);
        c->viewDir[2] = c->viewDir[2]*cos(rotationRight*PIdiv180) + c->up[2]*sin(rotationRight*PIdiv180);
        vecNormalize(c->viewDir);
        cross(right, c->viewDir, c->up);
        
        c->viewDir[0] = c->viewDir[0]*cos(rotationUp*PIdiv180) - right[0]*sin(rotationUp*PIdiv180);
        c->viewDir[1] = c->viewDir[1]*cos(rotationUp*PIdiv180) - right[1]*sin(rotationUp*PIdiv180);
        c->viewDir[2] = c->viewDir[2]*cos(rotationUp*PIdiv180) - right[2]*sin(rotationUp*PIdiv180); 
        vecNormalize(c->viewDir);
        */

        /* Rotação usando quaternion para rotacionar os vetores 
        //Rotaciona ao redor do eixo right
        quaternion q;
        vec3 right;
        cross(c->viewDir, c->up, right);
        fromAxisAngle(right, rotationRight, q);
        rotateVec(c->viewDir, q, c->viewDir);

        //Rotaciona ao redor do eixo up
        fromAxisAngle(c->up, rotationUp, q);
        rotateVec(c->viewDir, q, c->viewDir);
        vecNormalize(c->viewDir);
        */
    }
    if(e->type & KEYBOARD_EVENT) {
        if(e->keys[KEY_UP]) {
            vec3 dir; 
            vecMult(c->viewDir, 0.1, dir);
            vecAdd(c->pos, dir, c->pos);
        }
        if(e->keys[KEY_DOWN]) {
            vec3 dir; 
            vecMult(c->viewDir, -0.1, dir);
            vecAdd(c->pos, dir, c->pos);
        }
        if(e->keys[KEY_RIGHT]) {
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
        if(e->keys[KEY_LEFT]) {
            vec3 dir;
            cross(c->viewDir, c->up, dir);
            vecNormalize(dir);
            vecMult(dir, -0.1, dir);
            vecAdd(c->pos, dir, c->pos);
        }
    }
}

