#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
int buildErrorCode(int errorType,int errorNum){
    return errorType*ERROR_SPLIT_WIDTH +errorNum;
}
int getErrorType(int errorCode){
    return errorCode/ERROR_SPLIT_WIDTH;
}
int getErrorNum(int errorCode){
    return errorCode%ERROR_SPLIT_WIDTH;
}