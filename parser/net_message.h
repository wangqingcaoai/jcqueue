#ifndef PARSER_MESSAGE
#define PARSER_MESSAGE 
#include "../util/type.h"
#define NETMESSAGE_READSTATE_WAIT 1
#define NETMESSAGE_READSTATE_PARAM 2
#define NETMESSAGE_READSTATE_DATA 3
#define NETMESSAGE_READSTATE_FINISH 4

#define NETMESSAGE_WRITESTATE_WAIT 1
#define NETMESSAGE_WRITESTATE_FINISH 2
#define NETMESSAGE_SUCCESS 0
#define NETMESSAGE_ERROR_PARAM_ERROR 1

#define NETMESSAGE_ERROR_UNKNOW_PARAM 2
/**
 * 
 */
typedef struct NetMessage
{
    char* cmd ;
    char* target;
    char* targetType;
    char* host ;
    char* port ;
    char* user;
    char* password;
    char* key;
    char* extraParam ;
    void* data;
    int length;
    void* lastParserBuf;
    int lastParserBufLength;
	int readLength;
	int readState;// wait param data finish 
	int delay;//
    int priority;
    char* version;
    char* protocolType;
    int64 timestamp;//
    int errcode;//
    char* clientCmd;//
    char* clientTarget;//
    char* sendExtraParam;//
    void * sendData;//
    int sendLength; //
	int sendState;//
	void* sendBuf;
    int writedLength;
    char* currentUserKey;//
    char* currentUser;//
    char* currentPassword;//
    int64 sendTime;//
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
/*
 *http format
 *http 
 *
 *
 *
 *
 *
 * */
char* getCmd(NetMessagePtr);
char* getTarget(NetMessagePtr);
int setSendData(NetMessagePtr ptr,int errcode,char* clientCmd,char* clientTarget,char* sendData,int datalength);
int setSendUser(NetMessagePtr ptr,int clientUser,int clientKey,int clientPassword);
NetMessagePtr buildNetMessage();
int setError(NetMessagePtr ptr,int errcode,char* errorMessage,...);
char * getExtraParam(NetMessagePtr ptr,const char* paramName);
int setExtraParam(NetMessagePtr ptr,const char* paramName,const char* paramValue);

char * getSendExtraParam(NetMessagePtr ptr,const char* paramName);
int setSendExtraParam(NetMessagePtr ptr,const char* paramName,const char* paramValue);
int setNetMessageParam(NetMessagePtr ptr,const char* paramName,const char* value);
int displayNetMessage(NetMessagePtr ptr);
#endif
