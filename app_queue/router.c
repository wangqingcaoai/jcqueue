#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "router.h"
#include "../errors.h"
#include "subscribe.h"
#include "../base_queue/router.h"
#include "request.h"
#include "../secure/secure.h"
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
    if(!strcasecmp(ptr->cmd,AQ_SUBSCRIBE_ADD)){
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
    }else if(!strcasecmp(ptr->cmd,RESPONSE_ERROR)){
        result = aq_response_error(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,RESPONSE_OK)){
        result = aq_response_ok(serverPtr , ptr,uptr);
    }
    else{
        result = bq_router(serverPtr->baseServer, ptr);
        //aq_filter(serverPtr,ptr,result);
    }
    return result;
    
}

static int aq_response_ok(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    printf("cmd success!\n");

    if(ptr->length < 0){
        ptr->length = 0;
    }
    char message[ptr->length+1];

    if(ptr->length >0){
        memcpy(message,ptr->data,ptr->length);
    }
    message[ptr->length] = '\0';
    printf("%d %s %s\n", ptr->errcode,ptr->extraParam,message );
    return AQ_STOP_SEND;
}

static int aq_response_error(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    printf("cmd error!\n" );

    if(ptr->length < 0){
        ptr->length = 0;
    }
    char message[ptr->length+1];

    if(ptr->length >0){
        memcpy(message,ptr->data,ptr->length);
    }
    message[ptr->length] = '\0';
    printf("%d %s\n", ptr->errcode,message );
    return AQ_STOP_SEND;
}
static int aq_empty_appServer(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    return setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_APPSERVER_EMPTY),AQ_APPSERVER_EMPTY_MSG);
}
static int aq_unknow_cmd(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    printf("unknow cmd\n");
    return setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_UNKNOW_CMD),AQ_UNKNOW_CMD_MSG,ptr->cmd);
}
static int aq_subscribe_add(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    int result = addSubscribe(serverPtr->subscribeServer,uptr,ptr);
    if(result == SUBSCRIBE_ERROR_PARAM_EMPTY){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_ADD_SUBSCRIBE_PARAM_ERROR),AQ_ADD_SUBSCRIBE_PARAM_ERROR_MSG);
    }else if(result == SUBSCRIBE_SUCCESS){
        printf("add subscribe success\n");
        return setNetMessageError(ptr,RESPONSE_OK,buildErrorCode(AQ_SUCCESS_MARK,AQ_ADD_SUBSCRIBE_SUCCESS),AQ_ADD_SUBSCRIBE_SUCCESS_MSG);

    }
}

static int aq_subscribe_del(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    int result = delSubscribe(serverPtr->subscribeServer,uptr,ptr);
    if(result == SUBSCRIBE_ERROR_SUBSCRIBE_NOT_FOUND){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_SUBSCRIBE_ERROR_SUBSCRIBE_NOT_FOUND),AQ_SUBSCRIBE_ERROR_SUBSCRIBE_NOT_FOUND_MSG);
    }else if(result == SUBSCRIBE_SUCCESS){
        printf("del subscribe success\n");
        return setNetMessageError(ptr,RESPONSE_OK,buildErrorCode(AQ_SUCCESS_MARK,AQ_DEL_SUBSCRIBE_SUCCESS),AQ_DEL_SUBSCRIBE_SUCCESS_MSG);

    }
}
static int aq_subscribe_del_by_user(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    //int result = delSubscribeByUser(serverPtr->subscribeServer,uptr,ptr);
    //return setNetMessageError(ptr,RESPONSE_OK,buildErrorCode(AQ_SUCCESS_MARK,AQ_DEL_SUBSCRIBE_SUCCESS),AQ_DEL_SUBSCRIBE_SUCCESS_MSG,subscribe_id);

}
static int aq_subscribe_del_by_keyword_and_user(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    //return delSubscribebyKeywordAndUser(serverPtr->subscribeServer,uptr,ptr);

}
static int aq_subscribe_get_ids(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    //getSubscribeIdsByUser(serverPtr->subscribeServer,uptr,ptr);
    //return setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_DEL_SUBSCRIBE_SUCCESS),AQ_DEL_SUBSCRIBE_SUCCESS_MSG,subscribe_id);


}
static int aq_subscribe_get_by_id(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    //getSubscribeInfoById(serverPtr->subscribeServer,uptr,ptr);
    //return setNetMessageError(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_DEL_SUBSCRIBE_SUCCESS),AQ_DEL_SUBSCRIBE_SUCCESS_MSG,subscribe_id);


}static int aq_subscribe_get_topics_by_id(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
   
    //getSubscribeTopicIdsByUser(serverPtr->subscribeServer,uptr,ptr);
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
    }else if(!strcasecmp(ptr->cmd,RESPONSE_ERROR)){
        result = aq_response_error(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,RESPONSE_OK)){
        result = aq_response_ok(serverPtr , ptr,uptr);
    }else if(isSameString(ptr->protocolType,NETMESSAGE_TYPE_CMD)){
        result = aq_client_to_server(serverPtr,ptr,uptr);
    }else{
        return aq_unknow_cmd(serverPtr, ptr,uptr);
   
    }
    return result;
    
}
int aq_client_request_router(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
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
    if(!strcasecmp(ptr->cmd,RESPONSE_OK)){
        result = aq_response_ok(serverPtr, ptr,uptr);
    }else if(isSameString(ptr->cmd,RESPONSE_ERROR)){
        result = aq_response_error(serverPtr, ptr,uptr);
    }else{
        printf("unknow response\n");
    }
    return result;
    
}

