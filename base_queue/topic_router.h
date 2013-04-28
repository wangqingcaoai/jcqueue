#ifndef BQ_TOPIC_ROUTER_H
#define BQ_TOPIC_ROUTER_H
#include "server.h"
int addTopic(BaseServerPtr,char* topicName);
int removeTopic(BaseServerPtr,char* topicName);
int cleanTopic(BaseServerPtr,char* topicName);//清空topic
TopicPtr useTopic(BaseServerPtr,char*topicName); 
#endif
