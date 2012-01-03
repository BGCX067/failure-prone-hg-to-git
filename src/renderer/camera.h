#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../math/vec3.h"
#include "../math/quaternion.h"
#include "../math/matrix.h"
#include "../math/boundingbox.h"
#include "../glapp.h"

typedef enum _cameratype { FIRSTPERSON, TRACKBALL } CameraType;

typedef struct {
    vec3 pos, up, viewDir;
    quaternion orientation;
    mat4 modelview;
    mat4 projection;
    mat4 mvp;
    CameraType type;
} Camera;

void initCamera(Camera *c, CameraType t);
void cameraHandleEvent(Camera *c, event *e);
void setupViewMatrix(Camera *c);
void camerafit(Camera *c, boundingbox b, float fovy, float ratio, float znear, float zfar);

#endif
