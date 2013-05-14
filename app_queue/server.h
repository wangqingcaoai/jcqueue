#ifndef AQ_SERVER_H
#define AQ_SERVER_H 
#include "../base_queue/server.h"
#include "subscribe.h"
#include "push.h"
typedef struct SubscribeServer * SubscribeServerPtr ;
typedef struct PushServer * PushServerPtr;
typedef struct AppServer
{
    ListPtr usersList;
    BaseServerPtr baseServer ;
    SubscribeServerPtr subscribeServer;
    PushServerPtr pushServer;
}AppServer , * AppServerPtr;
AppServerPtr buildAppServer();
int initAppServer(AppServerPtr);
int freeAppServer(AppServerPtr *);

#endif