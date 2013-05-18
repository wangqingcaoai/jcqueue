#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "server.h"
	
AppServerPtr buildAppClientServer(){
    AppServerPtr ptr = allocMem(sizeof(AppServer));
    if(ptr == NULL){
        return ptr;
    }
    ptr->usersList = buildList();
    ptr->baseServer = NULL;
    ptr->subscribeServer = NULL;
    ptr->pushServer = NULL;
    ptr->transfarServer = buildTransfarServer("127.0.0.1","11221");
    ptr->request = (Handle)processClientRequest;
	ptr->response = (Handle)processClientResponse;
	ptr->pusherRequest = (Handle)processClientPusherRequest;
	ptr->pusherResponse = (Handle)processClientPusherResponse;
	return ptr;
}
int initAppClientServer(AppServerPtr ptr){
    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    restoreUsers(ptr->usersList);
    //restoreBaseServer(ptr->baseServer);
    //restoreSubscribes(ptr->subscribeServer);
    //restorePushs(ptr->pushServer);
    restoreTransfarServer(ptr->transfarServer);
    return APP_SERVER_SUCCESS;
}
int storeAppClientServer(AppServerPtr ptr){ 

    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    storeUsers(ptr->usersList);
    //storeBaseServer(ptr->baseServer);
    //storeSubscribes(ptr->subscribeServer);
    //storePushs(ptr->pushServer);
    storeTransfarServer(ptr->transfarServer);
    return APP_SERVER_SUCCESS;
}
int freeAppClientServer(AppServerPtr *pptr){
    if(pptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    AppServerPtr ptr = (*pptr);
    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    freeList(&(ptr->usersList),(Free)freeUser);
    //freeBaseServer(&(ptr->baseServer));
    //freeSubscribeServer(&(ptr->subscribeServer));
    //freePushServer(&(ptr->pushServer));
    freeTransfarServer(&(ptr->transfarServer));
    free(ptr);
    (*pptr) = ptr = NULL;
    return APP_SERVER_SUCCESS;
}
int processAppClientServer(AppServerPtr ptr){
    //processConnect(ptr->transfarServer);
}
int processClientRequest(ConnectPtr ptr,int ev){

}
int processClientPusherResponse(ConnectPtr ptr,int ev){

}
int processClientResponse(ConnectPtr ptr,int ev){

}
int processClientPusherRequest(ConnectPtr ptr,int ev){

}
