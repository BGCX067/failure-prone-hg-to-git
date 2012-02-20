#include "boundingbox.h"


void bbcenter(BoundingBox b, vec3 v) {
    v[0] = 0.5*(b.pmin[0] + b.pmax[0]);
    v[1] = 0.5*(b.pmin[1] + b.pmax[1]);
    v[2] = 0.5*(b.pmin[2] + b.pmax[2]);
}


void setboundingbox(BoundingBox *b, float *vertices, int numvertices) {

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


void bbunion(BoundingBox *res, BoundingBox b1, BoundingBox b2) {
    res->pmin[0] = b1.pmin[0] < b2.pmin[0] ? b1.pmin[0] : b2.pmin[0];
    res->pmin[1] = b1.pmin[1] < b2.pmin[1] ? b1.pmin[1] : b2.pmin[1];
    res->pmin[2] = b1.pmin[2] < b2.pmin[2] ? b1.pmin[2] : b2.pmin[2];

    res->pmax[0] = b1.pmax[0] > b2.pmax[0] ? b1.pmax[0] : b2.pmax[0];
    res->pmax[1] = b1.pmax[1] > b2.pmax[1] ? b1.pmax[1] : b2.pmax[1];
    res->pmax[2] = b1.pmax[2] > b2.pmax[2] ? b1.pmax[2] : b2.pmax[2];
}
