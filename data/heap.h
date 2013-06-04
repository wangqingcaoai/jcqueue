#ifndef DATA_HEAP_H
#define DATA_HEAP_H

#include "base.h"
typedef struct Heap {
    int     cap;
    int     len;
    void    **data;
    Less    less;
    Record  rec;
	long storePosition;
}Heap,* HeapPtr;
typedef struct HeapStore{
	int cap;
	int len;
	long data;
}HeapStore,*HeapStorePtr;
int heapinsert(HeapPtr h, void *x);
void * heapremove(HeapPtr h, int k);
HeapPtr buildHeap(Record record, Less less);
void freeHeap(HeapPtr *h,Free freeMem);
int heapFindIndex(HeapPtr h,Find f,void * arg);
long storeHeap(HeapPtr ptr,StoreHandle handle);
HeapPtr restoreHeap(long storePosition,RestoreHandle handle, Record record,Less less);

static long storeHeapData(HeapPtr ptr,StoreHandle handle);
static void**  restoreHeapData(long storePosition,RestoreHandle handle,int cap);
#endif
