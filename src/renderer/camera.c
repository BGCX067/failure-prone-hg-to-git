#include <stdio.h>
#include <string.h>
#include <math.h> 
#include "camera.h"
#include "../math/matrix.h"
#include "../math/mathutil.h"

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2		1.57079632679489661923	/* pi/2 */
#endif

static float zoomFactor = 5.0;
static float zoomWheelFactor = 0.5;
static float panFactor = 0.01;


/*
void camerafit(Camera *c, BoundingBox b, float fovy, float ratio, float znear, float zfar) {
    QUAT_IDENTITY(c->orientation);
    c->up[0] = 0.0;
    c->up[1] = 1.0;
    c->up[0] = 0.0;

    vec3 bcenter;
    bbcenter(b, bcenter);

    c->pos[0] = bcenter[0];
    c->pos[1] = bcenter[1];
    //c->pos[2] = bcenter[2] + 4*(b.pmax[2] - b.pmin[2]) + ((b.pmax[1] - bcenter[1])/tan(0.5*fovy));
    c->pos[2] = b.pmax[2] + (b.pmax[1] - bcenter[1])/tan(0.5*3.141592*fovy/180.0) + znear;
    printf("c->pos: %f, %f, %f\n", c->pos[0], c->pos[1], c->pos[2]); 
    printf("b.pmax: %f, %f, %f\n", b.pmax[0], b.pmax[1], b.pmax[2]); 
    printf("bcenter: %f, %f, %f\n", bcenter[0], bcenter[1], bcenter[2]); 

    c->viewDir[0] = 0;
    c->viewDir[1] = 0;
    c->viewDir[2] = -1;

    fpIdentity(c->modelview);
    fpperspective(c->projection, fovy, ratio, znear, zfar);
}
*/

void CamMoveX(Camera *c, float dist) {
    //pos += dist*right;
    vec3 right;
    Cross(c->view, c->up, right);
    Multv(right, dist, right);
    Addv(c->pos, right, c->pos);
}
void CamMoveY(Camera *c, float dist) {
    //pos += dist * c->up;
    vec3 disp;
    Multv(c->up, dist, disp);
    Addv(c->pos, disp, c->pos);
}
void CamMoveZ(Camera *c, float dist) {
    //pos += view*dist;
    vec3 disp;
    Multv(c->view, dist, disp);
    Addv(c->pos, disp, c->pos);
}

void CamRotX(Camera *c, float angle) {
    float rad = DegToRad(angle*0.5);
    vec3 right;
    Cross(c->view, c->up, right);
    //quat q = {right[0]*sin(rad), right[1]*sin(rad), right[2]*sin(rad), cos(rad)};
    quat q = {1.0*sin(rad), 0.0*sin(rad), 0.0*sin(rad), cos(rad)};
    Rotateq(c->view, q, c->view);
    Normalizev(c->view);
    Cross(right, c->view, c->up);
    Multq(q, c->orientation, c->orientation);
    Normalizeq(c->orientation);
}
void CamRotY(Camera *c, float angle) {
    float rad = DegToRad(angle*0.5);
    //quat q = {c->up[0]*sin(rad), c->up[1]*sin(rad), c->up[2]*sin(rad), cos(rad)};
    quat q = {0.0*sin(rad), 1.0*sin(rad), 0.0*sin(rad), cos(rad)};
    Rotateq(c->view, q, c->view);
    Normalizev(c->view);
    Multq(q, c->orientation, c->orientation);
    Normalizeq(c->orientation);
}
void CamRotZ(Camera *c, float angle) {
    float rad = DegToRad(angle*0.5);
    quat q = {c->view[0]*sin(rad), c->view[1]*sin(rad), c->view[2]*sin(rad), cos(rad)};
    Normalizeq(q);
    Rotateq(c->up, q, c->up);
    //quatMult(c->orientation, q, c->orientation);
    //quatNormalize(c->orientation);
}

void SetFovy(Camera* c, float f){
    c->fovy = f;
    Perspective(c->mprojection, c->fovy, (float)c->screenW/(float)c->screenH, c->znear, c->zfar);
}

void SetZnear(Camera* c, float f){
    c->znear = f;
    Perspective(c->mprojection, c->fovy, (float)c->screenW/(float)c->screenH, c->znear, c->zfar);
}

void SetZfar(Camera* c, float f){
    c->zfar = f;
    Perspective(c->mprojection, c->fovy, (float)c->screenW/(float)c->screenH, c->znear, c->zfar);
}
static void fpsUpdate(Camera *c, event *e, double *dt);
static void trackballUpdate(Camera *c, event *e, double *dt);

