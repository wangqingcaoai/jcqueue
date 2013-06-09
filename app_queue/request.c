#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../data/list.h"
#include "../data/store.h"
#include "request.h"
#include "../errors.h"
#include "user.h"
#include "../util/log.h"
#include "../transfar/connect.h"
#include "../parser/net_message.h"
RequestServerPtr buildRequestServer(AppServerPtr server){
    if(server == NULL){
        return NULL;
    }
    static int id;
    RequestServerPtr ptr = (RequestServerPtr)allocMem(sizeof(RequestServer));
    if(ptr==NULL){
        return ptr;
    }
    id++;
    ptr->appServer = server;
    ptr->serverId = id;
    ptr->requesters = buildList();
    ptr->current = NULL;
    ptr->storePosition = 0L;
    return ptr;
}
int freeRequestServer(RequestServerPtr * pptr){
    if(pptr == NULL){
        return REQUEST_PARAM_ERROR;
    }
    RequestServerPtr ptr = *pptr;
    if (ptr == NULL)
    {
        return REQUEST_PARAM_ERROR;
    }
    freeList(&(ptr->requesters),(Free)freeRequester);
    freeMem((void**)&ptr);
    (*pptr)==NULL;
    return REQUEST_SUCCESS;
}
RequesterPtr buildRequester(UserPtr user,const char* remoteHost,const char* remotePort){
    static int id;
    if(user == NULL ||remoteHost == NULL||remotePort == NULL){
        return NULL;
    }
    RequesterPtr ptr = (RequesterPtr)allocMem(sizeof(Requester));
    
    if(ptr == NULL){
        return ptr;
    }
    id++;
    ptr->requesterId = id;
    ptr->user = user;
    ptr->messageReady = buildList();
    ptr->remoteHost = allocString(remoteHost);
    ptr->remotePort = allocString(remotePort);
    ptr->connect = NULL;
    ptr->storePosition  = 0L;
    return ptr; 
    
}
int freeRequester(RequesterPtr * pptr){
    if(pptr == NULL){
        return REQUEST_PARAM_ERROR;
    }

    RequesterPtr ptr = (*pptr);
    if(ptr == NULL){
        return REQUEST_PARAM_ERROR;
    }
    freeList(&(ptr->messageReady),(Free)freeMicroNetMessage);
    connectClose(ptr->connect);
    //removeConnect(ptr->connect);
    freeMem((void**)pptr);
    ptr = NULL;
}
int setRequesterProtocol(RequesterPtr ptr,const char* protocol,const char* version){
    if(ptr == NULL || protocol == NULL || version == NULL){
        return REQUEST_PARAM_ERROR;
    }
    ptr->protocol = allocString(protocol);
    ptr->version = allocString(version);
    return REQUEST_SUCCESS;
}
int addRequester(RequestServerPtr server,UserPtr user,const char* host,const char* port){
    if(server == NULL||user==NULL){
        return REQUEST_PARAM_ERROR;
    }
    RequesterPtr  ptr = buildRequester(user,host,port);
    insertToList(server->requesters,ptr);
    if(server->current == NULL){
        server->current = ptr;
    }
    return REQUEST_SUCCESS;
}
int addMessageToRequester(RequesterPtr ptr,NetMessagePtr message){
    if(ptr == NULL || message == NULL){
        return REQUEST_PARAM_ERROR;
    }
    MicroNetMessagePtr messagePtr = buildMicroNetMessage(message);
    insertToList(ptr->messageReady,messagePtr);
    return REQUEST_SUCCESS;
}
int requestToTarget(RequestServerPtr server, RequesterPtr ptr){
    if(ptr == NULL || server == NULL){
        return REQUEST_PARAM_ERROR;
    }
    char buf[UTIL_NUM_BUF_SIZE];
    int bufSize = UTIL_NUM_BUF_SIZE;
    if(ptr->connect == NULL){
        ptr->connect = buildRequestConnect(server->appServer->transfarServer,ptr->remoteHost,ptr->remotePort,
            server->appServer->responseOut,server->appServer->requestOut);
        if(ptr->connect == NULL){
            addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,REQUEST_POSITION_NAME,"failed to build connect to %s:%s ",ptr->remoteHost,ptr->remotePort);
        
            return REQUEST_PARAM_ERROR;
        }
        ptr->connect->user = ptr->user;
        ptr->connect->netMessage->sendState = NETMESSAGE_WRITESTATE_FINISH;    
    }
    //means can send another message
    if(ptr->connect->netMessage->sendState == NETMESSAGE_WRITESTATE_FINISH){
        
        
        MicroNetMessagePtr mPtr= popFromList(ptr->messageReady);
        if(mPtr == NULL){
            return REQUEST_SUCCESS;
        }else{
            NetMessagePtr nmptr = ptr->connect->netMessage;
            setNetMessageParam(nmptr,"sendExtraParam",mPtr->extraParam);
            setNetMessageSendData(nmptr,mPtr->errcode,mPtr->cmd,mPtr->data,mPtr->length);
            if(nmptr->protocolType == NULL){
                nmptr->protocolType = allocString(NETMESSAGE_TYPE_JCQ);
                freeString(&nmptr->version);
                nmptr->version = allocString(NETMESSAGE_VERSION_JCQ);
            }
            ptr->connect->netMessage->sendState = NETMESSAGE_WRITESTATE_WAIT;
        }
    }
    
}
int processRequestServer(RequestServerPtr server){
    if(server == NULL){
        return REQUEST_PARAM_ERROR;
    }
    ListNodePtr  start = getListHeader(server->requesters);
    ListNodePtr  end = getListEnd(server->requesters);
    RequesterPtr requester = nextFromList(&start,end,NULL,NULL);
    while(requester!=NULL){
        requestToTarget(server,requester);
        requester = nextFromList(&start,end,NULL,NULL);
    }

}
MicroNetMessagePtr buildMicroNetMessage(NetMessagePtr ptr){
       
    if(ptr == NULL){
        return NULL;
    }
    static int64 id;
    MicroNetMessagePtr mptr = allocMem(sizeof(MicroNetMessage));
    if(mptr == NULL){
        return NULL;
    }
    id++;
    mptr->id = id;
    mptr->cmd = allocString(ptr->cmd) ;
    mptr->extraParam = allocString(ptr->extraParam);
    mptr->data = allocMem(ptr->length);
    memcpy(mptr->data,ptr->data,ptr->length);
    mptr->length = ptr->length;
    mptr->errcode = ptr->errcode;
    mptr->storePosition = 0L;
    return mptr;
}
int freeMicroNetMessage(MicroNetMessagePtr *pptr){
    if(pptr == NULL){
        return REQUEST_PARAM_ERROR;
    }
    MicroNetMessagePtr ptr= (*pptr);
    if(ptr == NULL){
        return REQUEST_PARAM_ERROR;
    }
    freeString(&(ptr->cmd));
    freeString(&(ptr->extraParam));
    freeMem(&(ptr->data));
    freeMem((void**)pptr);
    ptr==NULL;
    return REQUEST_SUCCESS;
}
int changeCurrentRequester(RequestServerPtr server,int requesterId){
    if(server == NULL || requesterId<0){
        return REQUEST_PARAM_ERROR;
    }
    RequesterPtr ptr = getFromList(server->requesters,(Find)isRequester,&requesterId);
    
    if(ptr==NULL){
        return REQUEST_ERROR_REQUESTER_NOT_FOUND ;
    }else{
        server->current = ptr;
    }
    return REQUEST_SUCCESS;
}
int isRequester(RequesterPtr ptr,int* requesterId){
    if(ptr == NULL || requesterId == NULL){
        return 0;
    }
    if(ptr->requesterId == (int)(*requesterId)){
        return 1;
    }
    return 0;
}


