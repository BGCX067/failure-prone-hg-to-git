#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../math/vec3.h"
#include "../math/quaternion.h"
#include "../math/matrix.h"
#include "../glapp.h"

typedef enum _cameratype { FIRSTPERSON, TRACKBALL } cameratype;

typedef struct camera {
    vec3 pos, up, viewDir;
    quaternion orientation;
    mat4 modelview;
    mat4 projection;
    mat4 mvp;
    cameratype type;
}camera;

void initCamera(camera *c, cameratype t);
void cameraHandleEvent(camera *c, event *e);
void setupViewMatrix(camera *c);

#endif
