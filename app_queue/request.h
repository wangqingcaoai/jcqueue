#ifndef AQ_REQUEST_H
#define AQ_REQUEST_H
#define REQEUST_SUCCESS 0
#define REQEUST_PARAM_ERROR 1
#define REQEUST_ERROR_REQUESTER_NOT_FOUND 2
#define REQUEST_POSITION_NAME "requester"
#define REQUEST_STATE_SENDING 1
#define REQEUST_STATE_FINISH 2
#define REQEUST_LIST_BUF_SIZE 512

typedef struct AppServer  * AppServerPtr;
typedef struct Connect  * ConnectPtr;
typedef struct List  * ListPtr;
typedef struct User  * UserPtr;
typedef struct NetMessage  * NetMessagePtr;
typedef struct Requester * RequesterPtr;
typedef struct RequestServer
{
    int serverId;
    ListPtr requesters;
    RequesterPtr current;
    AppServerPtr appServer;
}RequestServer ,*RequestServerPtr;
typedef struct Requester
{
    int requesterId ;
    ConnectPtr connect;
    UserPtr user;
    ListPtr messageReady;
    char* remoteHost;
    char* remotePort;
    char* protocol;//远端支持的协议
    char* version;//订阅类型
    int state;
}Requester,*RequesterPtr; 
typedef struct MircoNetMessage
{
    int64 id;
    //recv
    char* cmd ;
    char* target;
    char* targetType;
    char* extraParam ;
    void* data;
    int length;
    int errcode;//
}MircoNetMessage,*MircoNetMessagePtr;

RequestServerPtr buildRequestServer(AppServerPtr ptr);
int freeRequestServer(RequestServerPtr * pptr);
RequesterPtr buildRequester(UserPtr user,const char* remoteHost,const char* remotePort);
int setRequesterProtocol(RequesterPtr ptr,const char* protocol,const char* version);
int freeRequester(RequesterPtr * pptr);
int addRequester(RequestServerPtr server,UserPtr user,const char* host,const char* port);
int addMessageToRequester(RequesterPtr ,NetMessagePtr ptr);
int requestToTarget(RequestServerPtr server, RequesterPtr ptr);
int processRequestServer(RequestServerPtr server );
int changeCurrentRequester(RequestServerPtr,int requesterId);
MircoNetMessagePtr buildMircoNetMessage(NetMessagePtr ptr);
int freeMircoNetMessage(MircoNetMessagePtr *ptr);
int isRequester(RequesterPtr ptr,int* requesterId);

int getRequesterList(RequestServerPtr server,NetMessagePtr message);
int delRequesterFromServer(RequestServerPtr server,int requesterId);
int tickRequestServer(RequestServerPtr ptr);
#endif