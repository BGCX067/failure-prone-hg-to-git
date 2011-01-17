#ifndef BOUNDINGBOX_H_
#define BOUNDINGBOX_H_

#include "vec3.h"

/* axis-aligned bounding box */
typedef struct _boundingbox {
    vec3 pmin;
    vec3 pmax;
} boundingbox;

void bbcenter(boundingbox b, vec3 v);
void setboundingbox(boundingbox *b, float *vertices, int numvertices);


#endif
