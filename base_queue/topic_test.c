#include <stdio.h>
#include <stdlib.h>
#include "topic.h"
int test_topic(){
    TopicPtr ptr =  buildTopic("test");
    printf("%s\n",ptr->topicName );
    int data =2;
    MessagePtr mptr= buildMessage(MS_READY,23,1,&data,1,0);
    addMessage(ptr,mptr,0);
    MessagePtr rptr = getReadyMessage(ptr);
    if(rptr == mptr){
        //printf("add right %x\n",rptr);
    }
    rptr = getReadyMessage(ptr);
    if(rptr == NULL){
        printf(" get ready right\n" );
    }else{
        //printf("%x\n",rptr );
    }
    sleepMessage(ptr,mptr->messageid);
    int wake = wakeUpMessage(ptr,mptr->messageid,0);
    if(wake ==1){
        printf("wake finish\n");
    }
    rptr = getReadyMessage(ptr);
    if(rptr == mptr){
        //printf("wark up  right %x\n",rptr);
    }

    reuseMessage(ptr,rptr->messageid,0);
    rptr = getReadyMessage(ptr);
    if(rptr == mptr){
        //printf("reuse  right %x\n",rptr);
    }
    int messageid =rptr->messageid;
    int del = delMessage(ptr,rptr->messageid);
    int reuse = reuseMessage(ptr,messageid,0);
    if(reuse == -1){
        printf("del right\n");
    }
     mptr= buildMessage(MS_DELAY,23,1,&data,1,1);
    addMessage(ptr,mptr,1);
    rptr = getReadyMessage(ptr);
    if(rptr == NULL){
        printf("delay add success\n");
    }
    tickTopic(ptr,nanoseconds());
    rptr = getReadyMessage(ptr);
    if(rptr ==mptr){
        printf("tick right\n");
    }

}