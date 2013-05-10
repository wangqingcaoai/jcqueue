#ifndef BQ_TOPIC_ROUTER_H
#define BQ_TOPIC_ROUTER_H
#include "server.h"
int addTopic(BaseServerPtr,const char* topicName);
int removeTopic(BaseServerPtr,const char* topicName);
int cleanTopic(BaseServerPtr,const char* topicName);//清空topic
TopicPtr useTopic(BaseServerPtr,const char*topicName);
ListPtr getTopicListByKeyword(BaseServerPtr ,const char* keyword);
#endif
