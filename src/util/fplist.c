#include "fplist.h"
#include "malloc.h"

#include <stdio.h>

void fpnode_destroy(fpnode* n, void (*destroy)(void*)){
    destroy(n->data);
    dlfree(n);
}

fplist* fplist_init(void (*dest)(void*)) {
    fplist *l = (fplist*) dlmalloc(sizeof(fplist));
    l->first = NULL;
    l->last= NULL;
    l->size = 0;
    l->destroy = dest;
}

void fplist_destroy(fplist *l) {
    fpnode *n = l->first;
    while(n) {
        fpnode *next = n->next;
        fpnode_destroy(n, l->destroy);
        n = next;
    }
    dlfree(l); 
    l = NULL;
}

void* fplist_getdata(int index, fplist *l) {
    fpnode *n = l->first;

    int i = 0;
    while(n && i < index) {
        i++;
        n = n->next;
    }
    if (!n || i != index)
        return NULL;

    return n->data;
}

void fplist_insfront(void *data, fplist *l) {
    /* Aloca um node novo */
    fpnode *n = (fpnode*) dlmalloc(sizeof(fpnode));
    /* Ajusta os ponteiros corretamente - inserindo no início */
    n->next = l->first;
    n->prev = NULL;
    l->first = n;
    
    if(!l->last)
        l->last = n;

    /* Aloca  a memória para o elemento e copia o valor */
    /* FIXME a memória para data não está sendo alocada */
    /* newNode->data = dlmalloc(sizeof(data)); */
    n->data = data;

    l->size++;
}

int fplist_insback(void *data, fplist *l) {
    printf("insback\n");
    fpnode *n = (fpnode*)dlmalloc(sizeof(fpnode));
    n->data = data;
    n->next = NULL;

    /* Primeiro elemento */
    if(!l->first) {
        printf("\tprimeiro elemento na lista\n");
        l->first = n;
        l->last = n;
        n->prev = NULL;
    } else {
        n->prev = l->last;
        l->last->next = n;
        l->last = n;
    }
    l->size++;
    return l->size - 1;
}

void fplist_rmfront(fplist *l) {
    /* Ajusta os ponteiros */
    fpnode *n = l->first;
    l->first = l->first->next;

    if(!l->first)
        l->last = NULL;
    
    l->first->prev = NULL;
    /* Libera a memória do ponteiro que o node armazena */
    fpnode_destroy(n, l->destroy);
    
    l->size--;
}

void fplist_rmback(fplist *l) {
    fpnode *n = l->last;

    if(n->prev) {
        n->prev->next = NULL;
        l->last = n->prev;
    } else {
        l->first = NULL;
        l->last = NULL;
    }
    fpnode_destroy(n, l->destroy);

    l->size--;
}


fpnode* fplist_rmnode(fpnode *n, fplist *l) {
    fpnode *it = l->first;

    while(it != n) 
        it = it->next;

    if (it->prev)
        it->prev->next = it->next;
    else
        l->first = it->next;
    if (it->next) 
        it->next->prev = it->prev;
    else
        l->last = it->prev;

    fpnode* next = it->next;

    fpnode_destroy(it, l->destroy);

    return next;
}
