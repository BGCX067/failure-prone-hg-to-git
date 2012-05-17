#include "fplist.h"
#include <stdio.h>
#include <stdlib.h> //malloc

void fpnode_destroy(fpnode* n, void (*destroy)(void*)){
    destroy(n->data);
    free(n);
}

fplist* fplist_init(void (*dest)(void*)) {
    fplist *l = (fplist*) malloc(sizeof(fplist));
    l->first = NULL;
    l->last= NULL;
    l->size = 0;
    l->destroy = dest;

    return l;
}

void fplist_destroy(fplist *l) {
    fpnode *n = l->first;
    while(n) {
        fpnode *next = n->next;
        fpnode_destroy(n, l->destroy);
        n = next;
    }
    free(l); 
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
    fpnode* n = (fpnode*) malloc(sizeof(fpnode));

    n->data = data;

    /* Ajusta os ponteiros corretamente - inserindo no início */
    n->next = l->first;
    l->first = n;
    
    if(!l->last)
        l->last = n;

    l->size++;
}

int fplist_insback(void *data, fplist *l) {
    fpnode* n = (fpnode*) malloc(sizeof(fpnode));

    n->data = data;

    n->next = NULL;
    
    /* Primeiro elemento */
    if(!l->first) {
        //printf("!l->first\n");
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

//FIXME bugou quando tentei remover 1 elemento da lista quando
//a lista só tinha 1 elemento
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

//TODO  precisa passar um ponteiro pra funcao com uma comparefunc?
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

void fplist_rm(int index, fplist* l){

	if (l == NULL)
		return;

	if (index < 0 || index > l->size)
		return;

	if (index == 0){ 
		fplist_rmfront(l);
		return;
	}

	if (index == l->size - 1){
		fplist_rmback(l);
		return;
	}


	fpnode* it = l->first;
	for (int i = 0; i <= index; i++){
		if (it->next)
			it = it->next;
	}
	if (it){
		it->prev->next = it->next;
		fpnode_destroy(it, l->destroy);
	}	

}
