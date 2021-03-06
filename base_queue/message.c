#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"
#include "../util/maxids.h"

MessagePtr buildMessage(MessageState state,int64 timestamp, int priority,void* data,int length,int delay){
    
    
    MessagePtr ptr = (MessagePtr)allocMem(sizeof(Message));
    if(ptr == NULL){
        return NULL;
    }
    
    ptr->messageid = getMessageNextId();
    ptr->state = state;
    ptr->timestamp = timestamp;
    ptr->createtime =nanoseconds();//TODO;
    ptr->priority = priority;
    ptr->length = length;
    ptr->delay = delay;
    ptr->relateCount =0;
    ptr->activetime = ptr->createtime+delay;
    if(length > 0){
        ptr->data = (void*)allocMem(length);
        if(ptr->data != NULL){
            memcpy(ptr->data,data,length);
        }else{
            freeMem((void**)&ptr);
        }
        
    }else{
        ptr->data = NULL;
        ptr->length = 0;
    }
    ptr->storePosition = 0L;
    return ptr;
}
int setMessageState(MessagePtr ptr, MessageState state){
    ptr->state = state;
}
MessagePtr cloneMessage(MessagePtr oldPtr){
    return buildMessage(oldPtr->state,oldPtr->timestamp,oldPtr->priority,oldPtr->data,oldPtr->length,oldPtr->delay);
}
//释放消息
int freeMessage(MessagePtr *p){
    if (p==NULL)
    {
        return -1;
    }
    MessagePtr ptr = (*p);
    if(ptr== NULL){
        return -1;
    }
    if(ptr->length > 0 ){
        free(ptr->data);
        ptr->data = NULL;
    }
    freeMem((void**)&ptr);
    (*p)=NULL;
    return 1; 
}

void * getMessageData(MessagePtr ptr){
    return ptr->data;
}
MessageState getMessageState(MessagePtr ptr){
    return ptr->state;
}
int MessageLessByPriority(MessagePtr ptr1, MessagePtr ptr2){
    if(ptr1->priority<=ptr2->priority){
            return 1;
    }
    return 0;    
}
int MessageLessByActiveTime(MessagePtr ptr1, MessagePtr ptr2){

    if(ptr1->activetime<ptr2->activetime){
        return 1;
    }else if(ptr1->activetime = ptr2->activetime){
        if(ptr1->priority<=ptr2->priority){
            return 1;
        }
        return 0; 
    }
}
void MessageRecord(MessagePtr ptr, int i){
    //did nothing;
}

void setMessageDelay(MessagePtr ptr,int delay){
    ptr->activetime += delay;
    ptr->delay = delay;
}
int isMessageWaitFinished(MessagePtr ptr,int64 timestamp){
   if(ptr->activetime <= timestamp){
        return 1;
   } 
   return 0;
}
//判断消息id 是否等于传入的id
int isMessage(MessagePtr ptr,int64* messageid){  
    if(messageid == NULL){
        return 0;
    }
    if(ptr == NULL){
        return 0;
    }
    int64 id = (int64)(*messageid);  
    if(ptr->messageid == id){
        return 1;
    }
    return 0;
}
long storeMessage(MessagePtr ptr){
    if(ptr== NULL ){
        return -1L;
    }
    MessageStore mstore;
    if(ptr->storePosition > 0){
        restore(ptr->storePosition,&mstore,sizeof(MessageStore));
    }else{
        mstore.data = 0L; 
    }
    mstore.messageid = ptr->messageid;
    mstore.state = ptr->state;
    mstore.timestamp = ptr->timestamp;
    mstore.createtime = ptr->createtime;
    mstore.priority = ptr->priority;
    mstore.length = ptr->length;
    mstore.delay = ptr->delay;
    mstore.relateCount = ptr->relateCount;
    mstore.activetime = ptr->activetime;
    mstore.data = store(mstore.data,ptr->data,ptr->length);
    ptr->storePosition  = store(ptr->storePosition,&mstore,sizeof(MessageStore));
    return ptr->storePosition;
}
MessagePtr restoreMessage(long storePosition){
    if(storePosition <=0){
        return NULL;
    }
    MessageStore mstore;
    restore(storePosition,&mstore,sizeof(MessageStore));
    MessagePtr ptr = (MessagePtr)allocMem(sizeof(Message));
    ptr->messageid = mstore.messageid;
    ptr->state = mstore.state;
    ptr->timestamp = mstore.timestamp;
    ptr->createtime = mstore.createtime;
    ptr->priority = mstore.priority;
    ptr->length =  mstore.length;
    ptr->delay =mstore.delay;
    ptr->relateCount = mstore.relateCount;
    ptr->activetime = mstore.activetime;
    ptr->storePosition = storePosition;
    ptr->data = allocMem(mstore.length);
    restore(mstore.data,ptr->data,mstore.length);
    return ptr;
}