#ifndef CONNECT_H
#define CONNECT_H
#define CONNECT_STATE_WANTDATA 1//等待命令，用于刚连接时
#define CONNECT_STATE_SENDDATA 2
#define CONNECT_STATE_READDATA 3
#define CONNECT_STATE_WAIT 4
#define TRANSFAR_CONNECT_POSITION_NAME "connect"
#include "server.h"
#include "sockets.h"
#include "../parser/net_message.h"

typedef struct Connect{
    Server *srv;
    Socket sock;
    char state;
    char type;
    NetMessagePtr recvMessage;
    NetMessagePtr sendMessage;
    int recvLength;
    int sendLength;
    int onlineTime;
    int ip;
}Connect, *ConnectPtr ;
ConnectPtr buildConnect(const int cfd, const int state);
void acceptConnect(ServerPtr s,const int ev );
int readData(ConnectPtr,void *buffer, int length);
int writeData(ConnectPtr,void *buffer, int length);
int connectClose(ConnectPtr);
int connectTimeOut(ConnectPtr);
void closeFd(int fd);
void connectFree(ConnectPtr);
#endif