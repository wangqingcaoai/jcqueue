#ifndef TRANSFAR_CONSOLE_H
#define TRANSFAR_CONSOLE_H
#include "server.h" 
#define CONSOLE_STATE_READ_WAIT_DATA 1//等待读取数据
#define CONSOLE_STATE_READ_FORMAT_ERROR 2
#define CONSOLE_STATE_READ_MESSAGE_READY 3
#define CONSOLE_RECV_BUF_SIZE 1024
#define CONSOLE_SUCCESS 0
#define CONSOLE_ERROR_PARAM_ERROR 1
#include "../parser/net_message.h"
typedef struct NetMessage * NetMessagePtr;
typedef struct TransfarServer * TransfarServerPtr;
typedef struct Console
{
    Socket in;
    Socket out;
    int state;
    TransfarServerPtr tServer;
    NetMessagePtr netMessage;
}Console,*ConsolePtr;
ConsolePtr buildConsole(TransfarServerPtr tServer);
int freeConsole(ConsolePtr *ptr);
int setOnBlock(int fd);
int consoleStart(TransfarServerPtr ptr);
int getConsoleData(ConsolePtr ptr);

int sendConsoleData(ConsolePtr ptr);
#endif