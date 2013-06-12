#include <stdio.h>
#include <stdlib.h>
#include "../data/list.h"
#include "push.h"
#include "../errors.h"
#include "../util/maxids.h"
PushServerPtr buildPushServer(AppServerPtr server){
    if(server == NULL){
        return NULL;
    }
    PushServerPtr ptr = (PushServerPtr)allocMem(sizeof(PushServer));
    if(ptr==NULL){
        return ptr;
    }
    
    ptr->appServer = server;
    ptr->serverId = getPusherServerNextId();
    ptr->pushers = buildList();
    ptr->storePosition = 0L;
    return ptr;
}
int freePushServer(PushServerPtr * pptr){
    if(pptr == NULL){
        return PUSH_PARAM_ERROR;
    }
    PushServerPtr ptr = *pptr;
    if (ptr == NULL)
    {
        return PUSH_PARAM_ERROR;
    }
    freeList(&(ptr->pushers),(Free)freePusher);
    freeMem((void**)&ptr);
    (*pptr)==NULL;
    return PUSH_SUCCESS;
}
PusherPtr buildPusher(SubscribePtr subscribe){
    
    if(subscribe == NULL){
        return NULL;
    }
    
    PusherPtr ptr = (PusherPtr)allocMem(sizeof(Pusher));
    
    if(ptr == NULL){
        return ptr;
    }
    ptr->pusherId = getPusherNextId();
    ptr->subscribe = subscribe;
    ptr->messageReady = buildList();
    ptr->messagePushed = buildList();
    ptr->connect = NULL;
    ptr->storePosition = 0L;
    return ptr; 
    
}
int freePusher(PusherPtr * pptr){
    if(pptr == NULL){
        return PUSH_PARAM_ERROR;
    }
    PusherPtr ptr = (*pptr);
    freeList(&(ptr->messageReady),NULL);
    connectClose(ptr->connect);
    //removeConnect(ptr->connect);

    freeMem((void**)pptr);
    ptr = NULL;
}
int addPusher(PushServerPtr server,SubscribePtr subscribe){
    if(server == NULL||subscribe==NULL){
        return PUSH_PARAM_ERROR;
    }
    insertToList(server->pushers,subscribe->pusher);
    return PUSH_SUCCESS;
}

