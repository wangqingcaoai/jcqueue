#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "user.h"
#include "../transfar/server.h"
#include "push.h"
#include "../base_queue/server.h"
AppServerPtr buildAppServer(){
    AppServerPtr ptr = allocMem(sizeof(AppServer));
    if(ptr == NULL){
        return ptr;
    }
    ptr->usersList = buildList();
    ptr->baseServer = buildBaseServer();
    ptr->subscribeServer = buildSubscribeServer(ptr);
    ptr->pushServer = buildPushServer(ptr);
    ptr->transfarServer = buildTransfarServer("127.0.0.1","11221");
    ptr->request = (Handle)processRequest;
	ptr->response = (Handle)processResponse;
	ptr->pusherRequest=(Handle)processPusherRequest;
	ptr->pusherResponse=(Handle)processPusherResponse;
	return ptr;
}
int initAppServer(AppServerPtr ptr){
    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    restoreUsers(ptr->usersList);
    restoreBaseServer(ptr->baseServer);
    restoreSubscribes(ptr->subscribeServer);
    restorePushs(ptr->pushServer);
    restoreTransfarServer(ptr->transfarServer);
    return APP_SERVER_SUCCESS;
}
int storeAppServer(AppServerPtr ptr){ 

    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    storeUsers(ptr->usersList);
    storeBaseServer(ptr->baseServer);
    storeSubscribes(ptr->subscribeServer);
    storePushs(ptr->pushServer);
    storeTransfarServer(ptr->transfarServer);
    return APP_SERVER_SUCCESS;
}
int freeAppServer(AppServerPtr *pptr){
    if(pptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    AppServerPtr ptr = (*pptr);
    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    freeList(&(ptr->usersList),(Free)freeUser);
    freeBaseServer(&(ptr->baseServer));
    freeSubscribeServer(&(ptr->subscribeServer));
    freePushServer(&(ptr->pushServer));
    freeTransfarServer(&(ptr->transfarServer));
    free(ptr);
    (*pptr) = ptr = NULL;
    return APP_SERVER_SUCCESS;
}
int processAppServer(AppServerPtr ptr){
    //processConnect(ptr->transfarServer);
    
}
int processRequest(ConnectPtr ptr,int ev){
	int result = getRequestData(ptr);
	checkUser(ptr->netMessage);
	aq_router(ptr->netMessage);
	setResponseData(ptr->netMessage);
	setSecretInfo(ptr->netMessage);
}
int processResponse(ConnectPtr ptr,int ev){
	sendResponseData(ptr);
}
int processPusherResponse(ConnectPtr ptr,int ev){
	int result =getRequestData(ptr);

	checkUser(ptr->netMessage);
	pushResponse(ptr->netMessage);

}

int processPusherRequest(ConnectPtr ptr,int ev){
    sendResponseData(ptr);	
}
