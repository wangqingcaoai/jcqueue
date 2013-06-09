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
        
        ptr->usersList = restoreUsers(appStore.usersList);
        
        ptr->admin =  restoreUser(appStore.admin);
        
        ptr->sendUser = restoreUser(appStore.sendUser);

        ptr->acceptUser =restoreUser(appStore.acceptUser);
        ptr->baseServer = restoreBaseServer(appStore.baseServer,ptr);
        ptr->subscribeServer = restoreSubscribeServer(appStore.subscribeServer,ptr);
        ptr->pushServer =  restorePushServer(appStore.pushServer,ptr);
        //TODO need to think how to use tranfarServer store;
        ptr->storePosition = storePosition;
    }
    return APP_SERVER_SUCCESS;
}
int storeAppServer(AppServerPtr ptr){ 

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
    }
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