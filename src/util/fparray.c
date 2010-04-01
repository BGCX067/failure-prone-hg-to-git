#include "fparray.h"
#include "malloc.h"
#include "stdlib.h"

#define INIT_SIZE 8

fparray* fparray_init(void* (*_create)(void*), void (*dest)(void*), int elemsize) {
    fparray *vec = (fparray*) dlmalloc(sizeof(fparray));

    vec->create = _create;
    vec->destroy = dest;
    vec->alloc_size = INIT_SIZE;
    vec->size = 0;
    vec->elemsize = elemsize;
    vec->data = dlmalloc(INIT_SIZE*vec->elemsize);
    memset(vec->data, 0, INIT_SIZE*vec->elemsize);
    return vec;
}

void fparray_destroy(fparray *a) {
    for(int i = 0; i < a->size; i++)
        a->destroy(a->data[i]);
    dlfree(a->data);
    dlfree(a);
    a = NULL;
}

void* fparray_getdata(int index, fparray *a) {
    if (index < 0)
    	return NULL;
    return a->data[index];
}

int fparray_insback(void *data, fparray *a) {
    if(a->size < a->alloc_size) {
        if(a->create)
            a->data[a->size] = a->create(data);
        else
            a->data[a->size] = data;
    } else {
        /* Aloca mais espaço pro array */
        a->alloc_size *= 2;
        dlrealloc(a->data, a->elemsize*a->alloc_size);
        if(a->create)
            a->data[a->size] = a->create(data);
        else
            a->data[a->size] = data;
    }
    a->size++;

    return a->size - 1;
}


/* FIXME incrementar o tamanho até ser possível inserir */
void fparray_inspos(void* data, int pos, fparray *a) {
    if(a->size + 1 == a->alloc_size) {
        /* Aloca mais espaço pro array */
        a->alloc_size *= 2;
        dlrealloc(a->data, a->elemsize*a->alloc_size);
    }
    
    if(a->create)
        a->data[pos] = a->create(data);
    else
        a->data[pos] = data;
    
    if(pos >= a->size)
        a->size = pos + 1;
    else
        a->size++;
}

void fparray_rmback(fparray *a) {
    a->destroy(a->data[a->size - 1]);
    
    a->size--;

    if (a->size < a->alloc_size/4) {
        /* FIXME testar se o realloc realmente funciona
         * nesse caso. */
        a->alloc_size /= 2; 
        dlrealloc(a, a->alloc_size);
    }
}

void fparray_rmpos(int pos, fparray *a) {
    a->destroy(a->data[pos]);

    for(int i = pos; i < a->size - 1; i++)
        a->data[i] = a->data[i + 1];
    a->size--;

    if (a->size < a->alloc_size/4) {
        /* FIXME testar se o realloc realmente funciona
         * nesse caso. */
        a->alloc_size /= 2; 
        dlrealloc(a, a->alloc_size);
    }
}

