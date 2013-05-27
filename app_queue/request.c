#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../data/list.h"
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
    return ptr;
}
int freeRequestServer(RequestServerPtr * pptr){
    if(pptr == NULL){
        return REQEUST_PARAM_ERROR;
    }
    RequestServerPtr ptr = *pptr;
    if (ptr == NULL)
    {
        return REQEUST_PARAM_ERROR;
    }
    freeList(&(ptr->requesters),(Free)freeRequester);
    freeMem((void**)&ptr);
    (*pptr)==NULL;
    return REQEUST_SUCCESS;
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
    return ptr; 
    
}
int freeRequester(RequesterPtr * pptr){
    if(pptr == NULL){
        return REQEUST_PARAM_ERROR;
    }

    RequesterPtr ptr = (*pptr);
    if(ptr == NULL){
        return REQEUST_PARAM_ERROR;
    }
    freeList(&(ptr->messageReady),(Free)freeMicroNetMessage);
    connectClose(ptr->connect);
    //removeConnect(ptr->connect);
    freeMem((void**)pptr);
    ptr = NULL;
}
int setRequesterProtocol(RequesterPtr ptr,const char* protocol,const char* version){
    if(ptr == NULL || protocol == NULL || version == NULL){
        return REQEUST_PARAM_ERROR;
    }
    ptr->protocol = allocString(protocol);
    ptr->version = allocString(version);
    return REQEUST_SUCCESS;
}
int addRequester(RequestServerPtr server,UserPtr user,const char* host,const char* port){
    if(server == NULL||user==NULL){
        return REQEUST_PARAM_ERROR;
    }
    RequesterPtr  ptr = buildRequester(user,host,port);
    insertToList(server->requesters,ptr);
    if(server->current == NULL){
        server->current = ptr;
    }
    return REQEUST_SUCCESS;
}
int addMessageToRequester(RequesterPtr ptr,NetMessagePtr message){
    if(ptr == NULL || message == NULL){
        return REQEUST_PARAM_ERROR;
    }
    MicroNetMessagePtr messagePtr = buildMicroNetMessage(message);
    insertToList(ptr->messageReady,messagePtr);
    return REQEUST_SUCCESS;
}
int requestToTarget(RequestServerPtr server, RequesterPtr ptr){
    if(ptr == NULL || server == NULL){
        return REQEUST_PARAM_ERROR;
    }
    char buf[UTIL_NUM_BUF_SIZE];
    int bufSize = UTIL_NUM_BUF_SIZE;
    if(ptr->connect == NULL){
        ptr->connect = buildRequestConnect(server->appServer->transfarServer,ptr->remoteHost,ptr->remotePort,
            server->appServer->responseOut,server->appServer->requestOut);
        if(ptr->connect == NULL){
            addLog(LOG_ERROR,LOG_LAYER_APP_QUEUE,REQUEST_POSITION_NAME,"failed to build connect to %s:%s ",ptr->remoteHost,ptr->remotePort);
        
            return REQEUST_PARAM_ERROR;
        }
        ptr->connect->user = ptr->user;
        ptr->connect->netMessage->sendState = NETMESSAGE_WRITESTATE_FINISH;    
    }
    //means can send another message
    if(ptr->connect->netMessage->sendState == NETMESSAGE_WRITESTATE_FINISH){
        
        
        MicroNetMessagePtr mPtr= popFromList(ptr->messageReady);
        if(mPtr == NULL){
            return REQEUST_SUCCESS;
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
        return REQEUST_PARAM_ERROR;
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
    return mptr;
}
int freeMicroNetMessage(MicroNetMessagePtr *pptr){
    if(pptr == NULL){
        return REQEUST_PARAM_ERROR;
    }
    MicroNetMessagePtr ptr= (*pptr);
    if(ptr == NULL){
        return REQEUST_PARAM_ERROR;
    }
    freeString(&(ptr->cmd));
    freeString(&(ptr->extraParam));
    freeMem(&(ptr->data));
    freeMem((void**)pptr);
    ptr==NULL;
    return REQEUST_SUCCESS;
}
int changeCurrentRequester(RequestServerPtr server,int requesterId){
    if(server == NULL || requesterId<0){
        return REQEUST_PARAM_ERROR;
    }
    RequesterPtr ptr = getFromList(server->requesters,(Find)isRequester,&requesterId);
    
    if(ptr==NULL){
        return REQEUST_ERROR_REQUESTER_NOT_FOUND ;
    }else{
        server->current = ptr;
    }
    return REQEUST_SUCCESS;
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
        return REQEUST_PARAM_ERROR;
    }
    ListNodePtr  start = getListHeader(server->requesters);
    ListNodePtr  end = getListEnd(server->requesters);
    RequesterPtr requester = nextFromList(&start,end,NULL,NULL);
    char* buf= allocMem(REQEUST_LIST_BUF_SIZE);
    int writedLength = 0,leavLength =0,write;
    while(requester!=NULL){
        if(writedLength<REQEUST_LIST_BUF_SIZE){
            write = snprintf(buf+writedLength,REQEUST_LIST_BUF_SIZE-writedLength,"ID:%d %s:%s",requester->requesterId,requester->remoteHost,requester->remotePort);
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

    return REQEUST_SUCCESS;
}

int delRequesterFromServer(RequestServerPtr server,int requesterId){
    if(server == NULL || requesterId<0){
        return REQEUST_PARAM_ERROR;
    }
    if(server->current!=NULL&&server->current->requesterId == requesterId){
        server->current = NULL;
    }
    int removenum = removeFromList(server->requesters,(Find)isRequester,&requesterId,(Free)freeRequester);
    if(server->current == NULL){
        server->current = getFromList(server->requesters,NULL,NULL);
    }

    if(removenum ==0){
        return REQEUST_ERROR_REQUESTER_NOT_FOUND;
    }else{
        return REQEUST_SUCCESS;
    }
}
int tickRequestServer(RequestServerPtr ptr){
    processRequestServer(ptr);
}