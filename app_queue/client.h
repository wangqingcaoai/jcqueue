#ifndef AQ_CLIENT_H
#define AQ_CLIENT_H 
#include "../base_queue/server.h"
#include "subscribe.h"
#include "push.h"
#include "../transfar/server.h"

typedef struct TransfarServer * TransfarServerPtr;
AppServerPtr buildAppClientServer();
int initAppClientServer(AppServerPtr);
int storeAppClientServer(AppServerPtr);
int freeAppClientServer(AppServerPtr *);
int processAppClientServer(AppServerPtr);
int processClientRequest(ConnectPtr ptr,int ev);
int processClientPusherResponse(ConnectPtr ptr,int ev);
int processClientResponse(ConnectPtr ptr,int ev);
int processClientPusherRequest(ConnectPtr ptr,int ev);
#endif