int removePusherFromPushServer(PushServerPtr server,SubscribePtr subscribe){
    if(server == NULL){
        return PUSH_PARAM_ERROR;
    }
    removeFromList(server->pushers,(Find)isSameSubscribe,subscribe,NULL);
    
}
int addMessageToPusher(PusherPtr ptr,MessagePtr message,TopicPtr topic){
    if(ptr == NULL || message == NULL|| topic == NULL){
        return PUSH_PARAM_ERROR;
    }
  printf("add message %"PRId64" to push (%d) readymessage  \n",message->messageid,ptr->pusherId );  
    PusherMessagePtr pusherMessage = buildPusherMessage(message,topic);
    insertToList(ptr->messageReady,pusherMessage);
    return PUSH_SUCCESS;
}
int pushToTarget(PushServerPtr server, PusherPtr ptr){
    if(ptr == NULL || server == NULL){
        return PUSH_PARAM_ERROR;
    }
    char buf[UTIL_NUM_BUF_SIZE];
    int bufSize = UTIL_NUM_BUF_SIZE;
    if(ptr->connect == NULL){
        ptr->connect = buildRequestConnect(server->appServer->transfarServer,ptr->subscribe->remoteHost,intToString(ptr->subscribe->remotePort,buf,bufSize),
            server->appServer->pusherResponse,server->appServer->pusherRequest
            );
        ptr->connect->user = ptr->subscribe->user;
        ptr->connect->netMessage->sendState = NETMESSAGE_WRITESTATE_FINISH;    
        ptr->connect->relateData = ptr;
    }
    if(ptr->connect->netMessage->sendState == NETMESSAGE_WRITESTATE_FINISH){
        PusherMessagePtr pmPtr= popFromList(ptr->messageReady);
        if(pmPtr == NULL){
            ptr->connect->netMessage->sendState = NETMESSAGE_WRITESTATE_FINISH;
            
        }else{
            MessagePtr mPtr = pmPtr->message;
            TopicPtr tPtr = pmPtr->topic;
            if(mPtr == NULL || tPtr == NULL){
                ptr->connect->netMessage->sendState = NETMESSAGE_WRITESTATE_FINISH;
        
            }else{
                NetMessagePtr nmptr = ptr->connect->netMessage;
                if(ptr->connect->netMessage->protocolType ==NULL){
                    ptr->connect->netMessage->protocolType = NETMESSAGE_TYPE_JCQ;
                }
                if(ptr->connect->netMessage->version ==NULL){
                    ptr->connect->netMessage->version = NETMESSAGE_VERSION_JCQ;
                }
                setNetMessageSendData(nmptr,buildErrorCode(PUSH_SUCCESS_MARK,PUSH_PUSH_SUCCESS),PUSH_TO_CLIENT_CMD,mPtr->data,mPtr->length);
                setSendExtraParam(nmptr,"topic_name",tPtr->topicName);

                setSendExtraParam(nmptr,"message_id",int64ToString(mPtr->messageid,buf,bufSize));
                
                setSendExtraParam(nmptr,"priority",intToString(mPtr->priority,buf,bufSize));

                setSendExtraParam(nmptr,"delay",intToString(mPtr->delay,buf,bufSize));

                setSendExtraParam(nmptr,"activetime",int64ToString(mPtr->activetime,buf,bufSize));

                setSendExtraParam(nmptr,"timestamp",int64ToString(mPtr->timestamp,buf,bufSize));
                ptr->connect->netMessage->sendState = NETMESSAGE_WRITESTATE_WAIT;
                setConnectWrite(ptr->connect);
                insertToList(ptr->messagePushed,pmPtr);
                printf("pusher(%d) push message %"PRId64" to %s :%d \n",ptr->pusherId,mPtr->messageid,ptr->subscribe->remoteHost,ptr->subscribe->remotePort );
        
            }
        }
        return PUSH_SUCCESS;
    }
    
}


int processPush(PushServerPtr server){
    if(server == NULL){
        return PUSH_PARAM_ERROR;
    }
    ListNodePtr  start = getListHeader(server->pushers);
    ListNodePtr  end = getListEnd(server->pushers);
    PusherPtr pusher = nextFromList(&start,end,NULL,NULL);
    while(pusher!=NULL){
        pushToTarget(server,pusher);
        pusher = nextFromList(&start,end,NULL,NULL);
    }
}


int tickPushServer(PushServerPtr server){
    return processPush(server);
}

