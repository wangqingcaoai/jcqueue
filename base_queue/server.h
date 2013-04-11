#ifndef BQ_SERVER
#define BQ_SERVER 
#include "../data/list.h"
typedef struct BaseServer
{
    int baseserver_id;
    /* data */
    ListPtr topicList;
}BaseServer,* BaseServerPtr;
BaseServerPtr buildBaseServer();
int addTopic(BaseServerPtr,char* topicName);
int removeTopic(BaseServerPtr,char* topicName);
int cleanTopic(BaseServerPtr,char* topicName);//清空topic
int freeBaseServer(BaseServerPtr * p);
#endif