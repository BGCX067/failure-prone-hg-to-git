#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../math/vec3.h"
#include "../math/quaternion.h"
#include "../math/matrix.h"
#include "../glapp.h"

typedef struct camera {
   vec3 pos, up, viewDir;
   quaternion orientation;
}camera;

void initCamera(camera *c);
void cameraHandleEvent(camera *c, event e);
void setupViewMatrix(camera *c, mat4 m);


#endif
