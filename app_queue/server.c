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
    //processConnect(ptr->transfarServer);
    
}
int processRequest(ConnectPtr ptr,int ev){
	int result = getRequestData(ptr);
    switch(result){
        case CONNECT_STATE_READ_MESSAGE_READY:
            break;
        case CONNECT_STATE_READ_WAIT_DATA:
            return APP_SERVER_SUCCESS;
        case CONNECT_STATE_READ_FORMAT_ERROR:
        //setNetMessageError()
            return;
    }
    //check user
    if(result == CONNECT_STATE_READ_MESSAGE_READY){
        if(ptr->netMessage->user != NULL){
            if(ptr->user == NULL){
                ptr->user = findUser(ptr->tServer->appServer->usersList,ptr->netMessage->user);    
            }
            if(ptr->user == NULL){
                //setNetMessageError user not exists
                return;
            }else{
                int checkState = checkUser(ptr->user,ptr->netMessage->key,ptr->netMessage->password);
                if(checkState == USER_SUCCESS){
                    //success
                    aq_router(ptr->netMessage);
                }else{
                    //setNetMessageError  usercheck not pass
                }
            }    
        }
    }

	//setResponseData(ptr->netMessage);
	//setSecureInfo(ptr->netMessage);
    return APP_SERVER_SUCCESS;
}
int processResponse(ConnectPtr ptr,int ev){
	//sendResponseData(ptr);
}
int processPusherResponse(ConnectPtr ptr,int ev){
	// int result =getRequestData(ptr);

	// checkUser(ptr->netMessage);
	// pushResponse(ptr->netMessage);

}

int processPusherRequest(ConnectPtr ptr,int ev){
//    sendResponseData(ptr);	
}
