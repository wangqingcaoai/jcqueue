#ifndef DATA_HEAP_H
#define DATA_HEAP_H

#include "base.h"
typedef struct Heap {
    int     cap;
    int     len;
    void    **data;
    Less    less;
    Record  rec;
}Heap,* HeapPtr;
int heapinsert(HeapPtr h, void *x);
void * heapremove(HeapPtr h, int k);
HeapPtr buildHeap(Record record, Less less);
void freeHeap(HeapPtr *h,Free freeMem);
int heapFindIndex(HeapPtr h,Find f,void * arg);

#endif