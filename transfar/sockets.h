#ifndef SOCKETS_H
#define SOCKETS_H
#include "../util/type.h"
#define TRANSFAR_SOCKET_POSITION_NAME "sockets"
typedef void(*Handle)(void*, int rw);
typedef struct Socket{
    int fd;//套接字的文件描述符
    Handle f;//句柄
    int added;//已添加标记
    void   *x;//关联结构体 ，为Connect 或Server
}Socket,* SocketPtr;
//套接字事件队列初始化
int sockinit(void);
//获取下一个连接
int socknext(int eq,SocketPtr *s, int64 timeout);
//期望在该链接发生什么事件时通知
int sockwant(int eq,SocketPtr s, int rw);
#endif