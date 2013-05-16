#include <stdio.h>
#include <stdlib.h>
#include "../data/list.h"
#include "push.h"
PushServerPtr buildPushServer(){
	static int id;
	PushServerPtr ptr = (PushServerPtr)allocMem(sizeof(PushServer));
	if(ptr==NULL){
        return ptr;
    }
    id++;
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
	free(ptr);
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

}
int addPusher(PushServerPtr server,SubscribePtr subscribe){

}
int addMessageToPusher(PusherPtr ptr,MessagePtr message){

}
int pushToTarget(PusherPtr ptr){

}
int processPush(PushServerPtr server){

}