void CamInit(Camera *c, int w, int h, int ct, int pt) {
    c->screenW = w;
    c->screenH = h;
    c->zoom = 0.0; 
    Setvf(c->pos, 0.0, 0.0, 0.0);
    Setvf(c->up, 0.0, 1.0, 0.0);
    Setvf(c->view, 0.0, 0.0, -1.0);
    Setvf(c->pivot, 0.0, 0.0, 0.0);
    //Quaternion identidade
    Setqf(c->orientation, 0.0, 0.0, 0.0, 1.0);
    Identity(c->mview);
    Identity(c->mprojection);

    c->ctype = ct;
    switch(ct) {
        case FPS:
            c->update = fpsUpdate;
            WarpMouse(1);
            break;
        case TRACKBALL:
            c->update = trackballUpdate;
            break;
        default:
            c->update = NULL;
            break;
    }

    c->fovy = 45.0;
    c->znear = 0.1;
    c->zfar = 100.0;

    if(pt == PERSPECTIVE)
        Perspective(c->mprojection, c->fovy, (float)w/(float)h, c->znear, c->zfar);
    else if (pt == ORTHO) {
        //FIXME setar znear e zfar da camera diferente se a projeção for do tipo
        //orthographic.
        float xmax = c->znear*tan(0.5*c->fovy*M_PI/180.0);
        float xmin = -xmax;

        float ymax = xmax/((float)w/(float)h);
        float ymin = -ymax;

        Ortho(c->mprojection, xmin, xmax, ymin, ymax, c->znear, c->zfar);
    }
}

//UP da camera fps sempre será (0, 1, 0) para todos os efeitos (calcular right, por exemplo)
static void fpsUpdate(Camera *c, event *e, double *dt) {
    //TODO camSpeed e rotSpeed deveriam ser globais pra todas
    //as cameras
    static float camSpeed = 1.0;
    static float rotSpeed = 5.0;
    float elapsedTime = *dt; 

    if(e->keys[KEY_w])
        CamMoveZ(c, camSpeed*elapsedTime);
    if(e->keys[KEY_s])
        CamMoveZ(c, -camSpeed*elapsedTime);
    if(e->keys[KEY_d])
        CamMoveX(c, camSpeed*elapsedTime);
    if(e->keys[KEY_a])
        CamMoveX(c, -camSpeed*elapsedTime);
    if(e->keys[KEY_e]) 
        CamMoveY(c, camSpeed*elapsedTime);
    if(e->keys[KEY_q])
        CamMoveY(c, -camSpeed*elapsedTime);

    /*if(e->keys[KEY_i])
        CamRotX(c, rotSpeed*elapsedTime);
    if(e->keys[KEY_k])
        CamRotX(c, -rotSpeed*elapsedTime);
    if(e->keys[KEY_j])
        CamRotY(c, rotSpeed*elapsedTime);
    if(e->keys[KEY_l])
        CamRotY(c, -rotSpeed*elapsedTime);
    if(e->keys[KEY_u])
        CamRotZ(c, rotSpeed*elapsedTime);
    if(e->keys[KEY_o])
        CamRotZ(c, -rotSpeed*elapsedTime);*/

    if(e->type & MOUSE_MOTION_EVENT && e->buttonRight) {
        float dx = c->screenW*0.5 - e->x;
        float dy = c->screenH*0.5 - e->y;
       
        CamRotY(c, dx*rotSpeed*elapsedTime);
        vec3 oldview = {c->view[0], c->view[1], c->view[2]}; 
        CamRotX(c, dy*rotSpeed*elapsedTime);
        //Verifica se o vetor view é paralelo ao vetor Up do mundo.
        //Para evitar que view e up se alinhem, a rotação é limitada.
        vec3 upw = {0.0, 1.0, 0.0};
        float cosvu = Dot(c->view, upw);
        if(fabs(cosvu - 0.0001) > 0.97) {
            c->view[0] = oldview[0]; c->view[1] = oldview[1]; c->view[2] = oldview[2];
        }

        //alguma das rotações pode ter modificado o up da camera
        //na fps camera, sempre será considerado o up do mundo
        c->up[0] = 0.0; c->up[1] = 1.0; c->up[2] = 0.0;
    }
    LookAt(c->mview, c->pos, c->view, c->up);
}

static float projectToSphere(float r, float x, float y) {
    float d = sqrtf(x*x + y*y);
    float z;
    if(d < r*0.7071067811)
        z = sqrtf(r*r - d*d);
    else {
        float t = r/1.41421356237309504880;
        z = t*t/d;
    }
    return z;
}