int getRequesterList(RequestServerPtr server,NetMessagePtr message){
    if(server == NULL ||message == NULL){
        return REQUEST_PARAM_ERROR;
    }
    ListNodePtr  start = getListHeader(server->requesters);
    ListNodePtr  end = getListEnd(server->requesters);
    RequesterPtr requester = nextFromList(&start,end,NULL,NULL);
    char* buf= allocMem(REQUEST_LIST_BUF_SIZE);
    int writedLength = 0,leavLength =0,write;
    while(requester!=NULL){
        if(writedLength<REQUEST_LIST_BUF_SIZE){
            write = snprintf(buf+writedLength,REQUEST_LIST_BUF_SIZE-writedLength,"ID:%d %s:%s",requester->requesterId,requester->remoteHost,requester->remotePort);
            writedLength += write;
        }else{
             break;
        }
        requester = nextFromList(&start,end,NULL,NULL);
    }
    if(writedLength == 0){
        char* string = " no server saved in client!";
        strcpy(buf,string);
        writedLength = strlen(string);
    }
    setNetMessageJustSendData(message,buf,writedLength);

    return REQUEST_SUCCESS;
}

int delRequesterFromServer(RequestServerPtr server,int requesterId){
    if(server == NULL || requesterId<0){
        return REQUEST_PARAM_ERROR;
    }
    if(server->current!=NULL&&server->current->requesterId == requesterId){
        server->current = NULL;
    }
    int removenum = removeFromList(server->requesters,(Find)isRequester,&requesterId,(Free)freeRequester);
    if(server->current == NULL){
        server->current = getFromList(server->requesters,NULL,NULL);
    }

    if(removenum ==0){
        return REQUEST_ERROR_REQUESTER_NOT_FOUND;
    }else{
        return REQUEST_SUCCESS;
    }
}
int tickRequestServer(RequestServerPtr ptr){
    processRequestServer(ptr);
}

