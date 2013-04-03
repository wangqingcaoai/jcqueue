#include "list.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//减小计数
static int  removeOne(ListPtr header){
    if(!checkHeader(header)){
        return LIST_ERROR;
    }
    header->length--;
    return 1;
}
//检查所给的指针是否正确
int checkHeader(ListPtr header){
    if(header == NULL){
        return LIST_ERROR;
    }
    //指针不正确
    if(header->prev==NULL || header->next == NULL){
        return LIST_ERROR;
    }
}
ListPtr initList(Free freeMem, Copy copyData){
    ListPtr header = (ListPtr)malloc(sizeof(List));
    header->prev = header;
    header->next = header;
    header->data = NULL;
    header->freeMem = freeMem;
    header->copyData = copyData;
    header->length = 0;
    header->id=0;
    header->maxCount=0;
    return header;
}

int insertToList(ListPtr header,void *data,int length){
    if(!checkHeader(header)){
        return LIST_ERROR;
    }
    ListPtr new = (ListPtr)malloc(sizeof(List));
    header->prev->next = new;
    new->prev = header->prev;
    header->prev = new;
    new->next=header;

    header->maxCount++;
    header->length++;

    new->freeMem = header->freeMem;
    new->copyData = header->copyData;
   
    new->id = header->maxCount;
    new->maxCount = 0;
    new->length  = 0;

    if(data == NULL){
        new->data = NULL;
    }else{
        //如果没有定义拷贝函数则直接复制内存
        if(new->copyData == NULL){
            new->data=(void*) malloc(length);
            memcpy(new->data,data,length);
        }else{
            //调用拷贝函数进行复制
            new->data=(*(new->copyData))(data);
        }
    }
    new = NULL;
    return 1;
}

void * getFromList(ListPtr header,Find find,void*args[]){
    if(!checkHeader(header)){
        return NULL;
    }
    ListPtr ptr = getPtrFromList(header,find,args);
    if(ptr!=NULL){
        return ptr->data;
    }
    return NULL;
}
ListPtr findNode(ListPtr startNode,ListPtr endNode,Find find , void * args[],ListPosition position ){
    if(!checkHeader(startNode)){
        return NULL;
    }
    if(endNode == NULL){
        endNode = startNode;
    }
    ListPtr ptr =NULL;
    if(position == LIST_NEXT){
        ptr= startNode->next;
    }else{
        ptr=startNode->prev;
    }
    while(ptr!=endNode){
        if(find == NULL){
            break;
        }
        else if((*(find))(ptr->data,args)){
            break;
        };
        if(position == LIST_NEXT){
            ptr= ptr->next;
        }else{
            ptr=ptr->prev;
        }

    }
    if(ptr == endNode || find==NULL){
        return NULL;
    }else{
        return ptr;
    }
}
ListPtr getPtrFromList(ListPtr header,Find find,void*args[]){
    if(!checkHeader(header)){
        return NULL;
    }
    return findNode(header,header,find,args,LIST_NEXT);
    
}

void * nextFromList(ListPtr *node ,ListPtr endNode,Find find,void*args[]){
    if(!checkHeader(*node)){
        return NULL;
    }
    if (endNode ==NULL)
    {
        return NULL;
    }
    if((*node)->next == endNode){
        *node = endNode;
        return NULL;
    }
    if(find ==NULL){
        void* data = (*node)->next->data;
        *node = (*node)->next;
        return data;
    }
    ListPtr ptr= findNode(*node,endNode,find,args,LIST_NEXT);
    if(ptr!=NULL){
        *node = ptr;
        return ptr->data;        
    }else{
        *node = endNode;
        return NULL;
    }
}

void * preFromList(ListPtr *node,ListPtr endNode,Find find,void*args[]){
    if(!checkHeader(*node)){
        return NULL;
    }
    if (endNode ==NULL)
    {
        return NULL;
    }
    if((*node)->prev == endNode){
        *node = endNode;
        return NULL;
    }
    if(find ==NULL){
        void* data = (*node)->prev->data;
        *node = (*node)->prev;
        return data;
    }
    ListPtr ptr= findNode(*node,endNode,find,args,LIST_PRE);
    if(ptr!=NULL){
        *node = ptr;
        return ptr->data;        
    }else{
        *node = endNode;
        return NULL;
    }


}

int removeFromList(ListPtr header,Find find,void*args[]){
    if(!checkHeader(header)){
        return LIST_COUNT_ERROR;
    }
    ListPtr ptr = findNode(header,header,find,args,LIST_NEXT);
    ListPtr prev = NULL;
    int removeNum = 0;
    while(ptr!=NULL){
        ptr->prev ->next = ptr->next;
        ptr->next->prev = ptr->prev;
        prev = ptr->prev;;
        freeNode(&ptr);
        removeNum++;
        removeOne(header);
        //获取下一个
        ptr = findNode(prev,header,find,args,LIST_NEXT);
    }
    return removeNum;    
}