static int aq_client_response_ok(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    printf("cmd success!\n");

    if(ptr->length < 0){
        ptr->length = 0;
    }
    char message[ptr->length+1];

    if(ptr->length >0){
        memcpy(message,ptr->data,ptr->length);
    }
    message[ptr->length] = '\0';
    printf("%d %s %s\n", ptr->errcode,ptr->extraParam,message );
    return AQ_STOP_SEND;
}

static int aq_client_response_error(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    printf("cmd error!\n" );

    if(ptr->length < 0){
        ptr->length = 0;
    }
    char message[ptr->length+1];

    if(ptr->length >0){
        memcpy(message,ptr->data,ptr->length);
    }
    message[ptr->length] = '\0';
    printf("%d %s\n", ptr->errcode,message );
    return AQ_STOP_SEND;
}
int aq_push_server_router(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL){
        aq_empty_appServer(serverPtr,ptr,uptr);
    }
    if(ptr == NULL || uptr == NULL){
        return -1;
    }
    if(ptr->cmd == NULL){
        return aq_push_unknow_cmd(serverPtr, ptr,uptr);
    }
    int result;
    if(!strcasecmp(ptr->cmd,AQ_PUSH_SERVER_PUSH)){
        result =  aq_push_server_push(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_PUSH_SERVER_FINISH)){
        result = aq_push_server_finish(serverPtr,ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,RESPONSE_ERROR)){
        result = aq_response_error(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,RESPONSE_OK)){
        result = aq_response_ok(serverPtr , ptr,uptr);
    }
    return result;
}
static aq_push_unknow_cmd(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    printf("unknow push cmd get %s\n",ptr->cmd );
}

static int aq_push_server_push(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    
    char * topicName = getExtraParam(ptr,"topic_name");
    char * messageId = getExtraParam(ptr,"message_id");
    char * priority = getExtraParam(ptr,"priority");
    char * delay = getExtraParam(ptr,"delay");
    char * activetime = getExtraParam(ptr,"activetime");
    char * timestamp = getExtraParam(ptr,"timestamp");
    if(ptr->length< 0){
        ptr->length = 0;
    }
    char data[ptr->length+1];
    if(ptr->length >0){
        memcpy(data,ptr->data,ptr->length);    
    }
    data[ptr->length]='\0';
    printf("get pushed message from topic (%s)\n"\
        "message_id:%s "\
        "priority:%s "\
        "delay:%s "\
        "activetime:%s "\
        "timestamp:%s "\
        "data:%s "\
        "length:%d \n",
        topicName,messageId,priority,delay,activetime,timestamp,data,ptr->length
        );
    setNetMessageSendCMDData(ptr,buildErrorCode(PUSH_SUCCESS_MARK,PUSH_CLIENT_RECIVED_SUCCESS),AQ_PUSH_CLIENT_RECIVED);               
    setSendExtraParam(ptr,"message_id",messageId);
    setSendExtraParam(ptr,"topic_name",topicName);
    freeString(&topicName);
    freeString(&messageId);
    freeString(&priority);
    freeString(&delay);
    freeString(&activetime);
    freeString(&timestamp);
    return 0;
}

