#ifndef PARSER_JCQ_H
#define PARSER_JCQ_H
#include "net_message.h"

typedef struct Param
{
    char* paramName;
    char* paramData;
    int readLength;
}Param,*ParamPtr;
int parserJCQMessage(NetMessagePtr ptr,void * buf, int length);
int reparserJCQMessage(NetMessagePtr ptr,char * protocolType, char* version);
int isJCQMessage(NetMessagePtr ptr,void* buf,int length);
int getJCQParamNameAndData(char* ptr,ParamPtr param, int length);
int isJCQMessageData(char * buf,int length);
int isJCQMessageEnd(char* buf,int length);
int setLastParamBuffer(NetMessagePtr ptr,void * buf,int length);
int isJCQMessage(NetMessagePtr ptr,void* buf,int length);
#endif

