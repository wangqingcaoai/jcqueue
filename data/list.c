
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "list.h"
//减小计数
static int  removeOne(ListPtr list){
    if(!checklist(list)){
        return LIST_ERROR;
    }
    list->count--;
    return 1;
}
//检查所给的指针是否正确
int checklist(ListPtr list){
    if(list == NULL){
        return LIST_ERROR;
    }
    if(list->header == NULL){
        return LIST_ERROR;
    }
    //指针不正确
    if(list->header->prev==NULL || list->header->next == NULL){
        return LIST_ERROR;
    }
}
ListPtr buildList(){
    static int list_id;
    
    ListPtr list = (ListPtr)allocMem(sizeof(List));
    if(list == NULL){
        return NULL;
    }
    
    list->header = (ListNodePtr)allocMem(sizeof(ListNode));
    if(list->header == NULL){
        freeMem((void**)&list);
        return NULL;
    }
    list_id ++;
    list->count  = 0;
    list->listId = list_id; 
    list->header->id= LIST_HEADER_ID;
    list->header->prev = list->header->next = list->header;
    list->header->data = NULL;
    return list;
}

int insertToList(ListPtr list,void *data){
    if(!checklist(list)){
        return LIST_ERROR;
    }
    ListNodePtr header = list->header;
    ListNodePtr new = (ListNodePtr)allocMem(sizeof(ListNode));
    if(new == NULL){
        return LIST_ERROR_MEM_ALLOC_FAILED;
    }
    header->prev->next = new;
    new->prev = header->prev;
    header->prev = new;
    new->next=header;

    list->maxCount++;
    list->count++;
   
    new->id = list->maxCount;
    new->data = data;
    new = NULL;
    return 1;
}

void * getFromList(ListPtr list,Find find,void*arg){
    if(!checklist(list)){
        return NULL;
    }
    ListNodePtr ptr = getPtrFromList(list,find,arg);
    if(ptr!=NULL){
        return ptr->data;
    }
    return NULL;
}
ListNodePtr findNode(ListNodePtr startNode,ListNodePtr endNode,Find find , void * arg,ListPosition position ){
    if(startNode==NULL||startNode->prev==NULL||startNode->next==NULL){
        return NULL;
    }
    if(find == NULL){
        return NULL;
    }
    if(endNode == NULL){
        endNode = startNode;
    }
    ListNodePtr ptr =NULL;
    if(position == LIST_NEXT){
        ptr= startNode->next;
    }else{
        ptr=startNode->prev;
    }
    while(ptr!=endNode){
        if(find == NULL){
            break;
        }
        else if((*(find))(ptr->data,arg)){
            break;
        };
        if(position == LIST_NEXT){
            ptr= ptr->next;
        }else{
            ptr=ptr->prev;
        }

    }
    if(ptr == endNode){
        return NULL;
    }else{
        return ptr;
    }
}
ListNodePtr getPtrFromList(ListPtr list,Find find,void*arg){
    if(!checklist(list)){
        return NULL;
    }
    return findNode(list->header,list->header,find,arg,LIST_NEXT);
    
}

void * nextFromList(ListNodePtr *node ,ListNodePtr endNode,Find find,void*arg){
    ListNodePtr startNode = (*node); 
    if(startNode==NULL||startNode->prev==NULL||startNode->next==NULL){
        return NULL;
    }
    if (endNode ==NULL)
    {
        return NULL;
    }
    if(startNode->next == endNode){
        (*node) = endNode;
        return NULL;
    }
    if(find ==NULL){
        void* data = startNode->next->data;
        (*node) = startNode->next;
        return data;
    }
    ListNodePtr ptr= findNode(startNode,endNode,find,arg,LIST_NEXT);
    if(ptr!=NULL){
        (*node) = ptr;
        return ptr->data;        
    }else{
        (*node) = endNode;
        return NULL;
    }
}

void * preFromList(ListNodePtr *node,ListNodePtr endNode,Find find,void*arg){
    ListNodePtr startNode = (*node); 
    if(startNode==NULL||startNode->prev==NULL||startNode->next==NULL){
        return NULL;
    }
    if (endNode ==NULL)
    {
        return NULL;
    }
    if(startNode->prev == endNode){
        *node = endNode;
        return NULL;
    }
    if(find ==NULL){
        void* data = startNode->prev->data;
        *node = startNode->prev;
        return data;
    }
    ListNodePtr ptr= findNode(startNode,endNode,find,arg,LIST_PRE);
    if(ptr!=NULL){
        *node = ptr;
        return ptr->data;        
    }else{
        *node = endNode;
        return NULL;
    }


}

int removeFromList(ListPtr list,Find find,void*arg,Free freeHandle){
    if(!checklist(list)){
        return LIST_COUNT_ERROR;
    }
    ListNodePtr ptr = findNode(list->header,list->header,find,arg,LIST_NEXT);
    ListNodePtr prev = NULL;
    int removeNum = 0;
    while(ptr!=NULL){
        ptr->prev ->next = ptr->next;
        ptr->next->prev = ptr->prev;
        prev = ptr->prev;;
        freeListNode(&ptr,freeHandle);
        removeNum++;
        removeOne(list);
        //获取下一个
        ptr = findNode(prev,list->header,find,arg,LIST_NEXT);
    }
    return removeNum;    
}

