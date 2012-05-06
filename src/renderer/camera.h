#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../math/vec3.h"
#include "../math/quaternion.h"
#include "../math/matrix.h"
#include "../math/boundingbox.h"
#include "../glapp.h"

/**
 * TODO: camerafit deveria posicionar a camera de modo que não fosse mais necessário
 * transladar o modelo para o centro do sistema de coordenada quando estiver no modo
 * trackball
 */

typedef enum _cameratype { FIRSTPERSON, TRACKBALL, CAMERA_2D } CameraType;

typedef struct {
    vec3 pos, up, viewDir;
    quaternion orientation;
    mat4 modelview;
    mat4 projection;
    mat4 mvp;
    mat3 normalmatrix;
    CameraType type;
} Camera;

void initCamera(Camera *c, CameraType t);
void cameraHandleEvent(Camera *c, event *e);
void setupViewMatrix(Camera *c);
void camerafit(Camera *c, BoundingBox b, float fovy, float ratio, float znear, float zfar);

#endif
