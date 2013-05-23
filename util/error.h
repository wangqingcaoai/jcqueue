#ifndef UTIL_ERROR
#define UTIL_ERROR 
#define ERROR_SPLIT_WIDTH 100
int buildErrorCode(int errorType,int errorNum);
int getErrorType(int errorCode);
int getErrorNum(int errorCode);
#endif