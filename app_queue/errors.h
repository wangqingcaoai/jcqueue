#ifndef AQ_ERRORS_H
#define AQ_ERRORS_H 
#define AQ_ERRORS_MARK 1
#define AQ_SUCCESS_MARK 2
#define AQ_ERROR_USER_NOT_FOUND 1
#define AQ_ERROR_USER_NOT_FOUND_MSG "user %s not found"

#define AQ_ERROR_USER_ERROR_SECURE_FAILED  2
#define AQ_ERROR_USER_ERROR_SECURE_FAILED_MSG " user check failed please check passwd and securekey!"

#define AQ_ERROR_SUBSCRIBE_NOT_FOUND 3
#define AQ_ERROR_SUBSCRIBE_NOT_FOUND_MSG "subscribe  not found"

#define AQ_ERROR_USER_NOT_SEND 4
#define AQ_ERROR_USER_NOT_SEND_MSG "user  not give in message"

#define AQ_CLIENT_ERROR_SERVER_ADD_FAILED 5
#define AQ_CLIENT_ERROR_SERVER_ADD_FAILED_MSG " add server (%s:%s) failed!"


#define AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR 6
#define AQ_CLIENT_ERROR_SERVER_ADD_PARAM_ERROR_MSG "  param error %s:%s"


#define AQ_CLIENT_ERROR_SERVER_NOT_FOUND 7
#define AQ_CLIENT_ERROR_SERVER_NOT_FOUND_MSG "  server (%d) not found"

#define AQ_CLIENT_ERROR_SERVER_DEL_PARAM_EMPTY 8
#define AQ_CLIENT_ERROR_SERVER_DEL_PARAM_EMPTY_MSG "  server id not give"


#define AQ_CLIENT_ERROR_SERVER_USE_PARAM_EMPTY 9
#define AQ_CLIENT_ERROR_SERVER_USE_PARAM_EMPTY_MSG "  server id not give"

#define AQ_SERVER_ERROR_SERVER_USER_PARAM_EMPTY 10
#define AQ_SERVER_ERROR_SERVER_USER_PARAM_EMPTY_MSG "  user param failed format is user[username]password[qweqwe]group[1]privilege[1]"

#define AQ_SERVER_ERROR_SERVER_USER_EXISTS 11
#define AQ_SERVER_ERROR_SERVER_USER_EXISTS_MSG "  user %s is exists!"

#define AQ_CLIENT_TO_SERVER_FAILED_NO_SERVER_USING 12
#define AQ_CLIENT_TO_SERVER_FAILED_NO_SERVER_USING_MSG " no server using current ,please add a new server to connect!"

#define AQ_ADD_SUBSCRIBE_SUCCESS 1
#define AQ_ADD_SUBSCRIBE_SUCCESS_MSG "add subscribe suceess!"
#define AQ_DEL_SUBSCRIBE_SUCCESS 2
#define AQ_DEL_SUBSCRIBE_SUCCESS_MSG "del subscribe  success!"

#define AQ_CLIENT_TO_SERVER_SUCCESS 3
#define AQ_CLIENT_TO_SERVER_SUCCESS_MSG "cmd has send to  server(%s:%s) request queue!"

#define AQ_CLIENT_SERVER_ADD_SUCCESS 4
#define AQ_CLIENT_SERVER_ADD_SUCCESS_MSG "add  server(%s:%s) success!"

#define AQ_CLIENT_SERVER_LIST_SUCCESS 5

#define AQ_CLIENT_SERVER_DEL_SUCCESS 6
#define AQ_CLIENT_SERVER_DEL_SUCCESS_MSG "del server(%d) success!"

#define AQ_CLIENT_SERVER_USE_SUCCESS 7
#define AQ_CLIENT_SERVER_USE_SUCCESS_MSG "use server(%d) success!"

#define AQ_SERVER_ADD_USER_SUCCESS 8
#define AQ_SERVER_ADD_USER_SUCCESS_MSG "add user %s success!"



#define PUSH_SUCCESS_MARK 3
#define PUSH_PUSH_SUCCESS 1
#endif