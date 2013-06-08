#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "../util/util.h"
#include "heap.h"
#include "store.h"

static void set(Heap *h, int k, void *x)
{
    h->data[k] = x;
    if(h->rec!=NULL){
        
        h->rec(x, k);   
    }
}


static void swap(Heap *h, int a, int b)
{
    void *tmp;

    tmp = h->data[a];
    set(h, a, h->data[b]);
    set(h, b, tmp);
}


static int less(Heap *h, int a, int b)
{
    return h->less(h->data[a], h->data[b]);
}


static void siftdown(Heap *h, int k)
{
    for (;;) {
        int p = (k-1) / 2; /* parent */

        if (k == 0 || less(h, p, k)) {
            return;
        }

        swap(h, k, p);
        k = p;
    }
}


static void siftup(Heap *h, int k)
{
    for (;;) {
        int l, r, s;

        l = k*2 + 1; /* left child */
        r = k*2 + 2; /* right child */

        /* find the smallest of the three */
        s = k;
        if (l < h->len && less(h, l, s)) s = l;
        if (r < h->len && less(h, r, s)) s = r;

        if (s == k) {
            return; /* satisfies the heap property */
        }

        swap(h, k, s);
        k = s;
    }
}


// Heapinsert inserts x into heap h according to h->less.
// It returns 1 on success, otherwise 0.
int heapinsert(Heap *h, void *x)
{
    int k;

    if (h->len == h->cap) {
        void **ndata;
        int ncap = (h->len+1) * 2; /* allocate twice what we need */

        ndata = (void**)allocMem(sizeof(void*) * ncap);
        if (!ndata) {
            return 0;
        }

        memcpy(ndata, h->data, sizeof(void*)*h->len);
        freeMem((void**)&(h->data));
        h->data = ndata;
        h->cap = ncap;
    }

    k = h->len;
    h->len++;
    set(h, k, x);
    siftdown(h, k);
    return 1;
}


void * heapremove(Heap *h, int k)
{
    void *x;

    if (k >= h->len) {
        return 0;
    }

    x = h->data[k];
    h->len--;
    set(h, k, h->data[h->len]);
    siftdown(h, k);
    siftup(h, k);
    if(h->rec!=NULL){

        h->rec(x, -1);   
    }
    return x;
}

HeapPtr buildHeap(Record record, Less less){
    HeapPtr ptr = (HeapPtr)allocMem(sizeof(Heap));
    if(ptr == NULL){
        return NULL;
    }
    ptr->cap=ptr->len = 0;
    ptr->data = NULL;
    ptr->rec = record;
    ptr->less = less;
    ptr->storePosition= 0L;
    return ptr;
}
void freeHeap(HeapPtr * h,Free freeMem){
    if(h == NULL){
        return ;
    }
    HeapPtr ptr = *h;
    if(ptr!=NULL){
        if(freeMem!=NULL){
            int i = 0;
            for (i = 0; i < ptr->len; ++i)
            {
                (*freeMem)(&ptr->data[i]);
            }
        }
        freeMem((void**)&(ptr->data));
        freeMem((void**)&ptr);
        (*h) =NULL;
    }

}

int heapFindIndex(HeapPtr h,Find f,void * arg){
    int i = 0;
    for (i ; i < h->len; ++i)
    {
        if(f(h->data[i],arg)){
            return i;
        }
    }
    return -1;
}

long storeHeap(HeapPtr ptr,StoreHandle handle){
	if(ptr==NULL || handle ==NULL){
		return -1L;
	}
	HeapStore heapStore;
	heapStore.cap = ptr->cap;
	heapStore.len = ptr->len;
	heapStore.data= storeHeapData(ptr,handle);
	ptr->storePosition = store(ptr->storePosition,&heapStore,sizeof(HeapStore));
	return ptr->storePosition;
}
static long storeHeapData(HeapPtr ptr,StoreHandle handle){
	if(ptr== NULL || handle ==NULL){
		return -1;
	}
	int offset=0;
	int cap = ptr->cap; 
	if(ptr->storePosition >0){
		//check old store s cap
		HeapStore heapStore;
		restore(ptr->storePosition,&heapStore,sizeof(HeapStore));
		if(heapStore.cap<ptr->cap){
			//it means we need a new store place
		}else{
		// process >
			offset = heapStore.data;
			cap = heapStore.cap;
		}
	}
	long data[cap];
	int i=0;
	for(i=0;i<ptr->len;i++){
		data[i]=handle(ptr->data[i]);
	}
	for(i=ptr->len;i<ptr->cap;i++){
		data[i]=0;
	}
	return store(offset,data,cap);
}
HeapPtr restoreHeap(long storePosition,RestoreHandle handle ,Record record,Less less){
	if(storePosition <=0 || record == NULL|| less == NULL||handle == NULL){
		return NULL;
	}
    HeapStore heapStore;
	restore(storePosition,&heapStore,sizeof(HeapStore));
	HeapPtr ptr= allocMem(sizeof(Heap));
	ptr->cap = heapStore.cap;
	ptr->len = heapStore.len;	
    ptr->data = restoreHeapData(heapStore.data,handle,heapStore.cap);
	ptr->rec =record;
	ptr->less = less;
	ptr->storePosition = storePosition;
    return ptr;
}

static void**  restoreHeapData(long storePosition,RestoreHandle handle,int cap){
	if(storePosition <=0 || handle ==NULL|| cap <=0){
	return NULL;
	}	
	
    void** ndata = (void**)allocMem(sizeof(void*) * cap);
	long data[cap];
	restore(storePosition,data,cap);
	int i=0;
	for(i=0;i<cap;i++){
		if(data[i]<= 0){
			ndata[i] = NULL;
		}else{
			ndata[i]= handle(data[i]);
		}
	}
	return ndata;
}

void* getHeapDataByIndex(HeapPtr h,int index){
    if(index<=0 || h ==NULL || index>=h->len){
        return NULL;
    }else{
        return h->data[index];
    }
}
int getHeapLength(HeapPtr h){
    if(h == NULL){
        return -1;
    }else{
        return h->len;
    }
}