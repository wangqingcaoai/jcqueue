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
#include "../data/store.h"
AppServerPtr buildAppServer(const char* host,const char*port){
    AppServerPtr ptr = allocMem(sizeof(AppServer));
    if(ptr == NULL){
        return ptr;
    }
    ptr->request = (Handle)processRequest;
    ptr->response = (Handle)processResponse;
    ptr->requestOut = (Handle)processRequestOut;
    ptr->responseOut = (Handle)processResponseOut;
    
    ptr->pusherRequest=(Handle)processPusherRequest;
    ptr->pusherResponse=(Handle)processPusherResponse;
    ptr->consoleIn = (Handle)processConsoleIn;
    ptr->consoleOut = (Handle)processConsoleOut;
    ptr->tick = (TickHandle)tickAppServer;
    ptr->usersList = buildList();
    ptr->admin = buildUser(getConfig("adminUser","admin"),getConfig("adminPassword","jcqueue"));
    ptr->sendUser =buildUser(getConfig("sendUser","sender"),getConfig("senderPassword","send"));
    ptr->acceptUser = NULL;
    ptr->requestServer = NULL;
    ptr->baseServer = buildBaseServer(ptr);
    ptr->subscribeServer = buildSubscribeServer(ptr);
    ptr->pushServer = buildPushServer(ptr);
    ptr->transfarServer = buildTransfarServer(ptr,host,port);
    

	return ptr;
}
int initAppServer(AppServerPtr ptr){
    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    initStore();
    addUser(ptr->usersList,"test","pw",1,2);
    long offset =  storeUsers(ptr->usersList);
    printf("%ld\n",offset );
    ListPtr l = restoreUsers(offset);
    printf("%d\n",l->count );
    UserPtr u = popFromList(l);
    printf("%s\n",u->userName );
    exit(1);
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
    consoleStart(ptr->transfarServer);
    while(1){
        sleep(1);
        srvProcess(ptr->transfarServer);   
    }
    //tick
}
int processRequest(ConnectPtr ptr,int ev){
    if(ev == 'r'){    
    	int result = getRequestData(ptr);
        switch(ptr->state){
            case CONNECT_STATE_READ_MESSAGE_READY:
                break;
            case CONNECT_STATE_READ_WAIT_DATA:
                return APP_SERVER_SUCCESS;
            case CONNECT_STATE_READ_FORMAT_ERROR:
                setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_WAIT);
                
                setNetMessageError(ptr->netMessage,buildErrorCode(PARSER_ERRORS_MARK,PARSER_ERROR_FORMAT_ERROR),PARSER_ERROR_FORMAT_ERROR_MSG);
                return APP_SERVER_SUCCESS;
        }
        //check user
        if(ptr->state == CONNECT_STATE_READ_MESSAGE_READY){
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
                        aq_router(ptr->tServer->appServer,ptr->netMessage,ptr->user);
                    }else{
                        setNetMessageError(ptr->netMessage,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_ERROR_SECURE_FAILED),AQ_ERROR_USER_ERROR_SECURE_FAILED_MSG);
                    }
                }    
            }else{

                setNetMessageError(ptr->netMessage,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_NOT_SEND),AQ_ERROR_USER_NOT_SEND_MSG);
            }
            setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_WAIT);
                
        }else{
            printf("error connect state %d\n",ptr->state);
        }
    }else if(ev =='h'){
        ptr->halfClose =1;
    }
    return APP_SERVER_SUCCESS;
}
int processResponse(ConnectPtr ptr,int ev){
    if(ev == 'w'){
        setNetMessageSendUser(ptr->netMessage,ptr->tServer->appServer->sendUser);
        sendResponseData(ptr);   
    }else if(ev == 'h'){
        ptr->halfClose =1;
    }
}
int processPusherResponse(ConnectPtr ptr,int ev){
    if(ev =='r'){
    	int result =getRequestData(ptr);
    	switch(ptr->state){
            case CONNECT_STATE_READ_MESSAGE_READY:
                break;
            case CONNECT_STATE_READ_WAIT_DATA:
                return APP_SERVER_SUCCESS;
            case CONNECT_STATE_READ_FORMAT_ERROR:
                setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_WAIT);
                
                setNetMessageError(ptr->netMessage,buildErrorCode(PARSER_ERRORS_MARK,PARSER_ERROR_FORMAT_ERROR),PARSER_ERROR_FORMAT_ERROR_MSG);
                return APP_SERVER_SUCCESS;
        }
        //check user
        if(ptr->state == CONNECT_STATE_READ_MESSAGE_READY){
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
                        aq_push_client_router(ptr->tServer->appServer,ptr->netMessage,ptr->user);
                    }else{
                        setNetMessageError(ptr->netMessage,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_ERROR_SECURE_FAILED),AQ_ERROR_USER_ERROR_SECURE_FAILED_MSG);
                    }
                }    
            }else{
                setNetMessageError(ptr->netMessage,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_NOT_SEND),AQ_ERROR_USER_NOT_SEND_MSG);
            }
            setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_WAIT);
        }
    }else if(ev == 'h'){
        ptr->halfClose =1;
    }
}

int processPusherRequest(ConnectPtr ptr,int ev){//send to target
    if(ev == 'w'){
        setNetMessageSendUser(ptr->netMessage,ptr->tServer->appServer->sendUser);
        sendResponseData(ptr);
    }else if(ev == 'h'){
        ptr->halfClose =1;
    }
}

int tickAppServer(AppServerPtr ptr){
    tickUser(ptr->usersList);
    tickBaseServer(ptr->baseServer);
    tickSubscribeServer(ptr->subscribeServer);
    tickPushServer(ptr->pushServer);
    tickTransfarServer(ptr->transfarServer);
}
int processConsoleIn(ConsolePtr ptr,int ev){
    int result = getConsoleData(ptr);
    switch(ptr->state){
        case CONSOLE_STATE_READ_MESSAGE_READY:
            break;
        case CONSOLE_STATE_READ_WAIT_DATA:
            return APP_SERVER_SUCCESS;
        case CONSOLE_STATE_READ_FORMAT_ERROR:
            setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_WAIT);
                
            setNetMessageError(ptr->netMessage,buildErrorCode(PARSER_ERRORS_MARK,PARSER_ERROR_FORMAT_ERROR),PARSER_ERROR_FORMAT_ERROR_MSG);
            return APP_SERVER_SUCCESS;
    }
    //check user
    if(ptr->state == CONSOLE_STATE_READ_MESSAGE_READY){
        aq_server_router(ptr->tServer->appServer,ptr->netMessage,ptr->tServer->appServer->sendUser);
    }
    setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_WAIT);
                
    processConsoleOut(ptr,'w');    
}
int processConsoleOut(ConsolePtr ptr,int ev){
    sendConsoleData(ptr);
}
int processRequestOut(ConnectPtr ptr,int ev){

}
int processResponseOut(ConnectPtr ptr,int ev){

}