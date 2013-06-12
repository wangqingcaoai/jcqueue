#ifndef CONNECT_H
#define CONNECT_H
#include "server.h"
#include "sockets.h"
#include "../parser/net_message.h"
#include "../app_queue/user.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#define CONNECT_STATE_READ_WAIT_DATA 1//等待读取数据
#define CONNECT_STATE_READ_FORMAT_ERROR 2
#define CONNECT_STATE_READ_MESSAGE_READY 3
#define CONNECT_STATE_SEND_CONNECTING 4 //推送时等待连接建立
#define CONNECT_STATE_SEND_WRITE_DATA 5
#define TRANSFAR_CONNECT_POSITION_NAME "connect"

#define CONNECT_SUCCESS 0
#define CONNECT_ERROR_PARAM_ERROR 1
#define CONNECT_RECV_BUF_SIZE 1024
#define CONNECT_MAX_IP_SIZE 32
#define CONNECT_MAX_PORT_SIZE 6
typedef struct TransfarServer* TransfarServerPtr;
typedef struct User * UserPtr;
typedef struct Connect{
    int id;
    TransfarServerPtr tServer;
    Socket sock;
    char state;
    char type;//client or request
	NetMessagePtr netMessage;//
    int offlineTime;// use to close outtime client
    char *addr;
	char *port;
    UserPtr user;
    int halfClose;
    Handle read;
    Handle write;
    void * relateData;
}Connect, *ConnectPtr ;
ConnectPtr buildConnect(const int cfd, const int state);
int setConnectHostInfo(ConnectPtr ptr,const char * addr,const char *port);	
void acceptConnect(TransfarServerPtr s,const int ev );
int getRequestData(ConnectPtr);
int setSecureInfo(ConnectPtr ,UserPtr ptr);
int sendResponseData(ConnectPtr);
int connectClose(ConnectPtr);
int connectTimeOut(ConnectPtr);
void closeFd(int fd);
int freeConnect(ConnectPtr *);
int compareConnect(ConnectPtr ,ConnectPtr);
ConnectPtr buildRequestConnect(TransfarServerPtr ptr,const char* addr,const char * port,Handle read,Handle write);
void checkConnectError(ConnectPtr c, const char *s);
char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen);
char *get_port_str(const struct sockaddr *sa, char *s, size_t maxlen);
int tickConnects(TransfarServerPtr ptr);
int setConncetWrite(ConnectPtr ptr);
int setConncetRead(ConnectPtr ptr);
#endif