int setToList(ListPtr header ,void * data ,int length,Find find,void*args[]){
    if(!checkHeader(header)){
        return LIST_ERROR;
    }
 
    ListPtr ptr =header->next ;
    int updateNum = 0;
    if(find == NULL){
        return LIST_ERROR;
    }
    while(ptr!=header){
        
        if((*(find))(ptr->data,args)){
            updateNum++;
            if(ptr->freeMem==NULL){
                if(ptr->data!=NULL){
                    free(ptr->data);
                }
            }else{
                if (ptr->data!=NULL)
                {
                    (*(ptr->freeMem))(ptr->data);
                    free(ptr->data);
                }
            }
            if(data == NULL){
                ptr->data = NULL;
            }else{
                //如果没有定义拷贝函数则直接复制内存
                if(ptr->copyData == NULL){
                    ptr->data=(void*) malloc(length);
                    memcpy(ptr->data,data,length);
                }else{
                    //调用拷贝函数进行复制
                    ptr->data=(*(ptr->copyData))(data);
                }
            }       
        };
        //获取下一个
        ptr= ptr->next;
    }
    return updateNum;
}

//会释放根节点
int freeList(ListPtr *header){
    ListPtr head= (*header);
    if(!checkHeader(head)){
        return LIST_ERROR;
    }
    ListPtr ptr = head->next;
    head->next = NULL;
    int freeNum=0;
    ListPtr tmp;
    while(ptr!= NULL){
        freeNum++;
        tmp = ptr->next;
        freeNode(&ptr);
        ptr = tmp;
        tmp = NULL;
    }
    *header=NULL;
    return freeNum;
}

int getCount(ListPtr header){
    if(!checkHeader(header)){
        return LIST_COUNT_ERROR;
    }
    return header->length;
}

int displayList(ListPtr header,Display display,int lineShows){
    if(!checkHeader(header)){
        return LIST_ERROR;
    }
    ListPtr ptr = header->next;
    int count = 0;
    printf("list: length %d,maxCount %d, header id %d\n",
        getCount(header),header->maxCount,header->id);
    printf("%d",header->id );
    while(ptr!=header){
        printf("->%d",ptr->id);
        if (display!=NULL)
        {
            printf("[");
            (*display)(ptr->data,ptr->id);
            printf("]");
        }
        ptr = ptr->next;
        count++;
        if(count%lineShows == 0){
            printf("\n");
        }
    }
    printf("\n");
    return 1;
}

void * getByIdFromList(ListPtr header,int id){
    if(!checkHeader(header)){
        return NULL;
    }
    if(id <= 0){
        return NULL;
    }
    ListPtr ptr = getPtrByIdFromList(header,id);
    if(ptr!=NULL){
        return ptr->data;   
    }else{
        return NULL;
    }
}
ListPtr getPtrByIdFromList(ListPtr header,int id){
    if(!checkHeader(header)){
        return NULL;
    }
    if(id <= 0){
        return NULL;
    }
    ListPtr ptr = header->next;
    while(ptr!=header){
        if(ptr->id == id ){
            break;
        }
        ptr= ptr->next;
    }
    if(ptr == header){
        return NULL;
    }else{
        return ptr;
    }
}

int removeByIdFromList(ListPtr header,int id){
    if(!checkHeader(header)){
        return LIST_COUNT_ERROR;
    }
    if(id<=0){
        return LIST_COUNT_ERROR;
    }
    ListPtr ptr = getPtrByIdFromList(header,id);
    if(ptr == NULL){
        return 0;
    }else{
        freeNode(&ptr);
    }
    return 1;
}
int freeNode(ListPtr *node){
    ListPtr ptr =(*node);
    if(ptr->freeMem==NULL){
        if(ptr->data!=NULL){
            free(ptr->data);
        }
    }else{
        if (ptr->data!=NULL)
        {
            (*(ptr->freeMem))(ptr->data);
            free(ptr->data);
        }
    }
    free(ptr);
    ptr = NULL;
    (*node) ==NULL;
}

int getListNodeId(ListPtr node){
    if(node == NULL){
        return -1;
    }
    return node->id;
}
int isEmptyList(ListPtr header){
    if(header == NULL){
        return 1;
    }
    return header->length ==0;
}


void* getDataOfListNode(ListPtr node){
    if(node == NULL){
        return NULL;
    }
    return node->data;
}

