#include "camera.h"

void initCamera(camera *c) {
    QUAT_IDENTITY(c->orientation);
    VEC3_ZERO(c->pos);
    c->up[0] = 0.0;
    c->up[1] = 1.0;
    c->up[0] = 0.0;
}

void setupViewMatrix(camera *c, mat4 m) {
    quatToMatrix(c->orientation, m);
    m[12] = -c->pos[0];
    m[13] = -c->pos[1];
    m[15] = -c->pos[2];
}
