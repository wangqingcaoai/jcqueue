#ifndef SERVER_H
#define SERVER_H
#include "sockets.h"
#include "../data/heap.h"
#define TRANSFAR_SERVER_POSITION_NAME "socket_server"
#define TRANSFAR_SERVER_SUCCESS 0
#define TRANSFAR_SERVER_ERROR_PARAM_ERROR 1
typedef struct TransfarServer {
    int id;
    char *port;
    char *addr;
    int eventQueue;
    Socket sock;
    HeapPtr   conns;//这个server上的连接
}TransfarServer,*TransfarServerPtr;
//创建服务端套接字
int makeServerSocket(const char* host,const char*port);
int makeClientSocket(const char* host,const char*port);
TransfarServerPtr buildTransfarServer(const char* addr,const char* port);
int freeTransfarServer(TransfarServerPtr *);
int restoreTransfarServer(TransfarServerPtr );
int storeTransfarServer(TransfarServerPtr);
int acceptRequest(TransfarServerPtr tserver,const int ev);
void srvaccept(TransfarServer *s, int ev);
void srvserve(TransfarServer *s);
void srvtick(TransfarServer *s);

#endif