long storePusher(PusherPtr ptr){
    if (ptr == NULL)
    {
        return -1L;
    }
    PusherStore pStore;
    if(ptr->storePosition > 0){
        restore(ptr->storePosition,&pStore,sizeof(PusherStore));
    }else{
        pStore.subscribe = 0L;
        pStore.messageReady = 0L;
    }
    pStore.pusherId = ptr->pusherId;
    pStore.subscribe = ptr->subscribe->storePosition;
    pStore.messageReady = 0L;
    pStore.state = ptr->state;
    return ptr->storePosition = store(ptr->storePosition,&pStore,sizeof(PusherStore));

}
PusherPtr restorePusher(long storePosition){
    if(storePosition <=0 ){
        return NULL;
    }
    PusherStore pStore ;
    restore(storePosition,&pStore,sizeof(PusherStore));
    PusherPtr ptr = (PusherPtr)allocMem(sizeof(Pusher));
    ptr->pusherId = pStore.pusherId;
    ptr->subscribe = restoreSubscribe(pStore.subscribe);
    ptr->messageReady = buildList();
    ptr->state = pStore.state;
    return ptr;

}
long storePusherReadyMessage(MessagePtr ptr){
    if(ptr==NULL){
        return -1L;
    }
    return ptr->storePosition;
}
long storePushServer(PushServerPtr ptr){
    if(ptr==NULL){
        return -1L;
    }
    PushServerStore serverStore;
    if(ptr->storePosition>0){
        restore(ptr->storePosition,&serverStore,sizeof(PushServerStore));
    }else{
        serverStore.pushers = 0L;
    }
    serverStore.serverId = ptr->serverId;
    serverStore.pushers = storeList(ptr->pushers,(StoreHandle)storePusher);
    serverStore.appServer = ptr->appServer->storePosition;
    return ptr->storePosition =  store(ptr->storePosition,&serverStore,sizeof(PushServerStore));
}   
PushServerPtr restorePushServer(long storePosition,AppServerPtr appServer){
    if(appServer == NULL || storePosition <=0){
        return NULL;
    }
    PushServerStore  serverStore;
    restore(storePosition,&serverStore,sizeof(PushServerStore));
    PushServerPtr  ptr  =  (PushServerPtr) allocMem(sizeof(PushServer));
    ptr->serverId = serverStore.serverId;
    ptr->pushers = restoreList(serverStore.pushers,(RestoreHandle)restorePusher);
    ptr->appServer =  appServer;
    initPushersDatas(ptr);
    return ptr; 
}
int initPushersDatas(PushServerPtr server){
    if(server == NULL){
        return -1;
    }
    ListNodePtr  start = getListHeader(server->pushers);
    ListNodePtr  end = getListEnd(server->pushers);
    PusherPtr pusher = nextFromList(&start,end,NULL,NULL);
    while(pusher!=NULL){
        SubscribePtr stemp =  pusher->subscribe;
        pusher->subscribe = findSubscribeByStorePosition(server->appServer->subscribeServer,stemp->storePosition);
        freeMem((void**)stemp);
        PusherPtr ptemp = pusher->subscribe->pusher;
        pusher->subscribe->pusher = pusher;
        freeMem((void**)ptemp);
        pusher->messagePushed = buildList();// if stored we resend message
        initPushersReadyMessage(pusher);
        pusher = nextFromList(&start,end,NULL,NULL);
    }
}
int initPushersReadyMessage(PusherPtr ptr){
    if(ptr == NULL){
        return PUSH_PARAM_ERROR;
    }
    ptr->messageReady = getAllPushingMessagesBySubscribe(ptr->subscribe);
    return PUSH_SUCCESS;
}

PusherMessagePtr  buildPusherMessage(MessagePtr message,TopicPtr topic){
    if(message == NULL || topic == NULL){
        return NULL;
    }

    PusherMessagePtr ptr =  (PusherMessagePtr)allocMem(sizeof(PusherMessage));
    ptr->id = getPusherMessageNextId();
    ptr->message = message;
    ptr->topic = topic;
    return ptr;
}
int freePusherMessage(PusherMessagePtr *pptr){
    if(pptr == NULL){
        return PUSH_PARAM_ERROR;
    }
    PusherMessagePtr ptr = *pptr;
    if(ptr == NULL){
        return PUSH_PARAM_ERROR;
    }else{
        freeMem((void**)pptr);
    }
    return PUSH_SUCCESS;
}
// long storePusherServer(PusherMessagePtr );
// PusherMessagePtr restorePusherMessage(long storePosition);

int pushMessageRecived(PusherPtr ptr,int64 messageId,char* topicName){
    if(ptr == NULL || messageId <=0 || isEmptyString(topicName)){
        return PUSH_PARAM_ERROR;
    }
    else{
        if(!isEmptyList(ptr->messagePushed)){
            PusherMessagePtr pmPtr = getFromList(ptr->messagePushed,(Find)isPusherMessage,&messageId);
            if(pmPtr!=NULL){
                pmPtr->message->relateCount--;
                if(pmPtr->message->relateCount <=0){
                    delMessage(pmPtr->topic,messageId);
                    return PUSH_SUCCESS;
                }
            }
        }
    }
    return PUSH_MESSAGE_NOT_FOUND;
}
int isPusherMessage(PusherMessagePtr ptr,int64 *messageId){
    if(ptr== NULL || messageId ==NULL){
        return 0;
    }else{
        return isMessage(ptr->message , messageId);
    }
}
int isSameSubscribe(PusherPtr push,SubscribePtr subscribe){
    if(push == NULL || subscribe == NULL){
        return 0;
    }else if(push->subscribe ==subscribe){
        return 1;
    }else {
        return 0;
    }
}