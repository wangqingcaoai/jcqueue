#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include "topic.h"
BaseServerPtr buildBaseServer(){
    static int baseserver_id;
    baseserver_id++;
    BaseServerPtr ptr = malloc(sizeof(BaseServer));
    ptr->baseserver_id = baseserver_id;
    ptr->topicList = buildList(NULL,NULL);
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
    
}