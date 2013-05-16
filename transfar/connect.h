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
#define CONNECT_SUCCESS 0
#define CONNECT_ERROR_PARAM_ERROR 1
typedef struct Connect{
    int id;
    TransfarServerPtr srv;
    Socket sock;
    char state;
    char type;
    NetMessagePtr netMessage;
    int onlineTime;
    char* ip;
}Connect, *ConnectPtr ;
ConnectPtr buildConnect(const int cfd, const int state);
void acceptConnect(TransfarServerPtr s,const int ev );
int readData(ConnectPtr,void *buffer, int length);
int writeData(ConnectPtr,void *buffer, int length);
int connectClose(ConnectPtr);
int connectTimeOut(ConnectPtr);
void closeFd(int fd);
int freeConnect(ConnectPtr *);
int compareConnect(ConnectPtr ,ConnectPtr);

#endif