static int aq_push_server_finish(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr == NULL || uptr==NULL){
        return -1;
    }
    if(ptr->length < 0){
        ptr->length = 0;
    }
    char message[ptr->length+1];

    if(ptr->length >0){
        memcpy(message,ptr->data,ptr->length);
    }
    message[ptr->length] = '\0';
    printf("%d %s\n", ptr->errcode,message );
    return AQ_STOP_SEND;
}
//process client response
int aq_push_client_router(PusherPtr push,NetMessagePtr ptr,UserPtr uptr){
    if(push == NULL || ptr == NULL ||uptr == NULL){
        return -1;
    }
    char* messageIdStr = getExtraParam(ptr,"message_id");
    int64 messageId =0;
    if(messageIdStr != NULL){
        messageId= atoll(messageIdStr);
    }
    char* topicName = getExtraParam(ptr,"topic_name");
    if(topicName == NULL || messageId <=0){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_PUSH_RECIVED_ERROR_PARAM_ERROR),AQ_PUSH_RECIVED_ERROR_PARAM_ERROR_MSG,messageId,topicName);
    }else{
        int result = pushMessageRecived(push,messageId,topicName);
        if(PUSH_SUCCESS == result) {
            printf("push message recived success\n");
            setNetMessageError(ptr,AQ_PUSH_SERVER_FINISH,buildErrorCode(AQ_SUCCESS_MARK,AQ_PUSH_RECIVED_SUCCESS),AQ_PUSH_RECIVED_SUCCESS_MSG,messageId,topicName);  
        }else if(PUSH_MESSAGE_NOT_FOUND == result){
            setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_PUSH_RECIVED_ERROR_MESSAGE_NOT_FOUND),AQ_PUSH_RECIVED_ERROR_MESSAGE_NOT_FOUND_MSG,messageId,topicName);          
        }    
    }
    freeString(&topicName);
    freeString(&messageIdStr);
}
static int aq_client_server_add(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr== NULL ||uptr == NULL){
        return -1;
    }
    char* host = getExtraParam(ptr,"host");
    char* port = getExtraParam(ptr,"port");
    if(host == NULL){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR),AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR_MSG,host,port);
        return 0;
    }
    if(port == NULL){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR),AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR_MSG,host,port);
        return 0;
    }else{
        int portInt = atoi(port);
        if(portInt<=0 ||portInt>65535){
            setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR),AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR_MSG,host,port);
            return 0;
    
        }
    }
    int result = addRequester(serverPtr->requestServer,uptr,host,port);
    if(result == REQUEST_SUCCESS){
        setNetMessageError(ptr,RESPONSE_OK,buildErrorCode(AQ_SUCCESS_MARK,AQ_CLIENT_SERVER_ADD_SUCCESS),AQ_CLIENT_SERVER_ADD_SUCCESS_MSG,host,port);
    }else{
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_ADD_FAILED),AQ_CLIENT_ERROR_SERVER_ADD_FAILED_MSG,host,port);
    }
    freeString(&host);
    freeString(&port);
    return 0;
}
static int aq_client_server_list(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr== NULL ||uptr == NULL){
        return -1;
    }
    int result =getRequesterList(serverPtr->requestServer,ptr);
    if(result == REQUEST_SUCCESS){
        setNetMessageSendCMDData(ptr,buildErrorCode(AQ_SUCCESS_MARK,AQ_CLIENT_SERVER_LIST_SUCCESS),RESPONSE_OK);
    }
}
static int aq_client_server_del(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr== NULL ||uptr == NULL){
        return -1;
    }
    char* param = getExtraParam(ptr,"id");
    if(param == NULL){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_DEL_PARAM_EMPTY),AQ_CLIENT_ERROR_SERVER_DEL_PARAM_EMPTY_MSG);
        return 0;
    }
    int request_id = atoi(param);
    int result = delRequesterFromServer(serverPtr->requestServer,request_id);
    if(result == REQUEST_SUCCESS){
        setNetMessageError(ptr,RESPONSE_OK,buildErrorCode(AQ_SUCCESS_MARK,AQ_CLIENT_SERVER_DEL_SUCCESS),AQ_CLIENT_SERVER_DEL_SUCCESS_MSG,request_id);
    }else if(result == REQUEST_ERROR_REQUESTER_NOT_FOUND){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_NOT_FOUND),AQ_CLIENT_ERROR_SERVER_NOT_FOUND_MSG,request_id);
    }
    freeString(&param);
    return 0;
}
static int aq_client_server_use(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL || ptr== NULL ||uptr == NULL){
        return -1;
    }
    char* param = getExtraParam(ptr,"id");
    if(param == NULL){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_USE_PARAM_EMPTY),AQ_CLIENT_ERROR_SERVER_USE_PARAM_EMPTY_MSG);
        return 0;
    }
    int request_id = atoi(param);
    int result = changeCurrentRequester(serverPtr->requestServer,request_id);
    if(result == REQUEST_SUCCESS){
        setNetMessageError(ptr,RESPONSE_OK,buildErrorCode(AQ_SUCCESS_MARK,AQ_CLIENT_SERVER_USE_SUCCESS),AQ_CLIENT_SERVER_USE_SUCCESS_MSG,request_id);
    }else if(result == REQUEST_ERROR_REQUESTER_NOT_FOUND){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_ERROR_SERVER_NOT_FOUND),AQ_CLIENT_ERROR_SERVER_NOT_FOUND_MSG,request_id);
    }
    freeString(&param);
}

