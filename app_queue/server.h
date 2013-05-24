#ifndef AQ_SERVER_H
#define AQ_SERVER_H 
#include "../base_queue/server.h"
#include "subscribe.h"
#include "push.h"
#include "../transfar/sockets.h"
#include "../transfar/server.h"
#include "../transfar/connect.h"
#define APP_SERVER_SUCCESS 0
#define APP_SERVER_ERROR_PARAM_ERROR 1;
typedef void(*TickHandle)(void*);
typedef struct SubscribeServer * SubscribeServerPtr ;
typedef struct PushServer * PushServerPtr;
typedef struct TransfarServer * TransfarServerPtr;
typedef struct Connect * ConnectPtr ;
typedef struct Console * ConsolePtr ;
typedef struct User * UserPtr ;
typedef struct RequestServer * RequestServerPtr;
typedef struct AppServer
{

	ListPtr usersList;
    UserPtr admin;
    UserPtr sendUser;
    UserPtr acceptUser;
    BaseServerPtr baseServer ;
    SubscribeServerPtr subscribeServer;
    PushServerPtr pushServer;
    TransfarServerPtr transfarServer;
    RequestServerPtr requestServer;
	Handle request;
	Handle response;
	Handle pusherRequest;
	Handle pusherResponse;
    Handle consoleIn;
    Handle consoleOut;
    Handle requestOut;
    Handle responseOut;
    TickHandle tick;
	
}AppServer , * AppServerPtr;
AppServerPtr buildAppServer(const char* host,const char* port);
int initAppServer(AppServerPtr);
int storeAppServer(AppServerPtr);
int freeAppServer(AppServerPtr *);
int processAppServer(AppServerPtr);
int processRequest(ConnectPtr ptr,int ev);
int processPusherResponse(ConnectPtr ptr,int ev);
int processResponse(ConnectPtr ptr,int ev);
int processPusherRequest(ConnectPtr ptr,int ev);
int processConsoleIn(ConsolePtr ptr,int ev);
int processConsoleOut(ConsolePtr ptr,int ev);

int processRequestOut(ConnectPtr ptr,int ev);
int processResponseOut(ConnectPtr ptr,int ev);
int tickAppServer(AppServerPtr);
#endif
