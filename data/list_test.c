#include "list.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
typedef struct T
{
    int x;
    int y;
}T;
int find(void* data,void* args[]){
    T* test = (T*) data;
    if(test==NULL){
        return 0;
    }
    int *x =(int*)args[0];
    //printf("%d => %d\n",*x,test->x );
    if((*x)==test->x){
        return 1;
    } 
    return 0;
}
int display(void* data,int id){
    T* test =(T*)data;
    printf("id:%d",id);
    if(test == NULL){
        printf("NULL");
    }else{
        printf("x->%d,y->%d",test->x,test->y );
    }
}

int test(){
    T t ;
    t.x =1;
    t.y =2;
    T t1 = {3,4};
    Find f =&find;
    ListPtr list = initList(NULL,NULL);
    insertToList(list, &t, sizeof(t));
    insertToList(list, &t1, sizeof(t1));
    int a =1; 
    void*args[0];
    args[0]=&a;

    T* s = getFromList(list,f,args);
    //printf("%d %d\n", s->y, t.y);

    insertToList(list, &t1,sizeof(t1));
    insertToList(list, &t1,sizeof(t1));
    insertToList(list, &t1,sizeof(t1));
    insertToList(list, &t1,sizeof(t1));
    insertToList(list, &t1,sizeof(t1));
    a = 1;
    T* r = getFromList(list,f,args);
    printf("%d\n", r->y);
    printf("count %d\n", getCount(list));
    displayList(list,display,4);
    printf("remove %d\n",removeFromList(list,f,args));
    displayList(list,display,4);
    r = getFromList(list,f,args);
    //printf("%d\n", r);
    assert(r==NULL);
    displayList(list,display,4);
    t1.y=10;
    a = 3;
    int count = setToList(list,&t1,sizeof(t1),f,args);
    printf("update %d\n", count);
    displayList(list,display,4);
    ListPtr ptr = list;
    int i = 0;
    do{
        void * data = NULL;
        if(i>4){
            data= preFromList(&ptr,list,f,args);
        }else{
            data = nextFromList(&ptr,list,f,args);
        }
        if(ptr != list){
            display(data,ptr->id);   
            printf("\n");
        }
        i++;
    }while(ptr!=list);
    //displayList(list,display,4);

    int num = freeList(&list);
    printf("%d\n", num);

}