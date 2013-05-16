#ifndef UTIL_ERRORS
#define UTIL_ERRORS 
int buildErrorCode(int errorType,int errorNum);
int getErrorType(int errorCode);
int getErrorNum(int errorCode);
#endif