#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"
void test_buildMessage(){
    int data = 12;
   MessagePtr ptr = buildMessage(MS_READY,1,1,&data,sizeof(data),0);
   int *mdata = getMessageData(ptr);
    MessagePtr ptr2 = buildMessage(MS_READY,1,1,&data,sizeof(data),10);
   
   printf("%d\n", *mdata);
   setMessageState(ptr,MS_DELAY);
   MessageState s = getMessageState(ptr);
   if(MS_DELAY == s){
    printf("state right\n");
   }
   if(MessageLess(ptr,ptr2)){
    printf("less right\n");
   }
   if(isMessageWaitFinished(ptr,nanoseconds())){
    printf("time right\n");
   }
   int64 t = ptr->activetime;
   setMessageDelay(ptr,100);
   if(ptr->activetime-t==100){
    printf("delay right\n");
   }
    freeMessage(&ptr);
    
}