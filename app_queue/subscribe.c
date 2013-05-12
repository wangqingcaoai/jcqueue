#include <stdio.h>
#include <stdlib.h>
#include "subscribe.h"
SubscribeServerPtr buildSubscribeServer(AppServerPtr appServer){
    static int server_id;
    server_id++;
    SubscribeServerPtr ptr = malloc(sizeof(SubscribeServer));
    ptr->serverId = server_id;
    ptr->topics = bulidList();
    ptr->subscribes = bulidList();
    ptr->appServer = appServer;
    return ptr;

}
int freeSubscribeServer(SubscribeServerPtr *pptr){
    if(pptr==NULL||(*pptr)==NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    SubscribeServerPtr ptr = (*pptr);
    freeList(&(ptr->topics),NULL);
    freeList(&(ptr->subscribes),(Free)freeSubscribe);
    free(ptr);
    ptr =NULL;
    (*pptr)=NULL;
    return SUBSCRIBE_SUCCESS;
}
SubscribePtr bulidSubScribe(const char* subscribeKeyWord,const char* remoteHost,const int remotePort,const char* protocol,const char* type, UserPtr user){
    if(isEmptyString(subscribeKeyWord)||isEmptyString(remoteHost)||isEmptyString(protocol)||isEmptyString(type)){
        return NULL;
    }
    if(remotePort<=0 || remotePort >65535){
        return NULL;
    }
    if(user ==NULL){
        return NULL;
    }
    static int subscribe_id;
    subscribe_id++;
    SubscribePtr ptr = malloc(sizeof(subscribe));
    ptr->subscribeId = subscribe_id;
    ptr->subscribeKeyWord = allocString(subscribeKeyWord);
    ptr->remoteHost = allocString(remoteHost);
    ptr->remotePort = remotePort;
    ptr->protocol = allocString(protocol);
    ptr->type = allocString(type);
    ptr->user = user;
    ptr->channel = NULL;
    ptr->subscribedTopicLists = bulidList();
    return ptr;
}
int freeSubscribe(SubscribePtr *pptr){
    if(pptr==NULL||(*pptr)==NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    SubscribePtr ptr = (*pptr);
    ListNodePtr lptr = getPtrFromList(ptr->subscribedTopicLists,NULL,NULL);
    ListNodePtr lptrEnd =  lptr;
    if(lptr!=NULL){
        //当subscribe 被释放时，在对应的subscribeTopic中的列表中移除该项 TODO 检查逻辑
        SubscribeTopicPtr sptr = lptr->data;
        while(sptr!=NULL){
            int removeNum = removeFromList(sptr->subscribesList,(Find)isSubscribeById,&(ptr->id),NULL);
            sptr = nextFromList(&lptr,lptrEnd,NULL,NULL);
        }
    }
    freeString(&(ptr->subscribeKeyWord));
    freeString(&(ptr->remoteHost));
    freeString(&(ptr->protocol));
    freeString(&(ptr->type));
    ptr->user = NULL;
    ptr->channel =NULL;

    freeList(&(ptr->subscribedTopicLists,NULL));//free

    free(ptr);
    ptr = NULL;
    (*pptr) = NULL;
    return SUBSCRIBE_SUCCESS;
}

SubscribeTopicPtr bulidSubScribeTopic(const char* topicName){
    static int id;
    if(isEmptyString(topicName)){
        return NULL;
    }
    SubscribeTopicPtr ptr = malloc(sizeof(SubscribeTopic));
    ptr->id =  id;
    ptr->topicName = allocString(topicName);
    ptr->subscribesList = bulidList();
    return ptr;


}
int freeScribeTopic(SubscribeTopicPtr *pptr){
    if(pptr==NULL||(*pptr)==NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    SubscribeTopicPtr ptr = (*pptr);
    ListNodePtr lptr = getPtrFromList(ptr->subscribesList,NULL,NULL);
    ListNodePtr lptrEnd =  lptr;
    if(lptr!=NULL){
        //当subscribeTopic 被释放时，在对应的subscribe中的列表中移除该项 TODO 检查逻辑
        SubscribePtr sptr = lptr->data;
        while(sptr!=NULL){
            int removeNum = removeFromList(sptr->subscribedTopicLists,(Find)isSubscribeTopicById,&(ptr->id),NULL);
            sptr = nextFromList(&lptr,lptrEnd,NULL,NULL);
        }
    }
    freeList(&(ptr->subscribesList),NULL);
    freeString(&(ptr->topicName));
    free(ptr);
    ptr = NULL;
    (*pptr) = NULL;
    return SUBSCRIBE_SUCCESS;
}
int addSubscribe(SubscribeServerPtr server, UserPtr userPtr , NetMessagePtr netMessage){
    if(server == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(user == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(netMessage == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    char * keyword = getEntraParam(netMessage,"keyword");
    SubscribePtr ptr = bulidSubScribe( keyword,netMessage->remoteHost,const int remotePort,const char* protocol,const char* type, UserPtr user);
    if(ptr == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    insertIntoList(server->subscribes,ptr);
    if(server->appServer == NULL){
        addLog(LOG_ERROR,"no appServer defined on subscribeServer, could not find topic");
        return NULL;
    }
    ListPtr topicList = getTopicListByKeyword(server->appServer->baseServer,keyword);
    if(topicList!=NULL||(!isEmptyList(topicList))){
        //addSubscribeTopic(server,)//TODO
    }
    
int delSubscribe(SubscribeServerPtr server,int  subscribeId);
int delSubscribeByUser(SubscribeServerPtr server,UserPtr UserPtr,NetMessagePtr);
int getSubscribeIdsByUser(SubscribeServerPtr server);
int getSubscribeTopicIdsByUser(SubscribeServerPtr server);
