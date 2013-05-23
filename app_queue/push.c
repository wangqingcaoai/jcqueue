#include <stdio.h>
#include <stdlib.h>
#include "../data/list.h"
#include "push.h"
#include "../errors.h"
PushServerPtr buildPushServer(AppServerPtr server){
	if(server == NULL){
		return NULL;
	}
	static int id;
	PushServerPtr ptr = (PushServerPtr)allocMem(sizeof(PushServer));
	if(ptr==NULL){
        return ptr;
    }
    id++;
    ptr->appServer = server;
	ptr->serverId = id;
	ptr->pushers = buildList();
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
	static int id;
	if(subscribe == NULL){
		return NULL;
	}
	
	PusherPtr ptr = (PusherPtr)allocMem(sizeof(Pusher));
	
	if(ptr == NULL){
		return ptr;
	}
	id++;
	ptr->pusherId = id;
	ptr->subscribe = subscribe;
	ptr->messageReady = buildList();
	ptr->connect = NULL;
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
	PusherPtr  ptr = buildPusher(subscribe);
	insertToList(server->pushers,ptr);
	return PUSH_SUCCESS;
}
int addMessageToPusher(PusherPtr ptr,MessagePtr message){
	if(ptr == NULL || message == NULL){
		return PUSH_PARAM_ERROR;
	}
	insertToList(ptr->messageReady,message);
	return PUSH_SUCCESS;
}
int pushToTarget(PushServerPtr server, PusherPtr ptr){
	if(ptr == NULL || server == NULL){
		return PUSH_PARAM_ERROR;
	}
	if(ptr->state == PUSH_STATE_FINISH){
		int bufSize = 25;
		char buf[bufSize];
		if(ptr->connect == NULL){
			ptr->connect = buildRequestConnect(server->appServer->transfarServer,ptr->subscribe->remoteHost,intToString(ptr->subscribe->remotePort,buf,bufSize));
		}
		MessagePtr mPtr= popFromList(ptr->messageReady);
		if(mPtr == NULL){
			return PUSH_SUCCESS;
		}else{
			NetMessagePtr nmptr = ptr->connect->netMessage;
			setNetMessageSendData(nmptr,buildErrorCode(PUSH_SUCCESS_MARK,PUSH_PUSH_SUCCESS),PUSH_TO_CLIENT_CMD,nmptr->target,nmptr->targetType,mPtr->data,mPtr->length);
            
            setSendExtraParam(nmptr,"message_id",int64ToString(mPtr->messageid,buf,bufSize));
            
            setSendExtraParam(nmptr,"priority",intToString(mPtr->priority,buf,bufSize));

            setSendExtraParam(nmptr,"delay",intToString(mPtr->delay,buf,bufSize));

            setSendExtraParam(nmptr,"activetime",int64ToString(mPtr->activetime,buf,bufSize));

            setSendExtraParam(nmptr,"timestamp",int64ToString(mPtr->timestamp,buf,bufSize));
		}
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