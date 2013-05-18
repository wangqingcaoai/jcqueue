#ifndef AQ_SERVER_H
#define AQ_SERVER_H 
#include "../base_queue/server.h"
#include "subscribe.h"
#include "push.h"
#include "../transfar/server.h"
#define APP_SERVER_SUCCESS 0
#define APP_SERVER_ERROR_PARAM_ERROR 1;

typedef struct SubscribeServer * SubscribeServerPtr ;
typedef struct PushServer * PushServerPtr;
typedef struct TransfarServer * TransfarServerPtr;
typedef struct AppServer
{

	ListPtr usersList;
    BaseServerPtr baseServer ;
    SubscribeServerPtr subscribeServer;
    PushServerPtr pushServer;
    TransfarServerPtr transfarServer;
}AppServer , * AppServerPtr;
AppServerPtr buildAppServer();
int initAppServer(AppServerPtr);
int storeAppServer(AppServerPtr);
int freeAppServer(AppServerPtr *);
int processAppServer(AppServerPtr);
int processRequest(ConnectPtr ptr,int ev);
int processPusherResponse(ConnectPtr ptr,int ev);
int processResponse(ConnectPtr ptr,int ev);
int processPusherRequest(ConnectPtr ptr,int ev);
#endif
