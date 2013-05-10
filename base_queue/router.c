#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "topic_router.h"
#include "router.h"
#include "errors.h"


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
        result = bg_sleep(serverPtr,ptr);
    }else if(strcasecmp(ptr->cmd,BQ_REUSE)){
        result = bg_reuse(serverPtr,ptr);
    }else if(strcasecmp(ptr->cmd,BQ_DEL)){
        result = bg_del(serverPtr,ptr);
    }else if(strcasecmp(ptr->cmd,BQ_WAKEUP)){
        result = bg_wakeup(serverPtr,ptr);
    }else if(strcasecmp(ptr->cmd,BQ_ADD_TOPIC)){
        result = bg_add_topic(serverPtr,ptr);
    }else if(strcasecmp(ptr->cmd,BQ_DEL_TOPIC)){
        result = bg_del_topic(serverPtr,ptr);
    }else if(strcasecmp(ptr->cmd,BQ_CLEAN_TOPIC)){
        result = bg_clean_topic(serverPtr,ptr);
    }else{
        result = bq_unknow_cmd(serverPtr, ptr);
    }
    return result;

}
static int bq_empty_baseServer(BaseServerPtr serverPtr,NetMessagePtr ptr){
    return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_BASESERVER),BQ_EMPTY_BASESERVER_MSG);

}

static int bq_unknow_cmd(BaseServerPtr serverPtr,NetMessagePtr ptr){
    return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_CMD),BQ_UNKNOW_CMD_MSG,ptr->cmd);
}

static int bq_add(BaseServerPtr serverPtr,NetMessagePtr ptr){
    TopicPtr tPtr = useTopic(serverPtr,ptr->target);
    if(tPtr == NULL){
        return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_TOPIC),BQ_UNKNOW_TOPIC_MSG,ptr->target);
    }else{

        MessagePtr mPtr = buildMessage(ptr->delay!=0?MS_READY:MS_DELAY,ptr->sendTime,ptr->priority,ptr->data,ptr->length,ptr->delay );
        if(mPtr == NULL){
            return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_BUILD_MESSAGE_FAILED),BQ_BUILD_MESSAGE_FAILED_MSG);
        }
        addMessage(tPtr,mPtr,ptr->delay);
        return NM_setError(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_ADD_SUCCESS),BQ_ADD_SUCCESS_MSG);
    }

}
static int bq_get(BaseServerPtr serverPtr,NetMessagePtr ptr){
    TopicPtr tPtr = useTopic(serverPtr,ptr->target);
    if(tPtr == NULL){
        return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_TOPIC),BQ_UNKNOW_TOPIC_MSG,ptr->target);
    }else{
        MessagePtr mPtr = getReadyMessage(tPtr);
        if(mPtr == NULL){
            return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_NO_READY_MESSAGE),BQ_NO_READY_MESSAGE_MSG,ptr->target);
        }else{
            //TODO client cmd undefined now 
            return NM_setSendData(ptr,BQ_SUCCESS_MARK,"","",mPtr->data,mPtr->length);
        }
    }
}

static int bq_sleep(BaseServerPtr serverPtr,NetMessagePtr ptr){
    TopicPtr tPtr = useTopic(serverPtr,ptr->target);
    if(tPtr == NULL){
        return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_TOPIC),BQ_UNKNOW_TOPIC_MSG,ptr->target);
    }else{
        char * param = getEntraParam(ptr,'message_id');
        if(param == NULL){
            return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_EMPTY_MESSAGE_ID),BQ_EMPTY_MESSAGE_ID_MSG);
        }
        int messageId  = atoi(param);
        int code  = sleepMessage(tPtr,messageId);
        if(code == -1){
            return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_MSG),BQ_UNKNOW_MSG_MSG,messageId);
        }else{
            return NM_setError(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_SLEEP_SUCCESS),BQ_SLEEP_SUCCESS_MSG, messageId);
        }
    }
} 

static int bq_reuse(BaseServerPtr serverPtr,NetMessagePtr ptr){
    TopicPtr tPtr = useTopic(serverPtr,ptr->target);
    if(tPtr == NULL){
        return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_TOPIC),BQ_UNKNOW_TOPIC_MSG,ptr->target);
    }else{
        char * param = getEntraParam(ptr,'message_id');
        int messageId  = atoi(param);
        int code = reuseMessage(tPtr,messageId,ptr->delay);
        if(code == -1){
            return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_MSG),BQ_UNKNOW_MSG_MSG,messageId);
        }else{
            return NM_setError(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_REUSE_SUCCESS),BQ_REUSE_SUCCESS_MSG, messageId);
        }
    }
} 

static int bq_del(BaseServerPtr serverPtr,NetMessagePtr ptr){
    TopicPtr tPtr = useTopic(serverPtr,ptr->target);
    if(tPtr == NULL){
        return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_TOPIC),BQ_UNKNOW_TOPIC_MSG,ptr->target);
    }else{
        char * param = getEntraParam(ptr,'message_id');
        int messageId  = atoi(param);
        int code = delMessage(tPtr,messageId);
        if(code == -1){
            return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_MSG),BQ_UNKNOW_MSG_MSG,messageId);
        }else{
            return NM_setError(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_DEL_SUCCESS),BQ_DEL_SUCCESS_MSG, messageId);
        }
    }
} 

static int bq_wakeup(BaseServerPtr serverPtr,NetMessagePtr ptr){
    TopicPtr tPtr = useTopic(serverPtr,ptr->target);
    if(tPtr == NULL){
        return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_TOPIC),BQ_UNKNOW_TOPIC_MSG,ptr->target);
    }else{
        char * param = getEntraParam(ptr,'message_id');
        int messageId  = atoi(param);
        int code = wakeUpMessage(tPtr,messageId,ptr->delay);
        if(code == -1){
            return NM_setError(ptr,buildErrorCode(BQ_ERRORS_MARK,BQ_UNKNOW_MSG),BQ_UNKNOW_MSG_MSG,messageId);
        }else{
            return NM_setError(ptr,buildErrorCode(BQ_SUCCESS_MARK,BQ_WAKEUP_SUCCESS),BQ_WAKEUP_SUCCESS_MSG, messageId);
        }
    }
} 

static int bq_add_topic(BaseServerPtr serverPtr,NetMessagePtr ptr){


} 
static int bq_del_topic(BaseServerPtr serverPtr,NetMessagePtr ptr){

}
static int bq_clean_topic(BaseServerPtr serverPtr,NetMessagePtr ptr){

} 




