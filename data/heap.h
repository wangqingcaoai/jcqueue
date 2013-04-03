#ifndef HEAP_H
#define HEAP_H

typedef int(*Less)(void*, void*);
typedef void(*Record)(void*, int);
typedef struct Heap {
    int     cap;
    int     len;
    void    **data;
    Less    less;
    Record  rec;
}Heap;
int heapinsert(Heap *h, void *x);
void * heapremove(Heap *h, int k);

#endif