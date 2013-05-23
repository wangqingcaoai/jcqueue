#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "server.h"
#include "../transfar/console.h"
#include "../util/config.h"
#include "../errors.h"
	
AppServerPtr buildClientAppServer(const char* host,const char* port){
    AppServerPtr ptr = allocMem(sizeof(AppServer));
    if(ptr == NULL){
        return ptr;
    }
    //ptr->usersList = buildList();
    ptr->admin = buildUser(getConfig("adminUser","admin"),getConfig("adminPassword","jcqueue"));
    ptr->baseServer = NULL;
    ptr->subscribeServer = NULL;
    ptr->pushServer = NULL;
    ptr->transfarServer = buildTransfarServer(ptr,host,port);
    ptr->request = (Handle)processClientRequest;
	ptr->response = (Handle)processClientResponse;
	ptr->pusherRequest = (Handle)processClientPusherRequest;
	ptr->pusherResponse = (Handle)processClientPusherResponse;
	return ptr;
}
int initClientAppServer(AppServerPtr ptr){
    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    //restoreUsers(ptr->usersList);
    //restoreBaseServer(ptr->baseServer);
    //restoreSubscribes(ptr->subscribeServer);
    //restorePushs(ptr->pushServer);
    //restoreTransfarServer(ptr->transfarServer);
    return APP_SERVER_SUCCESS;
}
int storeClientAppServer(AppServerPtr ptr){ 

    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    //storeUsers(ptr->usersList);
    //storeBaseServer(ptr->baseServer);
    //storeSubscribes(ptr->subscribeServer);
    //storePushs(ptr->pushServer);
    //storeTransfarServer(ptr->transfarServer);
    return APP_SERVER_SUCCESS;
}
int freeClientAppServer(AppServerPtr *pptr){
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
    freeMem((void**)(void**)&ptr);
    (*pptr) = ptr ;
    return APP_SERVER_SUCCESS;
}
int processClientAppServer(AppServerPtr ptr){
    srvStart(ptr->transfarServer);
    while(1){
        sleep(1);
        srvProcess(ptr->transfarServer);   
    }
}
int processClientRequest(ConnectPtr ptr,int ev){
    int result = getRequestData(ptr);
    switch(result){
        case CONNECT_STATE_READ_MESSAGE_READY:
            break;
        case CONNECT_STATE_READ_WAIT_DATA:
            return APP_SERVER_SUCCESS;
        case CONNECT_STATE_READ_FORMAT_ERROR:
            setNetMessageError(ptr->netMessage,buildErrorCode(PARSER_ERRORS_MARK,PARSER_ERROR_FORMAT_ERROR),PARSER_ERROR_FORMAT_ERROR_MSG);
            return APP_SERVER_SUCCESS;
    }
    //check user
    if(result == CONNECT_STATE_READ_MESSAGE_READY){
        if(ptr->netMessage->user != NULL){
            
            int checkState = checkUser(ptr->tServer->appServer->acceptUser,ptr->netMessage->key,ptr->netMessage->password);
            if(checkState == USER_SUCCESS){
                //success
                aq_client_router(ptr->netMessage);
            }else{
                setNetMessageError(ptr->netMessage,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_ERROR_SECURE_FAILED),AQ_ERROR_USER_ERROR_SECURE_FAILED_MSG);
            }    
        }
    }
    return APP_SERVER_SUCCESS;
}
int processClientPusherResponse(ConnectPtr ptr,int ev){

}
int processClientResponse(ConnectPtr ptr,int ev){

}
int processClientPusherRequest(ConnectPtr ptr,int ev){

}

int processClientConsoleIn(ConsolePtr ptr,int ev){

}
int processClientConsoleOut(ConsolePtr ptr,int ev){

}