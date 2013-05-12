#ifndef AQ_SERVER_H
#define AQ_SERVER_H 
typedef struct AppServer
{
    List UsersList;
    List BaseServer ;
    List SubscribeServer;
}AppServer , * AppServerPtr;
AppServerPtr buildAppServer();
int initAppServer(AppServerPtr);
int freeAppServer(AppServerPtr *);

#endif