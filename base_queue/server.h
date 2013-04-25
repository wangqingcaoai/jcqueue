#ifndef BQ_SERVER
#define BQ_SERVER 
#include "../data/list.h"
#include "topic.h"
typedef struct BaseServer
{
    int baseserver_id;
    /* data */
    ListPtr topicList;
}BaseServer,* BaseServerPtr;
BaseServerPtr buildBaseServer();
int freeBaseServer(BaseServerPtr * p);
#endif