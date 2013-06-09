#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "router.h"
#include "../errors.h"
#include "subscribe.h"
#include "request.h"
int aq_router(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL){
        aq_empty_appServer(serverPtr,ptr,uptr);
    }
    if(ptr == NULL || uptr == NULL){
        return -1;
    }
    if(ptr->cmd == NULL){
        return aq_unknow_cmd(serverPtr, ptr,uptr);
    }
    int result = 0;
    if(isSameString(ptr->cmd,AQ_SUBSCRIBE_ADD)){
        result =  aq_subscribe_add(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_SUBSCRIBE_DEL)){
        result =  aq_subscribe_del(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_SUBSCRIBE_DEL_BY_USER)){
        result =  aq_subscribe_del_by_user(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_SUBSCRIBE_DEL_BY_KEYWORD_AND_USER)){
        result =  aq_subscribe_del_by_keyword_and_user(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_SUBSCRIBE_GET_IDS)){
        result =  aq_subscribe_get_ids(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_SUBSCRIBE_GET_BY_ID)){
        result =  aq_subscribe_get_by_id(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_SUBSCRIBE_GET_TOPICS_BY_ID)){
        result =  aq_subscribe_get_topics_by_id(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_SYSTEM_STATE)){
        result =  aq_system_state(serverPtr , ptr,uptr);
    }
    else{
        result = bq_router(serverPtr->baseServer, ptr,uptr);
        //aq_filter(serverPtr,ptr,result);
    }
    return result;
    
}
static int aq_empty_appServer(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){

}
static int aq_unknow_cmd(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){

}
static int aq_subscribe_add(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    int result = addSubscribe(serverPtr->subscribeServer,uptr,ptr);
    if(result == SUBSCRIBE_ERROR_PARAM_EMPTY){
        setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_DEL_SUBSCRIBE_SUCCESS),AQ_DEL_SUBSCRIBE_SUCCESS_MSG);
    }else if(result == SUBSCRIBE_SUCCESS){
        return setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_DEL_SUBSCRIBE_SUCCESS),AQ_DEL_SUBSCRIBE_SUCCESS_MSG);

    }
    return setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_ADD_SUBSCRIBE_SUCCESS),AQ_ADD_SUBSCRIBE_SUCCESS_MSG);
}

static int aq_subscribe_del(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    int result = delSubscribe(serverPtr->subscribeServer,uptr,ptr);
    if(result == SUBSCRIBE_ERROR_SUBSCRIBE_NOT_FOUND){
        setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_DEL_SUBSCRIBE_SUCCESS),AQ_DEL_SUBSCRIBE_SUCCESS_MSG);
    }else if(result == SUBSCRIBE_SUCCESS){
        return setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_DEL_SUBSCRIBE_SUCCESS),AQ_DEL_SUBSCRIBE_SUCCESS_MSG);

    }

}
static int aq_subscribe_del_by_user(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    int result = delSubscribeByUser(serverPtr->subscribeServer,uptr,ptr);
    //return setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_DEL_SUBSCRIBE_SUCCESS),AQ_DEL_SUBSCRIBE_SUCCESS_MSG,subscribe_id);


}
static int aq_subscribe_del_by_keyword_and_user(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    return delSubscribebyKeywordAndUser(serverPtr->subscribeServer,uptr,ptr);

}
static int aq_subscribe_get_ids(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    getSubscribeIdsByUser(serverPtr->subscribeServer,uptr,ptr);
    //return setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_DEL_SUBSCRIBE_SUCCESS),AQ_DEL_SUBSCRIBE_SUCCESS_MSG,subscribe_id);


}
static int aq_subscribe_get_by_id(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    getSubscribeInfoById(serverPtr->subscribeServer,uptr,ptr);
    //return setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_DEL_SUBSCRIBE_SUCCESS),AQ_DEL_SUBSCRIBE_SUCCESS_MSG,subscribe_id);


}static int aq_subscribe_get_topics_by_id(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
   
    getSubscribeTopicIdsByUser(serverPtr->subscribeServer,uptr,ptr);
   // return setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_DEL_SUBSCRIBE_SUCCESS),AQ_DEL_SUBSCRIBE_SUCCESS_MSG,subscribe_id);


}
static int aq_system_state(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){

}
int aq_client_router(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL){
        aq_empty_appServer(serverPtr,ptr,uptr);
    }
    if(ptr == NULL || uptr == NULL){
        return -1;
    }
    if(ptr->cmd == NULL){
        return aq_unknow_cmd(serverPtr, ptr,uptr);
    }
    int result = 0;
    if(!strcasecmp(ptr->cmd,AQ_CLIENT_SERVER_ADD)){
        result =  aq_client_server_add(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_CLIENT_SERVER_LIST)){
        result =  aq_client_server_list(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_CLIENT_SERVER_DEL)){
        result =  aq_client_server_del(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_CLIENT_SERVER_USE)){
        result =  aq_client_server_use(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_CLIENT_CLOSE)){
        result =  aq_client_close(serverPtr , ptr,uptr);
    }else if(isSameString(ptr->protocolType,NETMESSAGE_TYPE_CMD)){
        result = aq_client_to_server(serverPtr,ptr,uptr);
    }else{

    }
    return result;
    
}

