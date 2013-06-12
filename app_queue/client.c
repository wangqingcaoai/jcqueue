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
    ptr->request = (Handle)processClientPusherRequest;
    ptr->response = (Handle)processClientPusherResponse;
    ptr->requestOut = (Handle)processClientRequestOut;
    ptr->responseOut = (Handle)processClientResponseOut;

    //ptr->pusherRequest = (Handle)processClientPusherRequest;
   // ptr->pusherResponse = (Handle)processClientPusherResponse;
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
    initStore();
    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    AppServerStore appStore;
    long storePosition = getStartStorePosition();
    if(storePosition <=0){
        return APP_SERVER_SUCCESS;
    }else{
        restore(storePosition,&appStore,sizeof(AppServerStore));
        if(ptr->usersList!=NULL){
            freeList(&(ptr->usersList),(Free)freeUser);
        }
        if(ptr->admin != NULL){
            freeUser(&(ptr->admin));
        }
        if(ptr->acceptUser != NULL){
            freeUser(&(ptr->acceptUser));
        }
        if(ptr->sendUser !=NULL){
            freeUser(&(ptr->sendUser));
        }
        if(ptr->baseServer !=NULL){
            freeBaseServer(&(ptr->baseServer));
        }
        if(ptr->subscribeServer!=NULL){
            freeSubscribeServer(&(ptr->subscribeServer));
        }
        if(ptr->pushServer!=NULL){
            freePushServer(&(ptr->pushServer));
        }
        // if(ptr->transfarServer !=NULL){
        //     freeTransfarServer(&(ptr->transfarServer));
        // }
        // 不能改变这些语句的顺序
        ptr->maxIds = initMaxIds(appStore.maxIds);
        
        ptr->usersList = restoreUsers(appStore.usersList);
        
        ptr->admin =  restoreUser(appStore.admin);
        
        ptr->sendUser = restoreUser(appStore.sendUser);

        ptr->acceptUser =restoreUser(appStore.acceptUser);
        // ptr->baseServer = restoreBaseServer(appStore.baseServer,ptr);
        // ptr->subscribeServer = restoreSubscribeServer(appStore.subscribeServer,ptr);
        // ptr->pushServer =  restorePushServer(appStore.pushServer,ptr);
        // //TODO need to think how to use tranfarServer store;
        ptr->requestServer = restoreRequestServer(appStore.requestServer,ptr);
        
        ptr->storePosition = storePosition;
    }
    return APP_SERVER_SUCCESS;
}
int storeClientAppServer(AppServerPtr ptr){ 

    if(ptr == NULL){
        return APP_SERVER_ERROR_PARAM_ERROR;
    }
    AppServerStore appStore;
    if(ptr->storePosition > 0){
        restore(ptr->storePosition,&appStore,sizeof(AppServerStore));
    }else{
        appStore.usersList = 0L;
        appStore.admin = 0L;
        appStore.sendUser = 0L;
        appStore.acceptUser = 0L;
        appStore.baseServer = 0L;
        appStore.subscribeServer = 0L;
        appStore.pushServer = 0L;
        appStore.transfarServer = 0L;
        appStore.requestServer = 0L;
        appStore.maxIds = 0L;
    }
    appStore.maxIds = initMaxIds(ptr->maxIds);
    appStore.usersList = storeUsers(ptr->usersList);
    appStore.admin = storeUser(ptr->admin);
    appStore.sendUser = storeUser(ptr->sendUser);
    appStore.acceptUser = storeUser(ptr->acceptUser);
    appStore.baseServer = storeBaseServer(ptr->baseServer);
    appStore.subscribeServer = storeSubscribeServer(ptr->subscribeServer);
    appStore.pushServer = storePushServer(ptr->pushServer);
    appStore.transfarServer = 0L;
    appStore.requestServer = storeRequestServer(ptr->requestServer);
    ptr->storePosition = store(ptr->storePosition,&appStore,sizeof(AppServerStore));
    setStartStorePosition(ptr->storePosition);
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
                
                setNetMessageError(ptr->netMessage,RESPONSE_ERROR,buildErrorCode(PARSER_ERRORS_MARK,PARSER_ERROR_FORMAT_ERROR),PARSER_ERROR_FORMAT_ERROR_MSG);
                return APP_SERVER_SUCCESS;
        }
     
        setNetMessageReadState(ptr->netMessage,NETMESSAGE_READSTATE_WAIT);
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
                        setNetMessageError(ptr->netMessage,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_ERROR_SECURE_FAILED),AQ_ERROR_USER_ERROR_SECURE_FAILED_MSG);
                    }
                }    
            }else{
                setNetMessageError(ptr->netMessage,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_NOT_SEND),AQ_ERROR_USER_NOT_SEND_MSG);
            
                
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
                setNetMessageError(ptr->netMessage,RESPONSE_ERROR,buildErrorCode(PARSER_ERRORS_MARK,PARSER_ERROR_FORMAT_ERROR),PARSER_ERROR_FORMAT_ERROR_MSG);
                return APP_SERVER_SUCCESS;
        }
        
   //     setNetMessageReadState(ptr->netMessage,NETMESSAGE_READSTATE_WAIT);
        //check user
        if(ptr->state == CONNECT_STATE_READ_MESSAGE_READY){
            //displayNetMessage(ptr->netMessage);
            if(ptr->netMessage->user != NULL){
                if(!isSameString(ptr->tServer->appServer->acceptUser->userName,ptr->netMessage->user)){
                    setNetMessageError(ptr->netMessage,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_NOT_FOUND),AQ_ERROR_USER_NOT_FOUND_MSG,ptr->netMessage->user);
                }else{
                    ptr->user = ptr->tServer->appServer->acceptUser;
                    int checkState = checkUser(ptr->tServer->appServer->acceptUser,ptr->netMessage->key,ptr->netMessage->password);
                    if(checkState == USER_SUCCESS){
                        //success
                        aq_client_request_router(ptr->tServer->appServer,ptr->netMessage,ptr->user);
                        setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_FINISH);
                        return APP_SERVER_SUCCESS;
                    }else{
                        setNetMessageError(ptr->netMessage,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_ERROR_SECURE_FAILED),AQ_ERROR_USER_ERROR_SECURE_FAILED_MSG);
                    }
                }    
            }else{
                setNetMessageError(ptr->netMessage,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_NOT_SEND),AQ_ERROR_USER_NOT_SEND_MSG);
            
                    
            }
            setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_WAIT);
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
        setConnectRead(ptr);
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
                setNetMessageError(ptr->netMessage,RESPONSE_ERROR,buildErrorCode(PARSER_ERRORS_MARK,PARSER_ERROR_FORMAT_ERROR),PARSER_ERROR_FORMAT_ERROR_MSG);
                return APP_SERVER_SUCCESS;
        }

        setNetMessageReadState(ptr->netMessage,NETMESSAGE_READSTATE_WAIT);
        //check user
        if(ptr->state == CONNECT_STATE_READ_MESSAGE_READY){
            if(ptr->netMessage->user != NULL){
                                
                if(!isSameString(ptr->tServer->appServer->acceptUser->userName,ptr->netMessage->user)){
                    setNetMessageError(ptr->netMessage,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_NOT_FOUND),AQ_ERROR_USER_NOT_FOUND_MSG,ptr->netMessage->user);
                }else{
                    ptr->user = ptr->tServer->appServer->acceptUser;
                    int checkState = checkUser(ptr->user,ptr->netMessage->key,ptr->netMessage->password);
                    if(checkState == USER_SUCCESS){
                        //success
                      int result =   aq_push_server_router(ptr->tServer->appServer,ptr->netMessage,ptr->user);
                        if(AQ_STOP_SEND == result){

                            setNetMessageSendState(ptr->netMessage,NETMESSAGE_WRITESTATE_FINISH);
                            return APP_SERVER_SUCCESS;
                        }
                    }else{
                        setNetMessageError(ptr->netMessage,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_ERROR_SECURE_FAILED),AQ_ERROR_USER_ERROR_SECURE_FAILED_MSG);
                    }
                }    
            }else{
                setNetMessageError(ptr->netMessage,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_ERROR_USER_NOT_SEND),AQ_ERROR_USER_NOT_SEND_MSG);
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
                setNetMessageError(ptr->netMessage,RESPONSE_ERROR,buildErrorCode(PARSER_ERRORS_MARK,PARSER_ERROR_FORMAT_ERROR),PARSER_ERROR_FORMAT_ERROR_MSG);
                
        }

        setNetMessageReadState(ptr->netMessage,NETMESSAGE_READSTATE_WAIT);
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
   // printf("store data\n");
    storeClientAppServer(ptr);

}

int stopClientAppServer(AppServerPtr ptr){
    storeClientAppServer(ptr);
    stopTransfarServer(ptr->transfarServer);
    freeClientAppServer(&ptr);
    printf("client closed success\n" );
    exit(1);
}