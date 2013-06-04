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
    long storePosition;
}BaseServer,* BaseServerPtr;
typedef struct BaseServerStore
{
    int baseserver_id;
    long topicList;
    long appServer;
}BaseServerStore,*BaseServerStorePtr;
BaseServerPtr buildBaseServer(AppServerPtr app);
int freeBaseServer(BaseServerPtr * p);
int tickBaseServer(BaseServerPtr ptr);
long storeBaseServer(BaseServerPtr);
BaseServerPtr restoreBaseServer(long storePosition,AppServerPtr ptr);

#endif