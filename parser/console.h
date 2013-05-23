#ifndef PARSER_CONSOLE_H
#define PARSER_CONSOLE_H 
#define CONSOLE_BUF_SIZE 256
#define CONSOLE_BUF_CMD_SIZE 15
#define CONSOLE_BUF_TARGET_SIZE 50
#define CONSOLE_BUF_TARGETTYPE_SIZE 50
#define CONSOLE_BUF_EXTRAPARAM_SIZE 256
#define CONSOLE_BUF_DATA_SIZE 512
int parserCMDMessage(NetMessagePtr ptr,void * buf, int length);
int reparserCMDMessage(NetMessagePtr ptr,char * protocolType, char* version);
int isCMDMessage(NetMessagePtr ptr,void* buf,int length);
int getCMDParamNameAndData(char* ptr,ParamPtr param, int length);
int isCMDMessageData(char * buf,int length);
int isCMDMessageEnd(char* buf,int length);
#endif