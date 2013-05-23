#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "router.h"
int aq_router(AppServerPtr serverPtr, NetMessagePtr ptr){
    if(serverPtr == NULL){
        aq_empty_appServer(serverPtr,ptr);
    }
    if(ptr == NULL){
        return -1;
    }
    if(ptr->cmd == NULL){
        return aq_unknow_cmd(serverPtr, ptr);
    }
    int result = 0;
    if(strcasecmp(ptr->cmd,AQ_SUBSCRIBE_ADD)){
        result =  aq_subscribe_add(serverPtr , ptr);
    }else if(strcasecmp(ptr->cmd,AQ_SUBSCRIBE_DEL)){
        result =  aq_subscribe_del(serverPtr , ptr);
    }else{
        result = bq_router(serverPtr->baseServer, ptr);
        //aq_filter(serverPtr,ptr,result);
    }
    return result;
    
}
static aq_empty_appServer(AppServerPtr serverPtr, NetMessagePtr ptr){

}
static aq_unknow_cmd(AppServerPtr serverPtr, NetMessagePtr ptr){

}
static aq_subscribe_add(AppServerPtr serverPtr, NetMessagePtr ptr){

}

static aq_subscribe_del(AppServerPtr serverPtr, NetMessagePtr ptr){

}
static aq_system_state(AppServerPtr serverPtr, NetMessagePtr ptr){

}
int aq_client_router(AppServerPtr serverPtr, NetMessagePtr ptr){
    if(serverPtr == NULL){
        aq_empty_appServer(serverPtr,ptr);
    }
    if(ptr == NULL){
        return -1;
    }
    if(ptr->cmd == NULL){
        return aq_unknow_cmd(serverPtr, ptr);
    }
    int result = 0;
    return result;
    
}