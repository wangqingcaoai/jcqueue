#ifndef AQ_REQUEST_H
#define AQ_REQUEST_H
#define REQUEST_SUCCESS 0
#define REQUEST_PARAM_ERROR 1
#define REQUEST_ERROR_REQUESTER_NOT_FOUND 2
#define REQUEST_POSITION_NAME "requester"
#define REQUEST_STATE_SENDING 1
#define REQUEST_STATE_FINISH 2
#define REQUEST_LIST_BUF_SIZE 512

#define REQUEST_MAX_REQUEST_REMOTEHOST_SIZE 124
#define REQUEST_MAX_REQUEST_REMOTEPORT_SIZE 20
#define REQUEST_MAX_REQUEST_PROTOCOL_SIZE 20
#define REQUEST_MAX_REQUEST_VERSION_SIZE 20

#define REQUEST_MAX_REQUEST_CMD_SIZE 20
#define REQUEST_MAX_REQUEST_EXTRAPARAM_SIZE 512


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
    long storePosition;
}RequestServer ,*RequestServerPtr;
typedef struct RequestServerStore
{
    int serverId;
    long requesters;
    long current;
    long appServer;
}RequestServerStore,*RequestServerStorePtr;
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
    long storePosition;
}Requester,*RequesterPtr; 
typedef struct RequesterStore
{
    int requesterId;
    long user;
    long messageReady;
    long remoteHost;
    long remotePort;
    long protocol;
    long version;
    int state;
}RequesterStore,*RequesterStorePtr;
typedef struct MicroNetMessage
{
    int64 id;
    //recv
    char* cmd ;
    char* extraParam ;
    void* data;
    int length;
    int errcode;//
    long storePosition;
}MicroNetMessage,*MicroNetMessagePtr;
typedef struct MicroNetMessageStore
{
    int64 id;
    long cmd;
    long extraParam;
    long data;
    int length;
    int errcode;
}MicroNetMessageStore,*MicroNetMessageStorePtr;
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
MicroNetMessagePtr buildMicroNetMessage(NetMessagePtr ptr);
int freeMicroNetMessage(MicroNetMessagePtr *ptr);
int isRequester(RequesterPtr ptr,int* requesterId);

int getRequesterList(RequestServerPtr server,NetMessagePtr message);
int delRequesterFromServer(RequestServerPtr server,int requesterId);
int tickRequestServer(RequestServerPtr ptr);

long storeRequester(RequesterPtr ptr);
RequesterPtr restoreRequester(long storePosition);
long storeRequestServer(RequestServerPtr ptr);
RequestServerPtr restoreRequestServer(long storePosition,AppServerPtr appServer);
long storeMicroNetMessage(MicroNetMessagePtr ptr);
MicroNetMessagePtr restoreMicroNetMessage(long storePosition);
int isRequesterByStorePosition(RequesterPtr ptr, long * storePosition);
#endif