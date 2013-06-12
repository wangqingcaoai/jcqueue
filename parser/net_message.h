#ifndef PARSER_MESSAGE
#define PARSER_MESSAGE 
#include "../util/type.h"
#include "../app_queue/user.h"
#define NETMESSAGE_READSTATE_WAIT 1
#define NETMESSAGE_READSTATE_PARAM 2
#define NETMESSAGE_READSTATE_DATA 3
#define NETMESSAGE_READSTATE_END 4
#define NETMESSAGE_READSTATE_FINISH 5

#define NETMESSAGE_WRITESTATE_WAIT 1
#define NETMESSAGE_WRITESTATE_WRITING 2
#define NETMESSAGE_WRITESTATE_FINISH 3
#define NETMESSAGE_SUCCESS 0
#define NETMESSAGE_ERROR_PARAM_ERROR 1

#define NETMESSAGE_ERROR_UNKNOW_PARAM 2
#define NETMESSAGE_ERROR_PARAM_VALUE_FORMAT_ERROR 3
#define NETMESSAGE_DEFAULT_ERROR_BUF_SIZE 512

#define NETMESSAGE_DEFAULT_SEND_BUF_SIZE 512
#define NETMESSAGE_DEFAULT_PARAM_BUF_SIZE 30
#define NETMESSAGE_DEFAULT_PARAM_VALUE_BUF_SIZE 120
#define NETMESSAGE_TYPE_JCQ "jcq"
#define NETMESSAGE_TYPE_CMD "cmd"
#define NETMESSAGE_TYPE_HTTP "http"
#define NETMESSAGE_VERSION_CMD "1.0"
#define NETMESSAGE_VERSION_JCQ "1.0"
#define NETMESSAGE_VERSION_HTTP "1.1"
typedef struct User * UserPtr;
typedef struct Param
{
    char* paramName;
    char* paramData;
    int readLength;
}Param,*ParamPtr;
/**
 * 
 */
typedef struct NetMessage
{
    int64 id;
    //recv
    char* cmd ;
    char* user;
    char* password;
    char* key;
    char* extraParam ;
    void* data;
    int length;
    int64 timestamp;//
    int errcode;//

    char* version;
    char* protocolType;
    
    //not need send
    char* host ;
    char* port ;
    void* lastParserBuf;
    int lastParserBufLength;
    int readLength;
    int readState;// wait param data finish 
    //response
    
    char* sendCmd;//
    char* currentUserKey;//
    char* currentUser;//
    char* currentPassword;//
    char* sendExtraParam;//
    void * sendData;//
    int sendLength; //
    int64 sendTime;//
    int sendErrcode;

    //no need send
    int sendState;//
    void* sendBuf;
    int sendBufLength;
    int writedLength;
}NetMessage,* NetMessagePtr;
/*
 *base format
 * jcq 1.0 
 * cmd:
 * target:
 * targetType:
 * user:
 * password:
 * key:
 * delay:
 * priority:
 * timestamp:
 * extraParam:param1[value]param2[value2]
 * length:
 * data:eerttyytwwerttewww\r\n\r\n 
 *
 *
 *
 *
 *
 *
 *
 * */
 #define JCQ_FORMAT  "%s %s\r\n"\
    "cmd:%s\r\n"\
    "user:%s\r\n"\
    "password:%s\r\n"\
    "key:%s\r\n"\
    "errcode:%d\r\n"\
    "timestamp:%" PRId64 "\r\n"\
    "extraParam:%s\r\n"\
    "length:%d\r\n"\
    "data:"
#define JCQ_FORMAT_END "\r\n\r\n"

/*
 *http format
 *http 
 *
 *
 *
 *
 *
 * */
 /**
  * cmd format
  * cmd target targetType param1[value1]param2[] data:dfdfdf\n
  */
#define CMD_READ_FORMAT "%s %s %s\n"
#define CMD_WRITE_FORMAT  "%d "
int setNetMessageSendData(NetMessagePtr ptr,int errcode,char* sendCmd,void* sendData,int datalength);
int setNetMessageSendUser(NetMessagePtr ptr,UserPtr uptr);
NetMessagePtr buildNetMessage();
int setNetMessageError(NetMessagePtr ptr,char* cmd,int errcode,char* errorMessage,...);
char * getExtraParam(NetMessagePtr ptr,const char* paramName);
int setExtraParam(NetMessagePtr ptr,const char* paramName,const char* paramValue);

char * getSendExtraParam(NetMessagePtr ptr,const char* paramName);
int setSendExtraParam(NetMessagePtr ptr,const char* paramName,const char* paramValue);
int setNetMessageParam(NetMessagePtr ptr,const char* paramName,const char* value);
int displayNetMessage(NetMessagePtr ptr);
static char* findParamValueByString(const char* string,const char* paramName );
static int setParamByValue(char** pptr,const char* paramName,const char* paramValue);
int freeNetMessage(NetMessagePtr* pptr);
int isExtraParamFormatRight(const char*buf,int length);
int setNetMessageJustData(NetMessagePtr ptr,void *data,int length);
int setNetMessageSendCMDData(NetMessagePtr ptr,int errcode,char* sendCmd);
int setNetMessageSendState(NetMessagePtr ptr,int state);
int getNetMessageSendState(NetMessagePtr ptr);
int setNetMessageReadState(NetMessagePtr ptr,int state);
int getNetMessageReadState(NetMessagePtr ptr);

#endif
