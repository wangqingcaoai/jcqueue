#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "topic_router.h"
#include "router.h"
#include "../app_queue/server.h"
#include "../errors.h"


int bq_router(BaseServerPtr serverPtr,NetMessagePtr ptr){
    if(serverPtr == NULL){
        bq_empty_baseServer(serverPtr,ptr);
    }
    if(ptr == NULL){
        return -1;
    }
    if(ptr->cmd == NULL){
        return bq_unknow_cmd(serverPtr, ptr);
    }
    int result = 0;
    if(strcasecmp(ptr->cmd,BQ_ADD)){
        result =  bq_add(serverPtr , ptr);
        
    }else if(strcasecmp(ptr->cmd,BQ_GET)){
        result = bq_get(serverPtr, ptr);
    }else if(strcasecmp(ptr->cmd,BQ_SLEEP)){
        result = bq_sleep(serverPtr,ptr);
    }else if(strcasecmp(ptr->cmd,BQ_REUSE)){
        result = bq_reuse(serverPtr,ptr);
    }else if(strcasecmp(ptr->cmd,BQ_DEL)){
        result = bq_del(serverPtr,ptr);
    }else if(strcasecmp(ptr->cmd,BQ_WAKEUP)){
        result = bq_wakeup(serverPtr,ptr);
    }else if(strcasecmp(ptr->cmd,BQ_ADD_TOPIC)){
        result = bq_add_topic(serverPtr,ptr);
    }else if(strcasecmp(ptr->cmd,BQ_DEL_TOPIC)){
        result = bq_del_topic(serverPtr,ptr);
    }else if(strcasecmp(ptr->cmd,BQ_CLEAN_TOPIC)){
        result = bq_clean_topic(serverPtr,ptr);
    }else{
        result = bq_unknow_cmd(serverPtr, ptr);
    }
    return result;

}
static int bq_empty_baseServer(BaseServerPtr serverPtr,NetMessagePtr ptr){
    return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_BASESERVER),BQ_EMPTY_BASESERVER_MSG);

}

static int bq_unknow_cmd(BaseServerPtr serverPtr,NetMessagePtr ptr){
    return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_CMD),BQ_UNKNOW_CMD_MSG,ptr->cmd);
}

static int bq_add(BaseServerPtr serverPtr,NetMessagePtr ptr){
    char * param = getExtraParam(ptr,"topic_name");
    if(param == NULL){
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_TOPIC_NAME),BQ_EMPTY_TOPIC_NAME_MSG);
    }
    TopicPtr tPtr = useTopic(serverPtr,param);

    if(tPtr == NULL){
        freeString(&param);
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_TOPIC),BQ_UNKNOW_TOPIC_MSG,param);
    }else{
        freeString(&param);
          param = getExtraParam(ptr,"delay");
        int delay  = atoi(param);
        freeString(&param);
        param = getExtraParam(ptr,"priority");
        int priority  = atoi(param);
        freeString(&param);
        MessagePtr mPtr = buildMessage(delay==0?MS_READY:MS_DELAY,ptr->sendTime,priority,ptr->data,ptr->length,delay );
        if(mPtr == NULL){
            return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_BUILD_MESSAGE_FAILED),BQ_BUILD_MESSAGE_FAILED_MSG);
        }
        
        addMessage(tPtr,mPtr,delay);
        return setNetMessageError(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_ADD_SUCCESS),BQ_ADD_SUCCESS_MSG);
    }

}
static int bq_get(BaseServerPtr serverPtr,NetMessagePtr ptr){
    char * param = getExtraParam(ptr,"topic_name");
    if(param == NULL){
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_TOPIC_NAME),BQ_EMPTY_TOPIC_NAME_MSG);
    }
    TopicPtr tPtr = useTopic(serverPtr,param);

    if(tPtr == NULL){
        freeString(&param);
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_TOPIC),BQ_UNKNOW_TOPIC_MSG,param);
    }else{
        MessagePtr mPtr = getReadyMessage(tPtr);
        if(mPtr == NULL){
            setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_NO_READY_MESSAGE),BQ_NO_READY_MESSAGE_MSG,param);
        
            freeString(&param);
            return 0;
        }else{
            //TODO client cmd undefined now 
            setNetMessageSendData(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_GET_SUCCESS),BQ_CLIENT_OK,mPtr->data,mPtr->length);
            char buf[UTIL_NUM_BUF_SIZE];
            int size = UTIL_NUM_BUF_SIZE;
            setSendExtraParam(ptr,"message_id",int64ToString(mPtr->messageid,buf,size));
            
            setSendExtraParam(ptr,"priority",intToString(mPtr->priority,buf,size));

            setSendExtraParam(ptr,"delay",intToString(mPtr->delay,buf,size));

            setSendExtraParam(ptr,"activetime",int64ToString(mPtr->activetime,buf,size));

            setSendExtraParam(ptr,"timestamp",int64ToString(mPtr->timestamp,buf,size));
            freeString(&param);
            return 0;
        }
    }
}

static int bq_sleep(BaseServerPtr serverPtr,NetMessagePtr ptr){
    char * param = getExtraParam(ptr,"topic_name");
    if(param == NULL){
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_TOPIC_NAME),BQ_EMPTY_TOPIC_NAME_MSG);
    }
    TopicPtr tPtr = useTopic(serverPtr,param);

    if(tPtr == NULL){
        freeString(&param);
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_TOPIC),BQ_UNKNOW_TOPIC_MSG,param);
    }else{
        freeString(&param);
         param = getExtraParam(ptr,"message_id");
        if(param == NULL){
            return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_MESSAGE_ID),BQ_EMPTY_MESSAGE_ID_MSG);
        }
        int64 messageId  = atoll(param);
        freeString(&param);
        int code  = sleepMessage(tPtr,messageId);
        if(code == -1){
            return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_MSG),BQ_UNKNOW_MSG_MSG,messageId);
        }else{
            return setNetMessageError(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_SLEEP_SUCCESS),BQ_SLEEP_SUCCESS_MSG, messageId);
        }
    }
} 

