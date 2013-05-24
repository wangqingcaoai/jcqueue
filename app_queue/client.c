#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "server.h"
#include "../transfar/console.h"
#include "../transfar/server.h"
#include "../util/config.h"
#include "../errors.h"
#include "request.h"	
AppServerPtr buildClientAppServer(const char* host,const char* port){
    AppServerPtr ptr = allocMem(sizeof(AppServer));
    if(ptr == NULL){
        return ptr;
    }
    ptr->request = (Handle)processClientRequest;
    ptr->response = (Handle)processClientResponse;
    ptr->requestOut = (Handle)processClientRequestOut;
    ptr->responseOut = (Handle)processClientResponseOut;

    ptr->pusherRequest = (Handle)processClientPusherRequest;
    ptr->pusherResponse = (Handle)processClientPusherResponse;
    ptr->consoleIn = (Handle)processClientConsoleIn;
    ptr->consoleOut = (Handle)processClientConsoleOut;
    ptr->tick  = (TickHandle)tickClientAppServer;
    ptr->usersList = buildList();
    ptr->admin = buildUser(getConfig("adminUser","admin"),getConfig("adminPassword","jcqueue"));
    ptr->sendUser =buildUser(getConfig("sendUser","reciver"),getConfig("senderPassword","recv"));
    ptr->acceptUser = buildUser(getConfig("acceptUser","sender"),getConfig("accepterPassword","send"));
    ptr->baseServer = NULL;
    ptr->subscribeServer = NULL;
    ptr->pushServer = NULL;
    ptr->transfarServer = buildTransfarServer(ptr,host,port);
    ptr->requestServer = buildRequestServer(ptr);
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
    freeRequestServer(&(ptr->requestServer));
    freeMem((void**)(void**)&ptr);
    (*pptr) = ptr ;
    return APP_SERVER_SUCCESS;
}
int processClientAppServer(AppServerPtr ptr){
    srvStart(ptr->transfarServer);
    consoleStart(ptr->transfarServer);
    while(1){
        sleep(1);
        srvProcess(ptr->transfarServer);   
    }
}
int processClientRequest(ConnectPtr ptr,int ev){
    if(ev == 'r'){
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
                }else{
                    int checkState = checkUser(ptr->user,ptr->netMessage->key,ptr->netMessage->password);
                    if(checkState == USER_SUCCESS){
                        //success
                        aq_client_router(ptr->tServer->appServer,ptr->netMessage,ptr->user);
                    }else{
                        setNetMessageError(ptr->netMessage,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_ERROR_SECURE_FAILED),AQ_ERROR_USER_ERROR_SECURE_FAILED_MSG);
                    }
                }    
            }else{
                setNetMessageError(ptr->netMessage,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_NOT_SEND),AQ_ERROR_USER_NOT_SEND_MSG);
            
                
            }
            setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_WAIT);
            return APP_SERVER_SUCCESS;    
        }
    }else if(ev == 'h'){ 
        ptr->halfClose =1;
    }
}

int processClientResponse(ConnectPtr ptr,int ev){
    if(ev == 'w'){
        if(ptr->netMessage->sendState == NETMESSAGE_WRITESTATE_WAIT){
            setNetMessageSendUser(ptr->netMessage,ptr->tServer->appServer->sendUser);    
        }
        sendResponseData(ptr);   
    }else if(ev == 'h'){
        ptr->halfClose =1;
    }
}
int processClientRequestOut(ConnectPtr ptr,int ev){
    if(ev == 'w'){
        setNetMessageSendUser(ptr->netMessage,ptr->tServer->appServer->sendUser);
        sendResponseData(ptr);   
    }else if(ev == 'h'){
        ptr->halfClose =1;
    }
}
int processClientResponseOut(ConnectPtr ptr,int ev){
    if(ev == 'r'){
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
            displayNetMessage(ptr->netMessage);
            if(ptr->netMessage->user != NULL){
                if(ptr->user == NULL){
                    ptr->user = findUser(ptr->tServer->appServer->usersList,ptr->netMessage->user);    
                }else{
                    int checkState = checkUser(ptr->user,ptr->netMessage->key,ptr->netMessage->password);
                    if(checkState == USER_SUCCESS){
                        //success
                        aq_client_router(ptr->tServer->appServer,ptr->netMessage,ptr->user);
                    }else{
                        setNetMessageError(ptr->netMessage,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_ERROR_SECURE_FAILED),AQ_ERROR_USER_ERROR_SECURE_FAILED_MSG);
                    }
                }    
            }else{
                setNetMessageError(ptr->netMessage,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_NOT_SEND),AQ_ERROR_USER_NOT_SEND_MSG);
            
                    
            }
            //setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_WAIT);
            return APP_SERVER_SUCCESS;
        }
    }else if(ev == 'h'){ 
        ptr->halfClose =1;
    }
}


int processClientPusherResponse(ConnectPtr ptr,int ev){
    if(ev == 'w'){

        setNetMessageSendUser(ptr->netMessage,ptr->tServer->appServer->sendUser);
        sendResponseData(ptr);   
    }else if(ev == 'h'){
        ptr->halfClose =1;
    }
}

int processClientPusherRequest(ConnectPtr ptr,int ev){
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
                        aq_push_server_router(ptr->tServer->appServer,ptr->netMessage,ptr->user);
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

int processClientConsoleIn(ConsolePtr ptr,int ev){
    if(ev == 'r'){
        int result = getConsoleData(ptr);
        switch(ptr->state){
            case CONSOLE_STATE_READ_MESSAGE_READY:
                break;
            case CONSOLE_STATE_READ_WAIT_DATA:
                return APP_SERVER_SUCCESS;
            case CONSOLE_STATE_READ_FORMAT_ERROR:
                setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_WAIT);        
                setNetMessageError(ptr->netMessage,buildErrorCode(PARSER_ERRORS_MARK,PARSER_ERROR_FORMAT_ERROR),PARSER_ERROR_FORMAT_ERROR_MSG);
                
        }
        //check user
        if(ptr->state == CONSOLE_STATE_READ_MESSAGE_READY){
            aq_client_router(ptr->tServer->appServer,ptr->netMessage,ptr->tServer->appServer->sendUser);
        }
        setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_WAIT);
        processClientConsoleOut(ptr,'w');
            
    }else if(ev == 'h'){
        // close
    }
    return APP_SERVER_SUCCESS;    
}
int processClientConsoleOut(ConsolePtr ptr,int ev){
    if(ev == 'w'){

        sendConsoleData(ptr);   
    }else if(ev = 'h'){
        //process close
    }
}

int tickClientAppServer(AppServerPtr ptr){
    tickTransfarServer(ptr->transfarServer);

    tickRequestServer(ptr->requestServer);

}