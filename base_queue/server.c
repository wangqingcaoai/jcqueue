#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include "topic.h"
BaseServerPtr buildBaseServer(AppServerPtr app){
    static int baseserver_id;
    
    BaseServerPtr ptr = (BaseServerPtr)allocMem(sizeof(BaseServer));
    if(ptr == NULL){
        return NULL;
    }
    baseserver_id++;
    ptr->baseserver_id = baseserver_id;
    ptr->appServer = app;
    ptr->topicList = buildList(NULL,NULL);
    return ptr;
}

int freeBaseServer(BaseServerPtr * p){
    if(p == NULL){
        return -1;
    }
    BaseServerPtr ptr = (*p);
    if(ptr == NULL){
        return -1;
    }
    ListPtr lptr =ptr->topicList;
    freeList(&lptr,(Free)freeTopic);
    freeMem((void**)&ptr);
    (*p) = NULL;
    
}
int tickBaseServer(BaseServerPtr ptr){
    if(ptr == NULL){
        return -1;
    }
    return tickTopics(ptr);
}