//0.8 é o tamanho da trackball
static void trackball(quat q, float p1x, float p1y, float p2x, float p2y) {
    vec3 axis, p1, p2;
    
    //verifica se p1 e p2 sao iguais
    if(fabs(p1x - p2x) < 0.000001 && fabs(p1y - p2y) < 0.000001) {
        Setqf(q, 0.0, 0.0, 0.0, 1.0);
        return;
    }

    p1[0] = p1x; p1[1] = p1y; p1[2] = projectToSphere(0.8, p1x, p1y);
    p2[0] = p2x; p2[1] = p2y; p2[2] = projectToSphere(0.8, p2x, p2y);
    Cross(p2, p1, axis);
    vec3 d;
    Subv(p1, p2, d);
    //Magica: t é o seno do angulo, não entendo pq
    float t = Lengthv(d)/(2.0*0.8);
    
    if(t > 1.0) t = 1.0;
    if(t < -1.0) t = -1.0;
    //float angle = 2.0*asin(t);
    //angle = angle < 1.0 ? 1.0 : angle;
    //float angle = vangle(p1, p2);
    Normalizev(axis);
    q[0] = axis[0]*t;
    q[1] = axis[1]*t;
    q[2] = axis[2]*t;
    q[3] = 1.0 - t*t;
}

static void trackballUpdate(Camera *c, event *e, double *dt) {
    static int spinning = 0, zooming = 0, moving = 0;
    static float p1x = 0.0, p1y = 0.0;

    if(e->type & MOUSE_BUTTON_PRESS && e->buttonRight 
            && (e->keys[KEY_LCONTROL] || e->keys[KEY_RCONTROL])) {
        p1y = e->y;
        zooming = 1;
    } else if((e->type & MOUSE_BUTTON_PRESS) && e->buttonRight 
            && (e->keys[KEY_LSHIFT] || e->keys[KEY_RSHIFT])) {
        p1x = e->x;  
        p1y = c->screenH - e->y;
        moving = 1;
    } else if(e->type & MOUSE_BUTTON_PRESS && e->buttonRight) {
        p1x = e->x;
        p1y = e->y;
        spinning = 1;
    } else if(e->type & MOUSE_BUTTON_RELEASE) {
        spinning = 0;
        zooming = 0;
        moving = 0;
    } else if(e->type & MOUSE_MOTION_EVENT && e->buttonRight && spinning) {
        quat q;
        float p2x = e->x, p2y = e->y;
        trackball(q, (2.0*p1x - c->screenW)/c->screenW, (c->screenH - 2.0*p1y)/c->screenH,
                     (2.0*p2x - c->screenW)/c->screenW, (c->screenH - 2.0*p2y)/c->screenH);
        Multq(c->orientation, q, c->orientation);
        Normalizeq(c->orientation);
        p1x = p2x;
        p1y = p2y;
    } else if(e->type & MOUSE_MOTION_EVENT && e->buttonRight 
            && (e->keys[KEY_LCONTROL] || e->keys[KEY_RCONTROL]) && zooming) {
        float p2y = e->y;
        c->zoom += zoomFactor*(p2y - p1y)/c->screenH;
        p1y = p2y;
    } else if(e->type & MOUSE_WHEEL) {
        c->zoom -= zoomWheelFactor*e->wheel;
    }else if(e->type & MOUSE_MOTION_EVENT && e->buttonRight 
            && (e->keys[KEY_LSHIFT] || e->keys[KEY_RSHIFT]) && moving) {
        float p2x = e->x;
        float p2y = c->screenH - e->y;
        //TODO considerar aspect ratio e zoom
        float dx = c->zoom*panFactor*(p2x - p1x)/c->screenW;
        float dy = c->zoom*panFactor*(p2y - p1y)/c->screenH;

        mat4 m;
        ToMatrixq(c->orientation, m);
        
        c->pivot[0] += dx*m[0] + dy*m[1]; 
        c->pivot[1] += dx*m[4] + dy*m[5]; 
        c->pivot[2] += dx*m[8] + dy*m[9]; 
    } else if(e->keys[KEY_1]) {
        //FRONT
        vec3 v = {0.0, 0.0, 0.0};
        FromAxisAngle(v, 0.0, c->orientation);
    } else if(e->keys[KEY_2]) {
        //RIGHT
        vec3 v = {0.0, 1.0, 0.0};
        FromAxisAngle(v, M_PI_2, c->orientation);
    } else if(e->keys[KEY_3]) {
        //TOP
        vec3 v = {-1.0, 0.0, 0.0};
        FromAxisAngle(v, M_PI_2, c->orientation);
    }

    mat4 z, p;
    Identity(z);
    Identity(p);
    Translatef(z, 0.0, 0.0, -c->zoom);
    Translatef(p, -c->pivot[0], -c->pivot[1], -c->pivot[2]);
    ToMatrixq(c->orientation, c->mview);
    Multm(c->mview, z, c->mview);
    Multm(c->mview, c->mview, p);
}
