#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "user.h"
#include "../transfar/server.h"
#include "push.h"
#include "../base_queue/server.h"
#include "../errors.h"
#include "../util/config.h"
AppServerPtr buildAppServer(const char* host,const char*port){
    AppServerPtr ptr = allocMem(sizeof(AppServer));
    if(ptr == NULL){
        return ptr;
    }
    ptr->usersList = buildList();
    ptr->admin = buildUser(getConfig("adminUser","admin"),getConfig("adminPassword","jcqueue"));
    ptr->sendUser =buildUser(getConfig("sendUser","sender"),getConfig("senderPassword","send"));
    ptr->acceptUser = NULL;
    ptr->baseServer = buildBaseServer();
    ptr->subscribeServer = buildSubscribeServer(ptr);
    ptr->pushServer = buildPushServer(ptr);
    ptr->transfarServer = buildTransfarServer(ptr,host,port);
    ptr->request = (Handle)processRequest;
	ptr->response = (Handle)processResponse;
	ptr->pusherRequest=(Handle)processPusherRequest;
	ptr->pusherResponse=(Handle)processPusherResponse;
    ptr->tick = (TickHandle)tickAppServer;
	return ptr;
}
int initAppServer(AppServerPtr ptr){
    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    // restoreUsers(ptr->usersList);
    // restoreBaseServer(ptr->baseServer);
    // restoreSubscribes(ptr->subscribeServer);
    // restorePushs(ptr->pushServer);
    // restoreTransfarServer(ptr->transfarServer);
    return APP_SERVER_SUCCESS;
}
int storeAppServer(AppServerPtr ptr){ 

    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    // storeUsers(ptr->usersList);
    // storeBaseServer(ptr->baseServer);
    // storeSubscribes(ptr->subscribeServer);
    // storePushs(ptr->pushServer);
    // storeTransfarServer(ptr->transfarServer);
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
    freeMem((void**)&ptr);
    (*pptr) = ptr ;
    return APP_SERVER_SUCCESS;
}
int processAppServer(AppServerPtr ptr){
    srvStart(ptr->transfarServer);
    while(1){
        sleep(1);
        srvProcess(ptr->transfarServer);   
    }
    //tick
}
int processRequest(ConnectPtr ptr,int ev){
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
            if(ptr->user == NULL){
                ptr->user = findUser(ptr->tServer->appServer->usersList,ptr->netMessage->user);    
            }
            if(ptr->user == NULL){
                setNetMessageError(ptr->netMessage,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_NOT_FOUND),AQ_ERROR_USER_NOT_FOUND_MSG,ptr->netMessage->user);
        
                return APP_SERVER_SUCCESS;
            }else{
                int checkState = checkUser(ptr->user,ptr->netMessage->key,ptr->netMessage->password);
                if(checkState == USER_SUCCESS){
                    //success
                    aq_router(ptr->netMessage);
                }else{
                    setNetMessageError(ptr->netMessage,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_ERROR_SECURE_FAILED),AQ_ERROR_USER_ERROR_SECURE_FAILED_MSG);
                }
            }    
        }
    }
    return APP_SERVER_SUCCESS;
}
int processResponse(ConnectPtr ptr,int ev){
    setNetMessageSendUser(ptr->netMessage,ptr->tServer->appServer->sendUser);
    sendResponseData(ptr);
}
int processPusherResponse(ConnectPtr ptr,int ev){
	// int result =getRequestData(ptr);

	// checkUser(ptr->netMessage);
	// pushResponse(ptr->netMessage);

}

int processPusherRequest(ConnectPtr ptr,int ev){
//    sendResponseData(ptr);	
}

int tickAppServer(AppServerPtr ptr){
    printf("tick\n");
}
int processConsoleIn(ConsolePtr ptr,int ev){

}
int processConsoleOut(ConsolePtr ptr,int ev){

}