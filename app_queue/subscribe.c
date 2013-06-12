#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "push.h"
#include "server.h"
#include "../base_queue/topic_router.h"
#include "../base_queue/topic.h"
#include "../base_queue/server.h"
#include "../util/log.h"
#include "../util/regex.h"
#include "../util/util.h"
#include "../data/list.h"
#include "subscribe.h"
#include "../parser/net_message.h"
#include "../data/store.h"
#include "../util/maxids.h"
SubscribeServerPtr buildSubscribeServer(AppServerPtr appServer){
    
    SubscribeServerPtr ptr = (SubscribeServerPtr)allocMem(sizeof(SubscribeServer));
    if(ptr == NULL){
        return ptr;
    }
    ptr->serverId = getSubscribeServerNextId();
    ptr->subscribeTopics = buildList();
    ptr->subscribes = buildList();
    ptr->appServer = appServer;
    ptr->storePosition = 0L;
    return ptr;

}
int freeSubscribeServer(SubscribeServerPtr *pptr){
    if(pptr==NULL||(*pptr)==NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    SubscribeServerPtr ptr = (*pptr);
    freeList(&(ptr->subscribeTopics),NULL);
    freeList(&(ptr->subscribes),(Free)freeSubscribe);
    freeMem((void**)&ptr);
    (*pptr)=NULL;
    return SUBSCRIBE_SUCCESS;
}
SubscribePtr buildSubscribe(const char* subscribeKeyWord,const char* remoteHost,const int remotePort,const char* protocol,const char* type, UserPtr user){
    if(isEmptyString(subscribeKeyWord)||isEmptyString(remoteHost)||isEmptyString(protocol)||isEmptyString(type)){
        return NULL;
    }
    if(remotePort<=0 || remotePort >65535){
        return NULL;
    }
    if(user ==NULL){
        return NULL;
    }
    
    SubscribePtr ptr = (SubscribePtr)allocMem(sizeof(Subscribe));
    if(ptr == NULL){
        return ptr;
    }
    ptr->subscribeId = getSubscribeNextId();
    ptr->subscribeKeyWord = allocString(subscribeKeyWord);
    ptr->remoteHost = allocString(remoteHost);
    ptr->remotePort = remotePort;
    ptr->protocol = allocString(protocol);
    ptr->type = allocString(type);
    ptr->user = user;
    ptr->channel = NULL;
    ptr->subscribedTopicLists = buildList();
    ptr->pusher = buildPusher(ptr);
    ptr->storePosition = 0;
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
            int removeNum = removeFromList(sptr->subscribesList,(Find)isSubscribeById,&(ptr->subscribeId),NULL);
            sptr = nextFromList(&lptr,lptrEnd,NULL,NULL);
        }
    }
    freeString(&(ptr->subscribeKeyWord));
    freeString(&(ptr->remoteHost));
    freeString(&(ptr->protocol));
    freeString(&(ptr->type));
    ptr->user = NULL;
    ptr->channel =NULL;

    freeList(&(ptr->subscribedTopicLists),NULL);//free
    //freePusher(&(ptr->pusher));
    freeMem((void**)&ptr);
    (*pptr) = NULL;
    return SUBSCRIBE_SUCCESS;
}

