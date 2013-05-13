#include <stdio.h>
#include <stdlib.h>
#include "subscribe.h"
SubscribeServerPtr buildSubscribeServer(AppServerPtr appServer){
    static int server_id;
    server_id++;
    SubscribeServerPtr ptr = malloc(sizeof(SubscribeServer));
    ptr->serverId = server_id;
    ptr->subscribeTopics = bulidList();
    ptr->subscribes = bulidList();
    ptr->appServer = appServer;
    return ptr;

}
int freeSubscribeServer(SubscribeServerPtr *pptr){
    if(pptr==NULL||(*pptr)==NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    SubscribeServerPtr ptr = (*pptr);
    freeList(&(ptr->subscribeTopics),NULL);
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
    ptr->pusher = buildPusher(ptr);
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
    freePusher(ptr);
    free(ptr);
    ptr = NULL;
    (*pptr) = NULL;
    return SUBSCRIBE_SUCCESS;
}

SubscribeTopicPtr bulidSubScribeTopic(const char* topicName,TopicPtr tptr){
    static int id;
    if(isEmptyString(topicName)){
        return NULL;
    }
    if(tptr == NULL){
        return NULL;
    }
    SubscribeTopicPtr ptr = malloc(sizeof(SubscribeTopic));
    ptr->id =  id;
    ptr->topicName = allocString(topicName);
    ptr->topic = tptr;
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

int isSubscribeTopicById(SubscribeTopicPtr ptr,int id){
    if(ptr == NULL){
        return 0;
    }
    if(ptr->id == id){
        return 1;
    }else{
        return 0;
    }
}
int isSubscribeTopicByTopicName(SubscribeTopicPtr ptr,const char* topicName){
    if(ptr == NULL){
        return 0;
    }
    return isSameString(ptr->topicName,topicName);
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
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"no appServer defined on subscribeServer, could not find topic");
        return NULL;
    }
    ListPtr topicList = getTopicListByKeyword(server->appServer->baseServer,keyword);
    if(topicList!=NULL){
        if(!isEmptyList(topicList)){
            //将topic信息更新到topic列表当中，
            addSubscribeTopicsByList(server, topicList,  ptr);
        }
        freeList(&topicList,NULL);
    }
    return SUBSCRIBE_SUCCESS;

} 
int delSubscribe(SubscribeServerPtr server,int  subscribeId){
    if(server == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(server->subscibes == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    int removeNum =removeFromList(server->subscibes,(Find)isSubscribeById,subscribeId,(Free)freeSubscribe);
    if(removeNum == 0){
        return SUBSCRIBE_ERROR_SUBSCRIBE_NOT_FOUND;
    }else{
        return SUBSCRIBE_SUCCESS;
    }


}
int delSubscribeByUser(SubscribeServerPtr server,UserPtr UserPtr,NetMessagePtr){

}
int delSubscribebyKeywordAndUser(SubscribeServerPtr server,UserPtr userPtr,NetMessagePtr netPtr){

}
int getSubscribeIdsByUser(SubscribeServerPtr server){

}
int getSubscribeTopicIdsByUser(SubscribeServerPtr server){

}

int addSubscribeTopic(SubscribeServerPtr subscribeServer,TopicPtr tptr, SubscribePtr subscribe){
    if(subscribeServer == NULL || tptr == NULL || subscribe == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(subscribeServer->subscribeTopics == NULL){
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to add subscibetopic on SubscribeServer(serverID:%d) ,list is NULL",subscribeServer->serverId);
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }else{
        subscribeTopic ptr = getFromList(subscribeServer->subscribeTopics,(Find)isSubscribeTopicByTopicName,tptr->topicName);
        if(ptr == NULL){
            ptr = bulidSubScribeTopic(tptr->topicName,tptr);
            if(ptr == NULL){
                addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to add subscibetopic on SubscribeServer(serverID:%d) ,list is NULL",subscribeServer->serverId);
                return SUBSCRIBE_BUILD_SUBSCRIBE_TOPIC_FAILED;
            }
            insertIntoList(subscribeServer->subscribeTopics,ptr);
        }
        addSubscribeToSubscribeTopic(ptr,subscribe);
        addSubscribeTopicToSubscribe(subscibe,ptr);
    }
    return SUBSCRIBE_SUCCESS;
}
int addSubscribeTopicsByList(SubscribeServerPtr subscribeServer,ListPtr topicList, SubscribePtr subscribe){
    if(subscribeServer == NULL  || subscribe == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(topicList == NULL || isEmptyList(topicList)){
        return SUBSCRIBE_SUCCESS;
    }
    ListNodePtr start = getListHeader(topicList);
    ListNodePtr end = getListEnd(topicList);
    if(start==NULL || end == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }else{
        TopicPtr tptr = nextFromList(&start,end,NULL,NULL);
        while(tptr!= NULL){
            addSubscribeTopic(subscribeServer,tptr,subscribe);
            tptr = nextFromList(&start,end,NULL,NULL);
        }
    }
    return SUBSCRIBE_SUCCESS;
}
int delSubscribeTopic(SubscribeServerPtr subscribeServer,const char* topicName, SubscribePtr subscribe){

}
int processSubscribeTopic(SubscribeServerPtr server){
    if(server == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(server->subscribeTopics == NULL){
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to process subscibetopic on SubscribeServer(ID:%d) ,subscribeTopics list is NULL",server->serverId);
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    ListNodePtr start = getListHeader(server->subscribeTopics);
    ListNodePtr end = getListEnd(server->subscribeTopics);
    SubScribeTopicPtr stptr = nextFromList(&start,end,NULL,NULL);
    MessagePtr mptr = NULL;
    while(stptr!=NULL){
        //获取就绪消息
        mptr = getReadyMessage(stptr->topic);
        if(mptr == NULL){
            //没有就绪的消息 不做处理
        }else{
            //有就绪消息，发送到每个的推送器
            pushMessageToSubscribeList(server,stptr->subscribesList,mptr);
        }
        stptr = nextFromList(&start,end,NULL,NULL);
    }

}//need add Push server 
int addSubscribeToSubscribeTopic(SubscribeTopicPtr ptr,SubscribePtr sptr){
    if (ptr == NULL || sptr == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(ptr->subscribesList == NULL){
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to add subscibe to subscibetopic on SubscribeTopic(ID:%d,topicName:%s) ,subscribesList is NULL",ptr->id,ptr->topicName);
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    insertIntoList(ptr->subscribesList,sptr);
    return SUBSCRIBE_SUCCESS;
}
int addSubscribeTopicToSubscribe(SubscribePtr ptr,SubscribeTopicPtr sptr){
    if (ptr == NULL || sptr == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(ptr->subscribedTopicLists == NULL){
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to add subscibetopic to subscibe on Subscribe(ID:%d,keyword:%s) ,subscribedTopicLists is NULL",ptr->id,ptr->subscribeKeyWord);
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    insertIntoList(ptr->subscribedTopicLists,sptr);
    return SUBSCRIBE_SUCCESS;
}

int UpdateSubscribeAfterAddTopic(SubscribeServerPtr server, const char*topicName){
    if(server == NULL || topicName == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(server->subscribes == NULL){
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to update subscribe after add new topic, subscribeserver's subscribes is null");
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(server->appServer == NULL){
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to update subscribe after add new topic, subscribeserver's appServer is null");
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(server->appServer->baseServer == NULL){
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to update subscribe after add new topic, subscribeserver's baseServer is null");
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    ListNodePtr start = getListHeader(server->subscribes);
    ListNodePtr end = getListEnd(server->subscibes);
    TopicPtr tptr = useTopic(server->appServer->baseServer,topicName);
    SubScribePtr sptr = nextFromList(&start,end,(Find)isMatchSubscribeByTopicName,topicName);
    while(sptr!=NULL){
        addSubscribeTopic(server,tptr,sptr);
        sptr = nextFromList(&start,end,(Find)isMatchSubscribeByTopicName,topicName);
    }
    return SUBSCRIBE_SUCCESS;
}
int isMatchSubscribeByTopicName(SubscribePtr ptr ,const char* topicName){
    if(ptr == NULL || topicName ==NULL){
        return 0;
    }else if(REGEX_SUCCESS == isMatchedString(topicName,ptr->subscribeKeyWord)){
        return 1;
    }else{
        return  0;
    }
}

int UpdateSubscribeAfterRemoveTopic(SubscribeServerPtr server, const char*topicName){
    if(server == NULL || topicName == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(server->subscribes == NULL){
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to update subscribe after add new topic, subscribeserver's subscribes is null");
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(server->appServer == NULL){
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to update subscribe after add new topic, subscribeserver's appServer is null");
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    int removeNum =removeFromList(server->subscribeTopics,(Find)isSubscribeTopicByTopicName,topicName,(Free)freeSubscribeTopic);
    if(removeNum == 0){
        return SUBSCRIBE_ERROR_SUBSCRIBE_TOPIC_NOT_FOUND;
    }else{
        return SUBSCRIBE_SUCCESS;
    }

    return SUBSCRIBE_SUCCESS;
}

int pushMessageToSubscribeList(SubscribeServerPtr server,ListPtr subscribes,MessagePtr message){
    if(server ==NULL || subscibes == NULL ||message ==NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }

    ListNodePtr start = getListHeader(subscribes);
    ListNodePtr end = getListEnd(subscibes);
    SubScribePtr sptr = nextFromList(&start,end,NULL,NULL);
    while(sptr!=NULL){
        addMessageToPusher(sptr->pusher,message);
        sptr = nextFromList(&start,end,NULL,NULL);
    }
    return SUBSCRIBE_SUCCESS;
}
