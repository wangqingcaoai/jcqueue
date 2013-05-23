#ifndef AQ_CLIENT_H
#define AQ_CLIENT_H 
#include "../base_queue/server.h"
#include "subscribe.h"
#include "push.h"
#include "../transfar/server.h"

typedef struct TransfarServer * TransfarServerPtr;
AppServerPtr buildClientAppServer(const char* host,const char* port);
int initClientAppServer(AppServerPtr);
int storeClientAppServer(AppServerPtr);
int freeClientAppServer(AppServerPtr *);
int processClientAppServer(AppServerPtr);
int processClientRequest(ConnectPtr ptr,int ev);
int processClientPusherResponse(ConnectPtr ptr,int ev);
int processClientResponse(ConnectPtr ptr,int ev);
int processClientPusherRequest(ConnectPtr ptr,int ev);
int processClientConsoleIn(ConsolePtr ptr,int ev);
int processClientConsoleOut(ConsolePtr ptr,int ev);

int tickClientAppServer(AppServerPtr ptr);
#endif
