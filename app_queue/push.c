#include "push.h"
#include <stdio.h>
#include <stdlib.h>

PushServerPtr buildPushServer(){
	static int id;
	id++;
	PushServerPtr ptr = malloc(sizeof(PushServer));
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
	
}
int freePusher(SubscribePtr * pptr);
int addPusher(PushServerPtr server,SubscribePtr subscribe);
int addMessageToPusher(PusherPtr ,MessagePtr message);
int pushToTarget(PusherPtr ptr);
int processPush(PushServerPtr );