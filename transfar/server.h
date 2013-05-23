#ifndef SERVER_H
#define SERVER_H
#include "sockets.h"
#include "../data/heap.h"
#include "../app_queue/server.h"
#include "console.h"
#define TRANSFAR_SERVER_POSITION_NAME "socket_server"
#define TRANSFAR_SERVER_SUCCESS 0
#define TRANSFAR_SERVER_ERROR_PARAM_ERROR 1
#define TRANSFAR_SERVER_DEFAULT_PERIOD_TIME 10*1000000; // 10ms 
typedef struct AppServer * AppServerPtr;
typedef struct Console * ConsolePtr;
typedef struct TransfarServer {
    int id;
    char *port;
    char *addr;
    int eventQueue;
    Socket sock;
    HeapPtr   conns;//这个server上的连接
    ConsolePtr console;//命令行
    AppServerPtr appServer;
    int64_t period;
    int64_t lastServerTime;
}TransfarServer,*TransfarServerPtr;
//创建服务端套接字
int makeServerSocket(const char* host,const char*port);
int makeClientSocket(const char* host,const char*port);
TransfarServerPtr buildTransfarServer(AppServerPtr app, const char* addr,const char* port);
int initTransfarServer(const char*host,const);
int freeTransfarServer(TransfarServerPtr *);
int restoreTransfarServer(TransfarServerPtr );
int storeTransfarServer(TransfarServerPtr);
int acceptRequest(TransfarServerPtr tserver,const int ev);
void srvaccept(TransfarServerPtr s, int ev);
void srvStart(TransfarServerPtr s);
void srvProcess(TransfarServerPtr s );
void srvtick(TransfarServerPtr s);


#endif
