#ifndef AQ_SERVER_H
#define AQ_SERVER_H 
typedef struct AppServer
{
    ListPtr UsersList;
    ListPtr BaseServer ;
    ListPtr SubscribeServer;
    ListPtr PushServer;
}AppServer , * AppServerPtr;
AppServerPtr buildAppServer();
int initAppServer(AppServerPtr);
int freeAppServer(AppServerPtr *);

#endif