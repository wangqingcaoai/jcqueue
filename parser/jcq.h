#ifndef PARSER_JCQ_H
#define PARSER_JCQ_H
#include "net_message.h"
#define PARSER_SUCCESS 0
#define PARSER_ERROR_PARAM_ERROR 1
#define PARSER_ERROR_DATA_OUT_LINE 2
#define PARSER_ERROR_DATA_NEED_MORE 3
typedef struct Param
{
    char* paramName;
    char* paramData;
    int readLength;
}Param,*ParamPtr;
int parserJCQMessage(NetMessagePtr ptr,void * buf, int length);
int buildJCQMessage(NetMessagePtr ptr,void * buf, int length);
#endif

