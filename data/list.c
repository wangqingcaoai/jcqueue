
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
    static int listnode_id;
    listnode_id ++;
    ListPtr list = (ListPtr)malloc(sizeof(List));
    list->header = (ListNodePtr)malloc(sizeof(ListNode));
    list->count = 0;
    list->header->id=listnode_id;
    list->header->prev = list->header->next = list->header;
    return list;
}

int insertToList(ListPtr list,void *data){
    if(!checklist(list)){
        return LIST_ERROR;
    }
    ListNodePtr header = list->header;
    ListNodePtr new = (ListNodePtr)malloc(sizeof(ListNode));
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

int removeFromList(ListPtr list,Find find,void*arg,Free freeMem){
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
        freeNode(&ptr,freeMem);
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
//             if(ptr->freeMem==NULL){
//                 if(ptr->data!=NULL){
//                     free(ptr->data);
//                 }
//             }else{
//                 if (ptr->data!=NULL)
//                 {
//                     (*(ptr->freeMem))(ptr->data);
//                     free(ptr->data);
//                 }
//             }
//             if(data == NULL){
//                 ptr->data = NULL;
//             }else{
//                 //如果没有定义拷贝函数则直接复制内存
//                 if(ptr->copyData == NULL){
//                     ptr->data=(void*) malloc(count);
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
int freeList(ListPtr *l,Free freeMem){
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
        freeNode(&ptr,freeMem);
        ptr = tmp;
        tmp = NULL;
    }
    free(l);
    (*l)=NULL;
    list =NULL;
    return freeNum;
}

int getCount(ListPtr list){
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

int removeByIdFromList(ListPtr list,int id,Free freeMem){
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
        freeNode(&ptr,freeMem);
    }
    return 1;
}
int freeNode(ListNodePtr *node,Free freeMem){
    ListNodePtr ptr =(*node);
    if(freeMem==NULL){
        if(ptr->data!=NULL){
            free(ptr->data);
        }
    }else{
        if (ptr->data!=NULL)
        {
            (*(freeMem))(&ptr->data);
        }
    }
    free(ptr);
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

