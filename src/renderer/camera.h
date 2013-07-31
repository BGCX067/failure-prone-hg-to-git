#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../math/vec3.h"
#include "../math/quaternion.h"
#include "../math/matrix.h"
#include "../math/mathutil.h"
#include "../glapp.h"

//TODO List
//1. 3rd person e Isometric
//2. Como fazer pra camera 3ª pessoa seguir um ponto 
//3. API da camera
//   3.1. Opção de setar o tipo de camera. colocar tipo "free"
//        para quando não for desejado usar uma camera, o usuario
//        fica responsavel de fazer o setView do renderer.
//   3.2. Update da camera automatico? glapp é o único módulo da engine
//        que atualmente tem a possibilidade de fazer coisas automatiamente
//        através do mainloop
//   3.3. Setar a camera atual - cada demo cria e modifica a camera
//        3.3.1. Ter uma camera default na engine?
//4. Camera seguir uma curva
//   4.1. Interpolação?
//5. Funções auxiliares para posicionar uma camera
//  5.1. Fit em uma bounding box.
//  5.2. View from top, front, right, bottom, back, left
//6. Existe o caso em que mais de 1 camera participa da renderização?
//   5.1. Mais de um contexto/viewport deveriam ser criados? (tipo call of duty co-op)
//
//TODO FPS
//1. speed para rotação e translação deveriam ser globais e modificáveis
//   de algum modo.
//2.
//
//TODO TRACKBALL
//1. Zoom
//   1.1. Mouse wheel
//   1.2. translate?

//FIXME List 
//
//typedef enum _cameratype { FPS, TRACKBALL } CameraType;

enum cameratype { FPS, TRACKBALL };
enum projectionType { PERSPECTIVE, ORTHO };
typedef struct _camera Camera;

//TODO mudar o nome de view para look
struct _camera{
    vec3 pos, view, up;
    quat orientation;
    void (*update)(Camera *c, event *e, double *dt);
    int ctype, ptype;
    int screenW, screenH;
    mat4 mview, mprojection;
    float fovy;
    float znear, zfar;
};

//Mover
void CamMoveX(Camera *c, float dist);
void CamMoveY(Camera *c, float dist);
void CamMoveZ(Camera *c, float dist);


//AjuStar orientação
void CamRotX(Camera *c, float angle);
void CamRotY(Camera *c, float angle);
void CamRotZ(Camera *c, float angle);

//API HIGH LEVEL
void CamInit(Camera *c, int w, int h, int ct, int pt);

void SetFovy(Camera *c, float f);
void SetZfar(Camera *c, float f);
void SetZnear(Camera *c, float f);

#endif