int aq_push_server_router(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){

}
int aq_push_client_router(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){

}
int aq_client_server_add(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr== NULL ||uptr == NULL){
        return -1;
    }
    char* host = getExtraParam(ptr,"host");
    char* port = getExtraParam(ptr,"port");
    if(host == NULL){
        setNetMessageError(ptr,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR),AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR_MSG,host,port);
        return 0;
    }
    if(port == NULL){
        setNetMessageError(ptr,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR),AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR_MSG,host,port);
        return 0;
    }else{
        int portInt = atoi(port);
        if(portInt<=0 ||portInt>65535){
            setNetMessageError(ptr,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR),AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR_MSG,host,port);
            return 0;
    
        }
    }
    int result = addRequester(serverPtr->requestServer,uptr,host,port);
    if(result == REQUEST_SUCCESS){
        setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_CLIENT_SERVER_ADD_SUCCESS),AQ_CLIENT_SERVER_ADD_SUCCESS_MSG,host,port);
    }else{
        setNetMessageError(ptr,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_ADD_FAILED),AQ_CLIENT_ERROR_SERVER_ADD_FAILED_MSG,host,port);
    }
    freeString(&host);
    freeString(&port);
    return 0;
}
int aq_client_server_list(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr== NULL ||uptr == NULL){
        return -1;
    }
    int result =getRequesterList(serverPtr->requestServer,ptr);
    if(result == REQUEST_SUCCESS){
        setNetMessageSendCMDData(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_CLIENT_SERVER_LIST_SUCCESS),NULL);
    }
}
int aq_client_server_del(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr== NULL ||uptr == NULL){
        return -1;
    }
    char* param = getExtraParam(ptr,"id");
    if(param == NULL){
        setNetMessageError(ptr,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_DEL_PARAM_EMPTY),AQ_CLIENT_ERROR_SERVER_DEL_PARAM_EMPTY_MSG);
        return 0;
    }
    int request_id = atoi(param);
    int result = delRequesterFromServer(serverPtr->requestServer,request_id);
    if(result == REQUEST_SUCCESS){
        setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_CLIENT_SERVER_DEL_SUCCESS),AQ_CLIENT_SERVER_DEL_SUCCESS_MSG,request_id);
    }else if(result == REQUEST_ERROR_REQUESTER_NOT_FOUND){
        setNetMessageError(ptr,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_NOT_FOUND),AQ_CLIENT_ERROR_SERVER_NOT_FOUND_MSG,request_id);
    }
    freeString(&param);
    return 0;
}
int aq_client_server_use(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr== NULL ||uptr == NULL){
        return -1;
    }
    char* param = getExtraParam(ptr,"id");
    if(param == NULL){
        setNetMessageError(ptr,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_USE_PARAM_EMPTY),AQ_CLIENT_ERROR_SERVER_USE_PARAM_EMPTY_MSG);
        return 0;
    }
    int request_id = atoi(param);
    int result = changeCurrentRequester(serverPtr->requestServer,request_id);
    if(result == REQUEST_SUCCESS){
        setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_CLIENT_SERVER_USE_SUCCESS),AQ_CLIENT_SERVER_USE_SUCCESS_MSG,request_id);
    }else if(result == REQUEST_ERROR_REQUESTER_NOT_FOUND){
        setNetMessageError(ptr,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_NOT_FOUND),AQ_CLIENT_ERROR_SERVER_NOT_FOUND_MSG,request_id);
    }
    freeString(&param);
}

