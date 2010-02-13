#ifndef _FPLIST_H_
#define _FPLIST_H_

typedef struct _fpnode {
    void *data;
    struct _fpnode *next;
    struct _fpnode *prev;
}fpnode;

void fpnode_destroy(fpnode* n, void (*destroy)(void*));

typedef struct _fplist {
    fpnode *first;
    fpnode *last;
    int size;
    void (*destroy)(void*);
}fplist;

/* TODO considerar iterator */
/* typedef fpnode* fplist_iterator;*/

/* Inicializador e destrutor para a lista */
fplist* fplist_init(void (*dest)(void*));
void fplist_destroy(fplist *l);

void* fplist_getdata(int index, fplist *l);

void fplist_insfront(void *data, fplist *l);
int fplist_insback(void *data, fplist *l);
void fplist_rmfront(fplist *l);
void fplist_rmback(fplist *l);
fpnode* fplist_rmnode(fpnode *n, fplist *l);
#endif
