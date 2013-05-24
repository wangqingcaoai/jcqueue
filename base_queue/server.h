#ifndef BQ_SERVER
#define BQ_SERVER 
#include "../data/list.h"
#include "topic.h"
typedef struct AppServer * AppServerPtr;
typedef struct BaseServer
{
    int baseserver_id;
    /* data */
    ListPtr topicList;
    AppServerPtr appServer;
}BaseServer,* BaseServerPtr;
BaseServerPtr buildBaseServer(AppServerPtr app);
int freeBaseServer(BaseServerPtr * p);
int tickBaseServer(BaseServerPtr ptr);
#endif