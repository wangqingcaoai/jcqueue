#ifndef SOCKETS_H
#define SOCKETS_H
#include "../util/type.h"
typedef void(*Handle)(void*, int rw);
typedef struct Socket{
    int fd;//套接字的文件描述符
    Handle f;//句柄
    int added;//已添加标记
    void   *x;
}Socket;
//套接字事件队列初始化
int sockinit(void);
//获取下一个连接
int socknext(Socket **s, int64 timeout);
//期望在该链接发生什么事件时通知
int sockwant(Socket *s, int rw);
#endif