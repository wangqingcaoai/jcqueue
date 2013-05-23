#ifndef BQ_ERRORS_H
#define BQ_ERRORS_H 
#define BQ_ERRORS_MARK 10

#define BQ_SUCCESS_MARK 20


#define BQ_EMPTY_BASESERVER 1
#define BQ_EMPTY_BASESERVER_MSG "base server is empty ,please check it!"

#define BQ_UNKNOW_CMD 2
#define BQ_UNKNOW_CMD_MSG  "unknow command %s,please check it!"

#define BQ_UNKNOW_TARGET 3
#define BQ_UNKNOW_TARGET_MSG  "unknow target %s,please check it!"

#define BQ_UNKNOW_TOPIC 4
#define BQ_UNKNOW_TOPIC_MSG  "unknow topic %s,please check it!"

#define BQ_BUILD_MESSAGE_FAILED 5
#define BQ_BUILD_MESSAGE_FAILED_MSG  "bulid message failed ,please check it!"

#define BQ_NO_READY_MESSAGE 6
#define BQ_NO_READY_MESSAGE_MSG "don't have ready message"
#define BQ_UNKNOW_MSG 7
#define BQ_UNKNOW_MSG_MSG "message %d not found "

#define BQ_EMPTY_MESSAGE_ID 8 

#define BQ_EMPTY_MESSAGE_ID_MSG "message id not give "

#define BQ_EMPTY_TOPIC_NAME 9 

#define BQ_EMPTY_TOPIC_NAME_MSG "topicname not give "

#define BQ_EXISTS_TOPIC_NAME 9 

#define BQ_EXISTS_TOPIC_NAME_MSG "topicname %s is exists! "

#define BQ_UNKNOW_ERROR 10
#define BQ_UNKNOW_ERROR_MSG "unknow error happened! please contant us!"

#define BQ_ADD_SUCCESS 1
#define BQ_ADD_SUCCESS_MSG "add success"

#define BQ_GET_SUCCESS 2
#define BQ_GET_SUCCESS_MSG ""

#define BQ_SLEEP_SUCCESS 3
#define BQ_SLEEP_SUCCESS_MSG "sleep message %" PRId64 " success "

#define BQ_REUSE_SUCCESS 4
#define BQ_REUSE_SUCCESS_MSG "reuse message %" PRId64 " success "

#define BQ_DEL_SUCCESS 5
#define BQ_DEL_SUCCESS_MSG "del message %" PRId64 " success "


#define BQ_WAKEUP_SUCCESS 6
#define BQ_WAKEUP_SUCCESS_MSG "wakeup message %" PRId64 " success "

#define BQ_ADD_TOPIC_SUCCESS 7
#define BQ_ADD_TOPIC_SUCCESS_MSG "add topic %s success "


#define BQ_DEL_TOPIC_SUCCESS 8
#define BQ_DEL_TOPIC_SUCCESS_MSG "del topic %s success, remove count %d "

#endif