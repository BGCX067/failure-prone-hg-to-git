#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../math/vec3.h"
#include "../math/quaternion.h"
#include "../math/matrix.h"

typedef struct camera {
   vec3 pos, up;
   quaternion orientation;
}camera;

void initCamera(camera c);
/* TODO
 * void cameraHandleEvent()*/
void setupViewMatrix(camera c, matrix m);
/* TODO void rotate();*/



#endif
