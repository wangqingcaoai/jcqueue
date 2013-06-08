#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include "topic.h"
#include "../app_queue/server.h"
BaseServerPtr buildBaseServer(AppServerPtr app){
    static int baseserver_id;
    
    BaseServerPtr ptr = (BaseServerPtr)allocMem(sizeof(BaseServer));
    if(ptr == NULL){
        return NULL;
    }
    baseserver_id++;
    ptr->baseserver_id = baseserver_id;
    ptr->appServer = app;
    ptr->topicList = buildList();
    ptr->storePosition = 0L;
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

long storeBaseServer(BaseServerPtr ptr){
    if(ptr == NULL){
        return -1L;
    }
    BaseServerStore bstore;
    if(ptr->storePosition >0){
        restore(ptr->storePosition,&bstore,sizeof(bstore));
    }else{
        bstore.topicList  = 0L;
    }
    bstore.baseserver_id = ptr->baseserver_id;
    bstore.topicList = storeList(ptr->topicList,(StoreHandle)storeTopic);
    bstore.appServer = ptr->appServer->storePosition;
    return ptr->storePosition = store(ptr->storePosition,&bstore,sizeof(bstore));
}
BaseServerPtr restoreBaseServer(long storePosition,AppServerPtr server){
    if(storePosition <=0){
        return NULL;
    }

    BaseServerStore bstore;
    restore(storePosition,&bstore,sizeof(bstore));
    BaseServerPtr ptr = (BaseServerPtr)allocMem(sizeof(BaseServer));
    ptr->baseserver_id =bstore.baseserver_id ;
    ptr->topicList = restoreList(bstore.topicList,(RestoreHandle)restoreTopic);
    ptr->appServer = server;
    ptr->storePosition = storePosition;
    return ptr;
}