static int bq_reuse(BaseServerPtr serverPtr,NetMessagePtr ptr){
    char * param = getExtraParam(ptr,"topic_name");
    if(param == NULL){
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_TOPIC_NAME),BQ_EMPTY_TOPIC_NAME_MSG);
    }
    TopicPtr tPtr = useTopic(serverPtr,param);

    if(tPtr == NULL){
        freeString(&param);
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_TOPIC),BQ_UNKNOW_TOPIC_MSG,param);
    }else{
        freeString(&param);
          param = getExtraParam(ptr,"message_id");
        if(param == NULL){
            return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_MESSAGE_ID),BQ_EMPTY_MESSAGE_ID_MSG);
        }
        int64 messageId  = atoll(param);
        freeString(&param);
        param = getExtraParam(ptr,"delay");
        int delay  = atoi(param);
        freeString(&param);
        int code = reuseMessage(tPtr,messageId,delay);
        if(code == -1){
            return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_MSG),BQ_UNKNOW_MSG_MSG,messageId);
        }else{
            return setNetMessageError(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_REUSE_SUCCESS),BQ_REUSE_SUCCESS_MSG, messageId);
        }
    }
} 

static int bq_del(BaseServerPtr serverPtr,NetMessagePtr ptr){
    char * param = getExtraParam(ptr,"topic_name");
    if(param == NULL){
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_TOPIC_NAME),BQ_EMPTY_TOPIC_NAME_MSG);
    }
    TopicPtr tPtr = useTopic(serverPtr,param);

    if(tPtr == NULL){
        freeString(&param);
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_TOPIC),BQ_UNKNOW_TOPIC_MSG,param);
    }else{
        freeString(&param);
          param = getExtraParam(ptr,"message_id");
        if(param == NULL){
            return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_MESSAGE_ID),BQ_EMPTY_MESSAGE_ID_MSG);
        }
        int64 messageId  = atoll(param);
        freeString(&param);
        int code = delMessage(tPtr,messageId);
        if(code == -1){
            return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_MSG),BQ_UNKNOW_MSG_MSG,messageId);
        }else{
            return setNetMessageError(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_DEL_SUCCESS),BQ_DEL_SUCCESS_MSG, messageId);
        }
    }
} 

static int bq_wakeup(BaseServerPtr serverPtr,NetMessagePtr ptr){
    char * param = getExtraParam(ptr,"topic_name");
    if(param == NULL){
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_TOPIC_NAME),BQ_EMPTY_TOPIC_NAME_MSG);
    }
    TopicPtr tPtr = useTopic(serverPtr,param);

    if(tPtr == NULL){
        freeString(&param);
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_TOPIC),BQ_UNKNOW_TOPIC_MSG,param);
    }else{
        freeString(&param);
          param = getExtraParam(ptr,"message_id");
        if(param == NULL){
            return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_MESSAGE_ID),BQ_EMPTY_MESSAGE_ID_MSG);
        }
        int64 messageId  = atoll(param);
        freeString(&param);
        param = getExtraParam(ptr,"delay");
        int delay = atoi(param);
        freeString(&param);
        int code = wakeUpMessage(tPtr,messageId,delay);
        if(code == -1){
            return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_MSG),BQ_UNKNOW_MSG_MSG,messageId);
        }else{
            return setNetMessageError(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_WAKEUP_SUCCESS),BQ_WAKEUP_SUCCESS_MSG, messageId);
        }
    }
} 

static int bq_add_topic(BaseServerPtr serverPtr,NetMessagePtr ptr){

    char * param = getExtraParam(ptr,"topic_name");
    printf("%s\n",param );
    if(param == NULL){
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_TOPIC_NAME),BQ_EMPTY_TOPIC_NAME_MSG);
    }

    int code = addTopic(serverPtr,param);
    
    if(code == -1){
        int result = setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EXISTS_TOPIC_NAME),BQ_EXISTS_TOPIC_NAME_MSG,param);
        freeString(&param);
        return result;
    }else{
        UpdateSubscribeAfterAddTopic(serverPtr->appServer->subscribeServer,param);
        int result =  setNetMessageError(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_ADD_TOPIC_SUCCESS),BQ_ADD_TOPIC_SUCCESS_MSG, param);
        freeString(&param);
        return result;
    }
    
} 
static int bq_del_topic(BaseServerPtr serverPtr,NetMessagePtr ptr){
    char * param = getExtraParam(ptr,"topic_name");
    if(param == NULL){
        return setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_TOPIC_NAME),BQ_EMPTY_TOPIC_NAME_MSG);
    }
    int code = removeTopic(serverPtr,param);
    
    if(code == -1){
        int result = setNetMessageError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_ERROR),BQ_UNKNOW_ERROR_MSG);
        freeString(&param);
        return result;
    }else{
        UpdateSubscribeAfterRemoveTopic(serverPtr->appServer->subscribeServer,param);
        int result =  setNetMessageError(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_DEL_TOPIC_SUCCESS),BQ_DEL_TOPIC_SUCCESS_MSG, param,code);
        freeString(&param);
        return result;
    }
}
static int bq_clean_topic(BaseServerPtr serverPtr,NetMessagePtr ptr){

} 




