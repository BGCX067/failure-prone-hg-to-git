#ifndef BOUNDINGBOX_H_
#define BOUNDINGBOX_H_

#include "vec3.h"

/* axis-aligned bounding box */
typedef struct _boundingbox {
    vec3 pmin;
    vec3 pmax;
} BoundingBox;

void bbcenter(BoundingBox b, vec3 v);

//numvertices aqui é realmente o número de vertices (tamanho do vetor/3, em geral)
void setboundingbox(BoundingBox *b, float *vertices, int numvertices);

void bbunion(BoundingBox *res, BoundingBox b1, BoundingBox b2);


#endif
