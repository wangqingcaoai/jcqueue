#ifndef SERVER_H
#define SERVER_H
#include "sockets.h"
typedef struct Server {
    char *port;
    char *addr;
    char *user;

   // Wal    wal;
    Socket sock;
    //Heap   conns;//这个server上的连接
}Server;
//创建服务端套接字
int makeServerSocket(char* host, char*port);
static int verbose = 1;
void srvaccept(Server *s, int ev);
void srvserve(Server *s);
#endif