static int aq_client_close(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    stopClientAppServer(serverPtr);
    exit(0);
}
static int aq_client_to_server(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL|| ptr==NULL || uptr == NULL){
        return -1;
    }
    RequesterPtr rptr = serverPtr->requestServer->current;
    if(rptr == NULL){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_CLIENT_TO_SERVER_FAILED_NO_SERVER_USING),AQ_CLIENT_TO_SERVER_FAILED_NO_SERVER_USING_MSG);        
    }
    int result = addMessageToRequester(rptr,ptr);
    if(result == REQUEST_SUCCESS){
        setNetMessageError(ptr,RESPONSE_OK,buildErrorCode(AQ_SUCCESS_MARK,AQ_CLIENT_TO_SERVER_SUCCESS),AQ_CLIENT_TO_SERVER_SUCCESS_MSG,rptr->remoteHost,rptr->remotePort);
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
    }else if(!strcasecmp(ptr->cmd,AQ_SERVER_SET_USER)){
        result =  aq_server_set_user(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_SERVER_LIST_USERS)){
        result =  aq_server_list_users(serverPtr , ptr,uptr);
    }else if(!strcasecmp(ptr->cmd,AQ_SERVER_CLOSE)){
        result = aq_server_close(serverPtr,ptr,uptr);
    }else{
        return aq_unknow_cmd(serverPtr, ptr,uptr);   
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
    int uprivilege =0,ugroup=0; 
    if(privilege != NULL){
        uprivilege = atoi(privilege);
    }
    if(group !=NULL){
        ugroup= atoi(group);  
    }
    if(isEmptyString(user ) || isEmptyString(passwd )){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_SERVER_ERROR_SERVER_USER_PARAM_EMPTY),AQ_SERVER_ERROR_SERVER_USER_PARAM_EMPTY_MSG);
        return 0;
    }
    if( 0<=uprivilege || uprivilege > USER_PRIVILEGE_DEFAULT ){
        uprivilege = USER_PRIVILEGE_DEFAULT;
    }
    if(0<=ugroup || ugroup > USER_GROUP_DEFAULT){
        ugroup = USER_GROUP_DEFAULT;
    }
    char *md5Password = buildMd5Key(passwd);
    int result = addUser(serverPtr->usersList,user,passwd,uprivilege,ugroup);
    if(result == REQUEST_SUCCESS){
        printf("add new user %s (passwd:%s md5:%s)\n",user,passwd,md5Password);
        setNetMessageError(ptr,RESPONSE_OK,buildErrorCode(AQ_SUCCESS_MARK,AQ_SERVER_ADD_USER_SUCCESS),AQ_SERVER_ADD_USER_SUCCESS_MSG,user);
    }else if(result == USER_ERROR_USER_EXIST){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_SERVER_ERROR_SERVER_USER_EXISTS),AQ_SERVER_ERROR_SERVER_USER_EXISTS_MSG,user);
    }
    freeString(&user);
    freeString(&passwd);
    freeString(&group);
    freeString(&privilege);
    freeString(&md5Password);

}
static int aq_server_get_user(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL|| ptr==NULL || uptr == NULL){
        return -1;
    }
    char* user = getExtraParam(ptr,"user");
    if(user == NULL){
        return    setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_SERVER_ERROR_SERVER_USER_PARAM_EMPTY),AQ_SERVER_ERROR_SERVER_USER_PARAM_EMPTY_MSG);   
    }
    UserPtr userPtr = findUser(serverPtr->usersList,user);
    if(userPtr == NULL){
            setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_SERVER_ERROR_SERVER_USER_NOT_EXISTS),AQ_SERVER_ERROR_SERVER_USER_NOT_EXISTS_MSG,user);
    }else{
        return setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_SUCCESS_MARK,AQ_SERVER_ERROR_SERVER_USER_FOUND),AQ_SERVER_ERROR_SERVER_USER_FOUND_MSG,userPtr->userName,userPtr->userPassword,userPtr->privilege,userPtr->group);
    }
    

}
static int aq_server_del_user(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL|| ptr==NULL || uptr == NULL){
        return -1;
    }
    if(serverPtr == NULL|| ptr==NULL || uptr == NULL){
        return -1;
    }
    char* user = getExtraParam(ptr,"user");
    if(user == NULL){
        return    setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_SERVER_ERROR_SERVER_USER_PARAM_EMPTY),AQ_SERVER_ERROR_SERVER_USER_PARAM_EMPTY_MSG);   
    }
    int result = delUser(serverPtr->usersList,user);
    if(result == USER_ERROR_USER_NOT_FOUND){
            setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_SERVER_ERROR_SERVER_USER_NOT_EXISTS),AQ_SERVER_ERROR_SERVER_USER_NOT_EXISTS_MSG,user);
    }else{
        return setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_SUCCESS_MARK,AQ_SERVER_ERROR_SERVER_USER_DEL_SUCCESS),AQ_SERVER_ERROR_SERVER_USER_DEL_SUCCESS_MSG,user);
    }

}
static int aq_server_set_user(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    if(serverPtr == NULL|| ptr==NULL || uptr == NULL){
        return -1;
    }
    
    char* user = getExtraParam(ptr,"user");
    
    if(user == NULL){
        setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_SERVER_ERROR_SERVER_USER_PARAM_EMPTY),AQ_SERVER_ERROR_SERVER_USER_PARAM_EMPTY_MSG);
        return 0;
    }
    UserPtr userPtr =  findUser(serverPtr->usersList,user);
    if(userPtr == NULL){
            setNetMessageError(ptr,RESPONSE_ERROR,buildErrorCode(AQ_ERRORS_MARK,AQ_SERVER_ERROR_SERVER_USER_NOT_EXISTS),AQ_SERVER_ERROR_SERVER_USER_NOT_EXISTS_MSG,user);
    }
    char* passwd = getExtraParam(ptr,"password");
    if(passwd != NULL){
        setUserPassWord(userPtr,passwd);
    }
    int uprivilege =0,ugroup =0; 
    char* privilege = getExtraParam(ptr,"privilege");
    
    if(privilege != NULL){
        uprivilege = atoi(privilege);
        if( 0<=uprivilege || uprivilege > USER_PRIVILEGE_DEFAULT ){
            uprivilege = USER_PRIVILEGE_DEFAULT;
        }
        setUserPrivilege(userPtr,uprivilege);
    }
    char* group = getExtraParam(ptr,"group");
    
    if(group !=NULL){
        ugroup= atoi(group);  
        if(0<=ugroup || ugroup > USER_GROUP_DEFAULT){
            ugroup = USER_GROUP_DEFAULT;
        }
        setUserGroup(userPtr,ugroup);
    }
    setNetMessageError(ptr,RESPONSE_OK,buildErrorCode(AQ_SUCCESS_MARK,AQ_SERVER_SET_USER_SUCCESS),AQ_SERVER_SET_USER_SUCCESS_MSG,user);
    freeString(&user);
    freeString(&passwd);
    freeString(&group);
    freeString(&privilege);    
}
static int aq_server_list_users(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr){
    ListNodePtr start = getListHeader(serverPtr->usersList);
    ListNodePtr end = getListEnd(serverPtr->usersList);
    char* buf= allocMem(USER_LIST_BUF_SIZE);
    int writedLength = 0,leavLength =0,write;
    UserPtr userptr = nextFromList(&start,end,NULL,NULL);
    while(userptr!= NULL){

        if(writedLength<USER_LIST_BUF_SIZE){
            write = snprintf(buf+writedLength,USER_LIST_BUF_SIZE-writedLength,"ID:%d %s\n",userptr->userId,userptr->userName);
            writedLength += write;
        }else{
            break;
        }
        userptr = nextFromList(&start,end,NULL,NULL);
    }
    if(writedLength == 0){
        char* string = " no user saved in server!";
        strcpy(buf,string);
        writedLength = strlen(string);
    }
    setNetMessageJustSendData(ptr,buf,writedLength);
    setNetMessageSendCMDData(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_GET_TOPIC_LIST_SUCCESS),RESPONSE_OK);

}
static int aq_server_close(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr)
{
    stopAppServer(serverPtr);
    exit(0);
}