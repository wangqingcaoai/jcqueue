#ifndef PARSER_MESSAGE
#define PARSER_MESSAGE 
#include "../util/type.h"
#define NETMESSAGE_READSTATE_WAIT 1
#define NETMESSAGE_READSTATE_PARAM 2
#define NETMESSAGE_READSTATE_DATA 3
#define NETMESSAGE_READSTATE_FINISH 4

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
    char* entraParam ;
    void* data;
    int length;
    void* lastParserBuf;
    int lastParserBufLength;
	int readLength;
	int readState;// wait param data finish 
	int delay;//
    int priority;
    char* clientVersion;//
    char* clientType;//
    int64 timestamp;//
    int errcode;//
    char* clientCmd;//
    char* clientTarget;//
    char* sendExtraParam;//
    void * sendData;//
    int sendLength; //
	int sendState;//
	void* sendBuf;
    char* clientKey;//
    char* clientUser;//
    char* clientPassword;//
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
 * preiority:
 * timestamp:
 * extraParam:param1[value]param2[value2]
 * length:
 * data:eerttyytwwerttewww
 * 
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
char* NM_getCmd(NetMessagePtr);
char* NM_getTarget(NetMessagePtr);
int NM_setSendData(NetMessagePtr ptr,int errcode,char* clientCmd,char* clientTarget,char* sendData,int datalength);
int NM_setSendUser(NetMessagePtr ptr,int clientUser,int clientKey,int clientPassword);
NetMessagePtr NM_buildNetMessage();
int NM_setError(NetMessagePtr ptr,int errcode,char* errorMessage,...);
char * getExtraParam(NetMessagePtr ptr,const char* paramName);
int setExtraParam(NetMessagePtr ptr,const char* paramName,const char* paramValue);

char * getSendExtraParam(NetMessagePtr ptr,const char* paramName);
int setSendExtraParam(NetMessagePtr ptr,const char* paramName,const char* paramValue);
int setParam(NetMessagePtr ptr,const char* paramName,const char* value);
#endif
