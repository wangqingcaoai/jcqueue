#ifndef BQ_ERRORS_H
#define BQ_ERRORS_H 
#define BQ_ERRORS_MARK 0x11

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

#define BQ_SUCCESS_MARK 0x12
#define BQ_ADD_SUCCESS 1
#define BQ_ADD_SUCCESS_MSG "add success"

#define BQ_SLEEP_SUCCESS 2
#define BQ_SLEEP_SUCCESS_MSG "sleep message %d success "

#define BQ_REUSE_SUCCESS 3
#define BQ_REUSE_SUCCESS_MSG "reuse message %d success "

#define BQ_DEL_SUCCESS 4
#define BQ_DEL_SUCCESS_MSG "del message %d success "


#define BQ_WAKEUP_SUCCESS 5
#define BQ_WAKEUP_SUCCESS_MSG "wakeup message %d success "

#endif