SubscribeTopicPtr buildSubScribeTopic(const char* topicName,TopicPtr tptr){
    
    if(isEmptyString(topicName)){
        return NULL;
    }
    if(tptr == NULL){
        return NULL;
    }
    SubscribeTopicPtr ptr = (SubscribeTopicPtr)allocMem(sizeof(SubscribeTopic));
    if(ptr == NULL){
        return ptr;
    }
    
    ptr->id =  getSubscribeTopicNextId();
    ptr->topicName = allocString(topicName);
    ptr->topic = tptr;
    ptr->subscribesList = buildList();
    ptr->storePosition =0L;
    return ptr;


}
int freeSubscribeTopic(SubscribeTopicPtr *pptr){
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
    freeMem((void**)&ptr);
    (*pptr) = NULL;
    return SUBSCRIBE_SUCCESS;
}
int isSubscribeById(SubscribePtr ptr,const int64 *id){
    if(ptr == NULL ||id == NULL){
        return 0;
    }
    if(ptr->subscribeId = (*id)){
        return 1;
    }else{
        return 0;
    }
}
int isSubscribeTopicById(SubscribeTopicPtr ptr,const int64* id){
    if(ptr == NULL || id == NULL){
        return 0;
    }
    if(ptr->id == (*id)){
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
    if(userPtr == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(netMessage == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    char * keyword = getExtraParam(netMessage,"keyword");
    char * remoteHost = getExtraParam(netMessage,"remoteHost");
    char * remotePort = getExtraParam(netMessage,"remotePort");
    char * protocol = getExtraParam(netMessage,"protocol");
    char* type = getExtraParam(netMessage ,"type");
    int result = SUBSCRIBE_SUCCESS;
    if(isEmptyString(keyword)||
        isEmptyString(remoteHost)||
        isEmptyString(remotePort)
        ){
        result = SUBSCRIBE_ERROR_PARAM_EMPTY;
    }else{
        if(isEmptyString(protocol)){
            protocol = allocString("jcq");
        }
        if(isEmptyString(type)){
            type = allocString("push");
        }
        
        SubscribePtr ptr = buildSubscribe( keyword,remoteHost,atoi(remotePort), protocol, type,userPtr);
        char buf[UTIL_NUM_BUF_SIZE];
        setSendExtraParam(netMessage,"subscribe_id",int64ToString(ptr->subscribeId,buf,UTIL_NUM_BUF_SIZE));
       
        if(ptr == NULL){
            result =SUBSCRIBE_ERROR_PARAM_ERROR;
        }else {
            insertToList(server->subscribes,ptr);
            addPusher(server->appServer->pushServer,ptr);
            if(server->appServer == NULL){
                addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"no appServer defined on subscribeServer, could not find topic");
                result =  SUBSCRIBE_ERROR_PARAM_ERROR;
                
            }else{
                ListPtr topicList = getTopicListByKeyword(server->appServer->baseServer,keyword);
                if(topicList!=NULL){
                    if(!isEmptyList(topicList)){
                        //将topic信息更新到topic列表当中，
                        printf("add topic\n");
                        printf("add topic list to subscribe %"PRId64"\n",ptr->subscribeId );
                        addSubscribeTopicsByList(server, topicList,  ptr);
                    }
                    freeList(&topicList,NULL);
                }       
            }

        }    
    }
    
    freeString(&keyword);
    freeString(&remoteHost);
    freeString(&remotePort);
    freeString(&protocol);
    freeString(&type);
    return result;

} 
int delSubscribe(SubscribeServerPtr server,UserPtr userPtr,NetMessagePtr mptr){
    if(server == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(server->subscribes == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }

    char buf[UTIL_NUM_BUF_SIZE];
    char* param = getExtraParam(mptr,"subscribe_id");
    int64 subscribe_id = atoll(param);
    SubscribePtr sub = getFromList(server->subscribes,(Find)isSubscribeById,&subscribe_id);
    if(sub!=NULL){
        removePusherFromPushServer(server->appServer->pushServer,sub);
    }
    int removeNum =removeFromList(server->subscribes,(Find)isSubscribeById,&subscribe_id,(Free)freeSubscribe);
    if(removeNum == 0){
        return SUBSCRIBE_ERROR_SUBSCRIBE_NOT_FOUND;
    }else{
        return SUBSCRIBE_SUCCESS;
    }
}
int delSubscribeByUser(SubscribeServerPtr server,UserPtr userPtr,NetMessagePtr netMessage){

}
int delSubscribebyKeywordAndUser(SubscribeServerPtr server,UserPtr userPtr,NetMessagePtr netPtr){

}
int getSubscribeIdsByUser(SubscribeServerPtr server,UserPtr userPtr,NetMessagePtr nmptr){

}
int getSubscribeTopicIdsByUser(SubscribeServerPtr server,UserPtr userPtr,NetMessagePtr nmptr){

}
int getSubscribeInfoById(SubscribeServerPtr server,UserPtr userPtr,NetMessagePtr nmptr){

}


int addSubscribeTopic(SubscribeServerPtr subscribeServer,TopicPtr tptr, SubscribePtr subscribe){
    if(subscribeServer == NULL || tptr == NULL || subscribe == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(subscribeServer->subscribeTopics == NULL){
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to add subscribetopic on SubscribeServer(serverID:%d) ,list is NULL",subscribeServer->serverId);
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }else{
        SubscribeTopicPtr ptr = getFromList(subscribeServer->subscribeTopics,(Find)isSubscribeTopicByTopicName,tptr->topicName);
        if(ptr == NULL){
            ptr = buildSubScribeTopic(tptr->topicName,tptr);
            if(ptr == NULL){
                addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to add subscribetopic on SubscribeServer(serverID:%d) ,list is NULL",subscribeServer->serverId);
                return SUBSCRIBE_BUILD_SUBSCRIBE_TOPIC_FAILED;
            }
            printf("add new subscribetopic %s\n", tptr->topicName);
            insertToList(subscribeServer->subscribeTopics,ptr);
        }
        addSubscribeToSubscribeTopic(ptr,subscribe);
        addSubscribeTopicToSubscribe(subscribe,ptr);
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
        //addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to process subscribetopic on SubscribeServer(ID:%d) ,subscribeTopics list is NULL",server->serverId);
        //return SUBSCRIBE_ERROR_PARAM_ERROR;
        return SUBSCRIBE_SUCCESS;
    }
    ListNodePtr start = getListHeader(server->subscribeTopics);
    ListNodePtr end = getListEnd(server->subscribeTopics);
    SubscribeTopicPtr stptr = nextFromList(&start,end,NULL,NULL);
    MessagePtr mptr = NULL;
    while(stptr!=NULL){
        //获取就绪消息
        mptr = getReadyMessage(stptr->topic);
        
        if(mptr == NULL){
            //没有就绪的消息 不做处理
        }else{
            //有就绪消息，发送到每个的推送器
           // if(isOn(getConfig("verbose","off"))){
                printf("send \"%s\" 's message %"PRId64" to  pusher client \n",stptr->topic->topicName,mptr->messageid );
            //}
            pushMessageToSubscribeList(server,stptr->subscribesList,mptr,stptr->topic);
        }
        stptr = nextFromList(&start,end,NULL,NULL);
    }


}//need add Push server 
int addSubscribeToSubscribeTopic(SubscribeTopicPtr ptr,SubscribePtr sptr){
    if (ptr == NULL || sptr == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(ptr->subscribesList == NULL){
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to add subscribe to subscribetopic on SubscribeTopic(ID:%d,topicName:%s) ,subscribesList is NULL",ptr->id,ptr->topicName);
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    insertToList(ptr->subscribesList,sptr);
    return SUBSCRIBE_SUCCESS;
}
int addSubscribeTopicToSubscribe(SubscribePtr ptr,SubscribeTopicPtr sptr){
    if (ptr == NULL || sptr == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    if(ptr->subscribedTopicLists == NULL){
        addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,SUBSCRIBE_POSITION_NAME,"failed to add subscribetopic to subscribe on Subscribe(ID:%d,keyword:%s) ,subscribedTopicLists is NULL",ptr->subscribeId,ptr->subscribeKeyWord);
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    insertToList(ptr->subscribedTopicLists,sptr);
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
    ListNodePtr end = getListEnd(server->subscribes);
    TopicPtr tptr = useTopic(server->appServer->baseServer,topicName);
    SubscribePtr sptr = nextFromList(&start,end,(Find)isMatchSubscribeByTopicName,(char*)topicName);
    while(sptr!=NULL){
        addSubscribeTopic(server,tptr,sptr);
        printf("update subscribe %"PRId64" after add topic %s\n",sptr->subscribeId,topicName);
        sptr = nextFromList(&start,end,(Find)isMatchSubscribeByTopicName,(char*)topicName);
    }
    return SUBSCRIBE_SUCCESS;
}
int isMatchSubscribeByTopicName(SubscribePtr ptr ,const char* topicName){
    if(ptr == NULL || topicName ==NULL){
        return 0;
    }
    else if(!strcmp(topicName,ptr->subscribeKeyWord)){
        return 1;
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
    int removeNum =removeFromList(server->subscribeTopics,(Find)isSubscribeTopicByTopicName,(char*)topicName,(Free)freeSubscribeTopic);
    printf("update subscribe  after remove topic %s\n",topicName);
    if(removeNum == 0){
        return SUBSCRIBE_ERROR_SUBSCRIBE_TOPIC_NOT_FOUND;
    }else{
        return SUBSCRIBE_SUCCESS;
    }

    return SUBSCRIBE_SUCCESS;
}

int pushMessageToSubscribeList(SubscribeServerPtr server,ListPtr subscribes,MessagePtr message,TopicPtr topic){
    if(server ==NULL || subscribes == NULL ||message ==NULL || topic == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }

    ListNodePtr start = getListHeader(subscribes);
    ListNodePtr end = getListEnd(subscribes);
    SubscribePtr sptr = nextFromList(&start,end,NULL,NULL);
    while(sptr!=NULL){
        printf("push messageid %"PRId64" to  subscribe  %d\n",message->messageid,sptr->subscribeId);
        
        addMessageToPusher(sptr->pusher,message,topic);
        message->relateCount++;
        sptr = nextFromList(&start,end,NULL,NULL);
    }
    return SUBSCRIBE_SUCCESS;
}

int tickSubscribeServer(SubscribeServerPtr ptr){
    return processSubscribeTopic(ptr);
}


long storeSubscribe(SubscribePtr ptr){
    if(ptr == NULL){
        return -1L;
    }
    SubscribeStore subStore;
    if(ptr->storePosition >0){
        restore(ptr->storePosition,&subStore,sizeof(SubscribeStore));
    }else{
        subStore.subscribeKeyWord = 0L;
        subStore.remoteHost = 0L;
        subStore.protocol = 0L;
        subStore.type = 0L;
        subStore.user =0L;
        subStore.channel = 0L;
        subStore.subscribedTopicLists = 0L;
        subStore.pusher = 0L;
    }
    subStore.subscribeId = ptr->subscribeId;
    subStore.subscribeKeyWord = storeString(subStore.subscribeKeyWord,ptr->subscribeKeyWord,SUBSCRIBE_MAX_SUBSCRIBE_KEYWORDS_SIZE);
    subStore.remoteHost = storeString(subStore.remoteHost,ptr->remoteHost,SUBSCRIBE_MAX_SUBSCRIBE_REMOTEHOST_SIZE);
    subStore.remotePort = ptr->remotePort;
    subStore.protocol = storeString(subStore.protocol,ptr->protocol,SUBSCRIBE_MAX_SUBSCRIBE_PROTOCOL_SIZE);
    subStore.type = storeString(subStore.type,ptr->type,SUBSCRIBE_MAX_SUBSCRIBE_TYPE_SIZE);
    subStore.user = ptr->user->storePosition;
    subStore.channel = 0L;
    subStore.subscribedTopicLists = 0L;// it can be rebuild by  data
    subStore.pusher = ptr->pusher->storePosition;
    return ptr->storePosition = store(ptr->storePosition,&subStore,sizeof(subStore));

}
SubscribePtr restoreSubscribe(long storePosition){
    if(storePosition <=0){
        return NULL;
    }
    SubscribeStore subStore;

    restore(storePosition,&subStore,sizeof(SubscribeStore));
    SubscribePtr ptr = (SubscribePtr)allocMem(sizeof(Subscribe));
    if(ptr == NULL){
        return ptr;
    }
    ptr->subscribeId = subStore.subscribeId;
    ptr->subscribeKeyWord = restoreString(subStore.subscribeKeyWord);
    ptr->remoteHost = restoreString(subStore.remoteHost);
    ptr->remotePort = subStore.remotePort;
    ptr->protocol = restoreString(subStore.protocol);
    ptr->type = restoreString(subStore.type);
    ptr->user = restoreUser(subStore.user);
    ptr->channel = NULL;
    ptr->subscribedTopicLists = buildList();
    ptr->pusher = buildPusher(ptr);
    ptr->pusher->storePosition =  subStore.pusher;
    ptr->storePosition = storePosition;
    return ptr;

}
long storeSubscribeServer(SubscribeServerPtr ptr){
    if(ptr == NULL){
        return -1L;
    }
    SubscribeServerStore serverStore;
    serverStore.serverId = ptr->serverId;
    serverStore.subscribes = storeList(ptr->subscribes,(StoreHandle)storeSubscribe);
    serverStore.appServer = ptr->appServer->storePosition;
    return ptr->storePosition = store(ptr->storePosition,&serverStore,sizeof(SubscribeServerStore));    

}
SubscribeServerPtr restoreSubscribeServer(long storePosition,AppServerPtr appServer){
    if(storePosition <=0 ||appServer == NULL){
        return NULL;
    }
    SubscribeServerStore serverStore;
    restore(storePosition,&serverStore,sizeof(SubscribeServerStore));
    SubscribeServerPtr  ptr = (SubscribeServerPtr)allocMem(sizeof(SubscribeServer));
    ptr->serverId = serverStore.serverId;
    ptr->appServer = appServer;
    ptr->subscribes = restoreList(serverStore.subscribes,(RestoreHandle)restoreSubscribe); 
    initSubscribes(ptr->subscribes);
    return ptr;
}
int initSubscribes(SubscribeServerPtr server){
    if(server == NULL){
        return SUBSCRIBE_ERROR_PARAM_ERROR;
    }
    ListNodePtr start = getListHeader(server->subscribes);
    ListNodePtr end = getListEnd(server->subscribes);
    SubscribePtr sptr = nextFromList(&start,end,NULL,NULL);
    while(sptr!=NULL){
        UserPtr temp = sptr->user;
        sptr->user = findUserByStorePosition(server->appServer->usersList,temp->storePosition);
        freeMem((void**)&temp);
        ListPtr topicList = getTopicListByKeyword(server->appServer->baseServer,sptr->subscribeKeyWord);
        if(topicList!=NULL){
            if(!isEmptyList(topicList)){
                //将topic信息更新到topic列表当中，
                addSubscribeTopicsByList(server, topicList,  sptr);
            }
            freeList(&topicList,NULL);
        }
        sptr = nextFromList(&start,end,NULL,NULL);
    }
    return SUBSCRIBE_SUCCESS;
}

SubscribePtr findSubscribeByStorePosition(SubscribeServerPtr server, long storePosition){
    if(server == NULL || storePosition <=0){
        return NULL;
    }
    return getFromList(server->subscribes,(Find)isSubscribeByStorePosition,&storePosition);
}
int isSubscribeByStorePosition(SubscribePtr ptr, long* storePosition){
    if(ptr== NULL || storePosition == NULL || (*storePosition)<=0){
        return 0;
    }
    return ptr->storePosition == (*storePosition);
}

ListPtr getAllPushingMessagesBySubscribe(SubscribePtr ptr){
    if(ptr == NULL){
        return NULL;
    }
    ListNodePtr start = getListHeader(ptr->subscribedTopicLists);
    ListNodePtr end = getListEnd(ptr->subscribedTopicLists);
    SubscribeTopicPtr sptr = nextFromList(&start,end,NULL,NULL);
    ListPtr list = buildList();
    ListPtr using = NULL;
    while(sptr!=NULL){
        using = getUsingMessages(sptr->topic);
        ListNodePtr usingStart = getListHeader(using);
        ListNodePtr usingEnd = getListEnd(using);
        MessagePtr mptr = nextFromList(&usingStart,usingEnd,NULL,NULL);
        while(mptr != NULL){
            insertToList(list,buildPusherMessage(mptr,sptr->topic));    
            mptr = nextFromList(&usingStart,usingEnd,NULL,NULL);
        }
        freeList(&using,NULL);
        sptr = nextFromList(&start,end,NULL,NULL);
    }
    return list;
       
}