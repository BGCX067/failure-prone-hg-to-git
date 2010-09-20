#ifndef _FPARRAY_H_
#define _FPARRAY_H_

typedef struct _fparray {
    void (*destroy)(void*);
    void* (*create)(void*);

    void** data;
    int elemsize;
    int size; /* Armazena quantos elementos estão no vetor */
    int alloc_size; /* Armazena quantas posições foram alocadas para o vetor */
}fparray;

fparray* fparray_init(void* (*_create)(void*), void (*dest)(void*), int elemsize);
void fparray_destroy(fparray *a);

void* fparray_getdata(int index, fparray *a);

int fparray_insback(void *data, fparray *a);
void fparray_inspos(void* data, int pos, fparray *a);
void fparray_rmback(fparray *a);
void fparray_rmpos(int pos, fparray *a);

#endif