// int setToList(ListPtr list ,void * data ,int count,Find find,void*arg){
//     if(!checklist(list)){
//         return LIST_ERROR;
//     }
 
//     ListPtr ptr =list->next ;
//     int updateNum = 0;
//     if(find == NULL){
//         return LIST_ERROR;
//     }
//     while(ptr!=list){
        
//         if((*(find))(ptr->data,arg)){
//             updateNum++;
//             if(ptr->freeHandle==NULL){
//                 if(ptr->data!=NULL){
//                     free(ptr->data);
//                 }
//             }else{
//                 if (ptr->data!=NULL)
//                 {
//                     (*(ptr->freeHandle))(ptr->data);
//                     free(ptr->data);
//                 }
//             }
//             if(data == NULL){
//                 ptr->data = NULL;
//             }else{
//                 //如果没有定义拷贝函数则直接复制内存
//                 if(ptr->copyData == NULL){
//                     ptr->data=(void*) allocMem(count);
//                     memcpy(ptr->data,data,count);
//                 }else{
//                     //调用拷贝函数进行复制
//                     ptr->data=(*(ptr->copyData))(data);
//                 }
//             }       
//         };
//         //获取下一个
//         ptr= ptr->next;
//     }
//     return updateNum;
// }

//会释放根节点
int freeList(ListPtr *l,Free freeHandle){
    ListPtr list= (*l);
    if(!checklist(list)){
        return LIST_ERROR;
    }
    ListNodePtr ptr = list->header->next;
    list->header->next = NULL;
    int freeNum=0;
    ListNodePtr tmp;
    while(ptr!= NULL){
        freeNum++;
        tmp = ptr->next;
        freeListNode(&ptr,freeHandle);
        ptr = tmp;
        tmp = NULL;
    }
    freeMem((void**)&list);
    (*l)=NULL;
    list =NULL;
    return freeNum;
}

int getCountOfList(ListPtr list){
    if(!checklist(list)){
        return LIST_COUNT_ERROR;
    }
    return list->count;
}

int displayList(ListPtr list,Display display,int lineShows){
    if(!checklist(list)){
        return LIST_ERROR;
    }
    ListNodePtr header = list->header;
    ListNodePtr ptr = header->next;
    int count = 0;
    printf("list: count %d,maxCount %d, \n",
        list->count,list->maxCount);
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

void * getByIdFromList(ListPtr list,int id){
    if(!checklist(list)){
        return NULL;
    }
    if(id <= 0){
        return NULL;
    }
    ListNodePtr ptr = getPtrByIdFromList(list,id);
    if(ptr!=NULL){
        return ptr->data;   
    }else{
        return NULL;
    }
}
ListNodePtr getPtrByIdFromList(ListPtr list,int id){
    if(!checklist(list)){
        return NULL;
    }
    if(id <= 0){
        return NULL;
    }
    ListNodePtr header =list->header;
    ListNodePtr ptr = header->next;
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

int removeByIdFromList(ListPtr list,int id,Free freeHandle){
    if(!checklist(list)){
        return LIST_COUNT_ERROR;
    }
    if(id<=0){
        return LIST_COUNT_ERROR;
    }
    ListNodePtr ptr = getPtrByIdFromList(list,id);
    if(ptr == NULL){
        return 0;
    }else{
        freeListNode(&ptr,freeHandle);
    }
    return 1;
}
int freeListNode(ListNodePtr *node,Free freeHandle){
    ListNodePtr ptr =(*node);
    if(freeHandle==NULL){
        if(ptr->data!=NULL){
            //free(ptr->data);
            //并不知道传入的是分配的内存还是 何处的指针
            ptr->data = NULL;
        }
    }else{
        if (ptr->data!=NULL)
        {
            (*(freeHandle))(&ptr->data);
        }
    }
    freeMem((void**)&ptr);
    ptr = NULL;
    (*node) ==NULL;
}

int getListNodeId(ListNodePtr node){
    if(node == NULL){
        return -1;
    }
    return node->id;
}
int isEmptyList(ListPtr list){
    if(list == NULL){
        return 1;
    }
    return list->count ==0;
}


void* getDataOfListNode(ListNodePtr node){
    if(node == NULL){
        return NULL;
    }
    return node->data;
}

ListNodePtr getListHeader(ListPtr ptr){
    if(ptr == NULL){
        return NULL;
    }
    return ptr->header;
}
ListNodePtr getListEnd(ListPtr ptr){
    if(ptr == NULL){
        return NULL;
    }
    return ptr->header;
}

void* popFromList(ListPtr list){
    if(list==NULL){
        return NULL;
    }
    if(isEmptyList(list)){
        return NULL;
    }
    ListNodePtr header = list->header;

    ListNodePtr temp = header->next;
    header->next =header->next->next;
    header->next->prev = header;
    void* data =temp->data;
    freeListNode(&temp,NULL);
    list->count--;
    return data;
}
int pushToList(ListPtr ptr, void*data){
    return insertToList(ptr,data);
}

long storeList(ListPtr ptr,long storePosition,StoreHandle handle){

	
}