int aq_client_close(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    stopClientAppServer(serverPtr);
}
static int aq_client_to_server(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL|| ptr==NULL || uptr == NULL){
        return -1;
    }
    RequesterPtr rptr = serverPtr->requestServer->current;
    if(rptr == NULL){
        setNetMessageError(ptr,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_TO_SERVER_FAILED_NO_SERVER_USING),AQ_CLIENT_TO_SERVER_FAILED_NO_SERVER_USING_MSG);        
    }
    int result = addMessageToRequester(rptr,ptr);
    if(result == REQUEST_SUCCESS){
        setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_CLIENT_TO_SERVER_SUCCESS),AQ_CLIENT_TO_SERVER_SUCCESS_MSG,rptr->remoteHost,rptr->remotePort);
        return 0;
    }

}
int aq_server_router(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
        if(serverPtr == NULL){
        aq_empty_appServer(serverPtr,ptr,uptr);
    }
    if(ptr == NULL || uptr == NULL){
        return -1;
    }
    if(ptr->cmd == NULL){
        return aq_unknow_cmd(serverPtr, ptr,uptr);
    }
    int result = 0;
    if(!strcasecmp(ptr->cmd,AQ_SERVER_ADD_USER)){
        result =  aq_server_add_user(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_SERVER_DEL_USER)){
        result =  aq_server_del_user(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_SERVER_GET_USER)){
        result =  aq_server_get_user(serverPtr , ptr,uptr);
    }else{
        
    }
    return result;
}
static int aq_server_add_user(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL|| ptr==NULL || uptr == NULL){
        return -1;
    }
    
    char* user = getExtraParam(ptr,"user");
    char* passwd = getExtraParam(ptr,"password");
    char* group = getExtraParam(ptr,"group");
    char* privilege = getExtraParam(ptr,"privilege");
    int uprivilege,ugroup; 
    if(privilege != NULL){
        uprivilege = atoi(privilege);
    }
    if(group !=NULL){
        ugroup= atoi(group);  
    }
    if(user == NULL){
        setNetMessageError(ptr,buildErrorCode(AQ_ERRORS_MARK,AQ_SERVER_ERROR_SERVER_USER_PARAM_EMPTY),AQ_SERVER_ERROR_SERVER_USER_PARAM_EMPTY_MSG);
        return 0;
    }
    
    if( 0<=uprivilege || uprivilege > USER_PRIVILEGE_DEFAULT ){
        uprivilege = USER_PRIVILEGE_DEFAULT;
    }
    if(0<=ugroup || ugroup > USER_GROUP_DEFAULT){
        ugroup = USER_GROUP_DEFAULT;
    }
    int result = addUser(serverPtr->usersList,user,passwd,uprivilege,ugroup);
    if(result == REQUEST_SUCCESS){
        setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_SERVER_ADD_USER_SUCCESS),AQ_SERVER_ADD_USER_SUCCESS_MSG,user);
    }else if(result == USER_ERROR_USER_EXIST){
        setNetMessageError(ptr,buildErrorCode(AQ_ERRORS_MARK,AQ_SERVER_ERROR_SERVER_USER_EXISTS),AQ_SERVER_ERROR_SERVER_USER_EXISTS_MSG,user);
    }
    freeString(&user);
    freeString(&passwd);
    freeString(&group);
    freeString(&privilege);

}
static int aq_server_get_user(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL|| ptr==NULL || uptr == NULL){
        return -1;
    }
    

}
static int aq_server_del_user(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){
    // if(serverPtr == NULL|| ptr==NULL || uptr == NULL){
    //     return -1;
    // }
    // RequesterPtr rptr = serverPtr->requestServer->current;
    // int result = addMessageToRequester(rptr,ptr);
    // if(result == REQUEST_SUCCESS){
    //     setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_CLIENT_TO_SERVER_SUCCESS),AQ_CLIENT_TO_SERVER_SUCCESS_MSG,rptr->remoteHost,rptr->remotePort);
    //     return 0;
    // }

}
