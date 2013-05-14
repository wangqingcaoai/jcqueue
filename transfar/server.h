#ifndef SERVER_H
#define SERVER_H
#include "sockets.h"
#include "../data/heap.h"
#define TRANSFAR_SERVER_POSITION_NAME "socket_server"
typedef struct Server {
    char *port;
    char *addr;
    char *user;
    Socket sock;
    Heap   conns;//这个server上的连接
}Server,*ServerPtr;
//创建服务端套接字
int makeServerSocket(char* host, char*port);
void srvaccept(Server *s, int ev);
void srvserve(Server *s);
void srvtick(Server *s);

#endif