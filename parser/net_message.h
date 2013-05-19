#ifndef PARSER_MESSAGE
#define PARSER_MESSAGE 
/**
 * 对传递的消息定义
 */
typedef struct NetMessage
{
    char* cmd ;//命令名
    char* target;//操作的目标
    char* targetType;
    char* host ;//来�m�主机
    char* port ;//端口
    char* user;//来源用户
    char* password;//用户密码
    char* key;//用户校验key
    char* entraParam ;//额外参数
    void* data;//消息数据
    int length;//消息长度
    void* lastParserBuf;
	int readLength;
	int readState;// wait param data finish 
	int delay;//消息延迟
    int priority;
    char* clientVersion;//客户端版本
    char* clientType;//客户端类型
    int64 timestamp;//创建时间
    int errcode;//响应码
    char* clientCmd;//响应命令 如处理完反馈等
    char* clientTarget;//响应对应的对象
    char* sendEntraParam;//发送的额外参数
    void * sendData;//响应正文
    int sendLength; //响应长�
	int sendState;//
	void* sendBuf;
    char* clientKey;//响应客户端key
    char* clientUser;//响应用户名
    char* clientPassword;//响应密码
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
 * datalength:
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
char * getEntraParam(NetMessagePtr ptr,const char* paramName);
int setEntraParam(NetMessagePtr ptr,const char* paramName,const char* paramValue);

char * getSendEntraParam(NetMessagePtr ptr,const char* paramName);
int setSendEntraParam(NetMessagePtr ptr,const char* paramName,const char* paramValue);
#endif
