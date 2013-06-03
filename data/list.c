
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "list.h"
#include "store.h"
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
    list->header->storePosition = 0;
    list->storePosition = 0;
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
    new->storePosition = 0;
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

long storeList(ListPtr ptr,StoreHandle handle){
    if(ptr == NULL || handle == NULL){
        return -1;
    }

    ListStore lstore;
    lstore.listId = ptr->listId;
    lstore.header = storeListNodes(ptr->header,handle);
    lstore.count = ptr->count;
    lstore.maxCount = ptr->maxCount;
    ptr->storePosition = store(ptr->storePosition,&lstore,sizeof(ListStore));
    return ptr->storePosition;
}
ListPtr restoreList(long storePosition,RestoreHandle handle){
    if(storePosition <=0 || handle == NULL ){
        return NULL;
    }
    ListStore lstore;
    int result = restore(storePosition,&lstore,sizeof(ListStore));
    if(result != STORE_SUCCESS){
        return NULL;
    }else{
        ListPtr ptr = (ListPtr)allocMem(sizeof(List));
        ptr->listId = lstore.listId;
        ptr->header = restoreListNodes(lstore.header,handle);
        ptr->count = lstore.count ;
        ptr->maxCount = lstore.maxCount;
        ptr->storePosition = storePosition;
        return ptr; 
    }
}
long storeListNodes(ListNodePtr header,StoreHandle handle){
    if(header == NULL||handle == NULL){
        return -1;
    }
    long headerPosition = storeListNode(header,handle);
    ListNodePtr ptr = header->next;
    // it means a empty list
    if(ptr == header){
        storeListNode(header,handle);
    }else{
            //first store we can just write the prev position into store;
        while(ptr != header){
            storeListNode(ptr,handle);
            ptr =ptr->next;
        }
    }
    return headerPosition;
    

}
ListNodePtr restoreListNodes(long storePosition,RestoreHandle handle){
    ListNodeStore sheader;
    ListNodePtr header = restoreListNode(storePosition,&sheader,handle);
    long position = sheader.next;
    ListNodeStore temp;
    ListNodePtr prev = header;
    ListNodePtr current = header;
    while(position!=storePosition&&position>0){
        current = restoreListNode(position,&temp,handle);
        prev->next = current;
        current->prev = prev;
        position = temp.next;
    }
    current->next = header;
    header->prev = current;
    return header;
}

long storeListNode(ListNodePtr ptr,StoreHandle handle){
    if(ptr == NULL ||handle == NULL){
        return -1;
    }
    ListNodeStore node;
    node.prev = ptr->prev->storePosition;
    node.next = ptr->next->storePosition;
    node.id =  ptr->id;
    if(ptr->data == NULL){
        node.data = 0;
    }else{
        node.data = handle(ptr->data);
    }
    ptr->storePosition = store(ptr->storePosition,&node,sizeof(ListNodeStore));
    
    if(ptr->prev->storePosition>0){
        ListNodeStore prev;
        restore(ptr->prev->storePosition,&prev,sizeof(ListNodeStore));
        if(prev.next!=ptr->storePosition){
            prev.next = ptr->storePosition;
            store(ptr->prev->storePosition,&prev,sizeof(ListNodeStore));
        }
    }
    if(ptr->next->storePosition>0){
        ListNodeStore next;
        restore(ptr->next->storePosition,&next,sizeof(ListNodeStore));
        if(next.prev!=ptr->storePosition){
            next.prev = ptr->storePosition;
            store(ptr->next->storePosition,&next,sizeof(ListNodeStore));
        }
    }
    return ptr->storePosition;
}
ListNodePtr restoreListNode(long storePosition,ListNodeStorePtr node, RestoreHandle handle){
    if(storePosition<=0 || handle == NULL ||node == NULL){
        return NULL;
    }
    restore(storePosition,node,sizeof(ListNodeStore));
    ListNodePtr listNode = allocMem(sizeof(ListNode));
    listNode->prev = NULL;
    listNode->next = NULL;
    listNode->id = node->id;
    if(node->data <=0){
        listNode->data = NULL;
    }else{
        listNode->data = handle(node->data);    
    }
    listNode->storePosition = storePosition;
    return listNode;
} 