long storeRequester(RequesterPtr ptr){
    if(ptr == NULL){
        return -1L;
    }
    RequesterStore rstore;
    if(ptr->storePosition >0){
        restore(ptr->storePosition,&rstore,sizeof(RequesterStore));
    }else{
        rstore.user = 0L;
        rstore.messageReady = 0L;
        rstore.remoteHost = 0L;
        rstore.remotePort = 0L;
        rstore.protocol = 0L;
        rstore.version =0L;
    }
    rstore.requesterId = ptr->requesterId;
    rstore.user = ptr->user->storePosition;
    rstore.messageReady = storeList(ptr->messageReady,(StoreHandle)storeMicroNetMessage);
    rstore.remoteHost = storeString(rstore.remoteHost,ptr->remoteHost,REQUEST_MAX_REQUEST_REMOTEHOST_SIZE);
    rstore.remotePort = storeString(rstore.remotePort,ptr->remotePort,REQUEST_MAX_REQUEST_REMOTEPORT_SIZE);
    rstore.protocol = storeString(rstore.protocol,ptr->protocol,REQUEST_MAX_REQUEST_PROTOCOL_SIZE);
    rstore.version = storeString(rstore.version,ptr->version,REQUEST_MAX_REQUEST_VERSION_SIZE);
    rstore.state = ptr->state;
    return ptr->storePosition = store(ptr->storePosition,&rstore,sizeof(RequesterStore));
}
RequesterPtr restoreRequester(long storePosition){
    if(storePosition <=0){
        return NULL;
    }
    RequesterStore rstore;
    restore(storePosition,&rstore,sizeof(RequesterStore));
    RequesterPtr ptr = (RequesterPtr)allocMem(sizeof(Requester));
    if(ptr != NULL){
        ptr->requesterId = rstore.requesterId;
        ptr->user = restoreUser(rstore.user);
        ptr->messageReady = restoreList(rstore.messageReady,(RestoreHandle)restoreMicroNetMessage);
        ptr->remoteHost =  restoreString(rstore.remoteHost);
        ptr->remotePort = restoreString(rstore.remotePort);
        ptr->protocol = restoreString(rstore.protocol);
        ptr->version = restoreString(rstore.version);
        ptr->state = rstore.state;
        ptr->storePosition = storePosition;
    }
    return ptr;
}
long storeRequestServer(RequestServerPtr ptr){
    if(ptr == NULL){
        return -1L;
    }
    RequestServerStore rsStore;
    if(ptr->storePosition>0){
        restore(ptr->storePosition,&rsStore,sizeof(RequestServerStore));
    }else{
        rsStore.requesters = 0L;
        rsStore.current = 0L;
        rsStore.appServer = 0L;
    }
    rsStore.serverId = ptr->serverId;
    rsStore.requesters = storeList(ptr->requesters,(StoreHandle)storeRequester);
    if(ptr->current == NULL){
        rsStore.current = 0L;
    }else{
        rsStore.current =  ptr->current->storePosition;    
    }
    rsStore.appServer = ptr->appServer->storePosition;
    return ptr->storePosition = store(ptr->storePosition,&rsStore,sizeof(RequestServerStore));
}
RequestServerPtr restoreRequestServer(long storePosition,AppServerPtr appServer){
    if(storePosition <=0){
        return NULL;
    }
    RequestServerStore rsStore;
    restore(storePosition,&rsStore,sizeof(RequestServerStore));
    RequestServerPtr ptr = (RequestServerPtr)allocMem(sizeof(RequestServer));
    ptr->serverId = rsStore.serverId;
    ptr->requesters = restoreList(rsStore.requesters,(RestoreHandle)restoreRequester);
    ptr->appServer = appServer;
    ptr->current = getFromList(ptr->requesters,(Find)isRequesterByStorePosition,&rsStore.current);
    return ptr;
}
long storeMicroNetMessage(MicroNetMessagePtr ptr){
    if(ptr == NULL){
        return -1L;
    }
    MicroNetMessageStore mStore;
    if(ptr->storePosition >0){
        restore(ptr->storePosition,&mStore,sizeof(MicroNetMessageStore));
    }else{
        mStore.cmd = 0L;
        mStore.extraParam = 0L;
        mStore.data = 0L;
        mStore.length = 0;
        mStore.errcode = 0;
    }
    mStore.id =  ptr->id;
    mStore.cmd = storeString(mStore.cmd,ptr->cmd,REQUEST_MAX_REQUEST_CMD_SIZE);
    mStore.extraParam = storeString(mStore.extraParam,ptr->extraParam,REQUEST_MAX_REQUEST_EXTRAPARAM_SIZE);
    mStore.length = ptr->length;
    mStore.errcode=ptr->errcode;
    mStore.data = store(mStore.data,ptr->data,ptr->length);
    return ptr->storePosition = store(ptr->storePosition,&mStore,sizeof(MicroNetMessageStore));
}
MicroNetMessagePtr restoreMicroNetMessage(long storePosition){
    if(storePosition <= 0){
        return NULL;
    }
    MicroNetMessageStore mStore;
    restore(storePosition,&mStore,sizeof(MicroNetMessageStore));
    MicroNetMessagePtr ptr= (MicroNetMessagePtr)allocMem(sizeof(MicroNetMessage));
    if(ptr != NULL){
        ptr->id = mStore.id;
        ptr->cmd =restoreString(mStore.cmd);
        ptr->extraParam = restoreString(mStore.extraParam);
        ptr->length = mStore.length;
        ptr->errcode =mStore.errcode;
        ptr->data = allocMem(mStore.length);
        restore(mStore.data,ptr->data,mStore.length);
    }
    ptr->storePosition = storePosition;
    return ptr;
}
int isRequesterByStorePosition(RequesterPtr ptr, long * storePosition){
    if(ptr== NULL || storePosition == NULL || (*storePosition)<=0){
        return 0;
    }
    return ptr->storePosition == (*storePosition);

}