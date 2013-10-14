#include "halfedge.h"


typedef enum { PLUS, MINUS }HESign;

extern inline HEHalfEdge* HEMate(HEHalfEdge *he);

static HEHalfEdge* addhe(HEEdge *e, HEVertex *v, HEHalfEdge *h, HESign sign) {
    HEHalfEdge *he;
    if (h->edge == NULL)
        he = h;
    else {
        //he = new HalfEdge;
        he = malloc(sizeof(HEHalfEdge));
        h->prev->next = he;
        he->prev = h->prev;
        h->prev = he;
        he->next = h;
    }

    he->edge = e;
    he->origin = v;
    he->loop = h->loop;
    if(sign == PLUS)
        e->hed1 = he;
    else
        e->hed2 = he;

    return he;
}

static HEHalfEdge* delhe(HEHalfEdge *he) {
    if(he->edge == 0) {
        //delete he;
        free(he);
        return NULL;
    } else if (he->next == he) {
        he->edge = 0;
        return he;
    }
    else {
        he->prev->next = he->next;
        he->next->prev = he->prev;
        delete he;
        return he->prev;
    }
}

