#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"

MessagePtr buildMessage(MessageState state,int64 timestamp, int priority,void* data,int length,int delay){
    static int messageid;
    
    MessagePtr ptr = (MessagePtr)allocMem(sizeof(Message));
    if(ptr == NULL){
        return NULL;
    }
    messageid++;
    ptr->messageid = messageid;
    ptr->state = state;
    ptr->timestamp = timestamp;
    ptr->createtime =nanoseconds();//TODO;
    ptr->priority = priority;
    ptr->length = length;
    ptr->delay = delay;
    ptr->activetime = ptr->createtime+delay;
    if(length > 0){
        ptr->data = (void*)allocMem(length);
        if(ptr->data != NULL){
            memcpy(ptr->data,data,length);
        }else{
            free(ptr);
            messageid --;
            ptr = NULL;
        }
        
    }else{
        ptr->data = NULL;
        ptr->length = 0;
    }
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
    free(ptr);
    (*p)=ptr=NULL;
    return 1; 
}

void * getMessageData(MessagePtr ptr){
    return ptr->data;
}
MessageState getMessageState(MessagePtr ptr){
    return ptr->state;
}
int MessageLess(MessagePtr ptr1, MessagePtr ptr2){
    if(ptr1->activetime<ptr2->activetime){
        return 1;
    }else if(ptr1->activetime = ptr2->activetime){
        if(ptr1->priority<=ptr2->priority){
            return 1;
        }
    }
    return 0;
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
int isMessage(MessagePtr ptr,int* messageid){  
    if(messageid == NULL){
        return 0;
    }
    if(ptr == NULL){
        return 0;
    }
    int id = (int)(*messageid);  
    if(ptr->messageid == id){
        return 1;
    }
    return 0;
}