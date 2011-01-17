#include "boundingbox.h"


void bbcenter(boundingbox b, vec3 v) {
    v[0] = 0.5*(b.pmin[0] + b.pmax[0]);
    v[1] = 0.5*(b.pmin[1] + b.pmax[1]);
    v[2] = 0.5*(b.pmin[2] + b.pmax[2]);
}


void setboundingbox(boundingbox *b, float *vertices, int numvertices) {

    b->pmin[0] = 99999999;
    b->pmin[1] = 99999999;
    b->pmin[2] = 99999999;

    b->pmax[0] = -999999999;
    b->pmax[1] = -999999999;
    b->pmax[2] = -999999999;

    for(int j = 0; j < numvertices; j++) {
        //X
        if(vertices[3*j] < b->pmin[0])
            b->pmin[0] = vertices[3*j];
        if (vertices[3*j] > b->pmax[0])
            b->pmax[0] = vertices[3*j];
        //Y
        if(vertices[3*j + 1] < b->pmin[1])
            b->pmin[1] = vertices[3*j + 1];
        if (vertices[3*j + 1] > b->pmax[1])
            b->pmax[1] = vertices[3*j + 1];
        //Z
        if(vertices[3*j + 2] < b->pmin[2])
            b->pmin[2] = vertices[3*j + 2];
        if (vertices[3*j + 2] > b->pmax[2])
            b->pmax[2] = vertices[3*j + 2];
    }
}
