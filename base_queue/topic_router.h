
#ifndef BQ_TOPIC_ROUTER_H
#define BQ_TOPIC_ROUTER_H
#include "server.h"
#define TOPIC_LIST_BUF_SIZE 1024
int addTopic(BaseServerPtr,const char* topicName);
int removeTopic(BaseServerPtr,const char* topicName);
int cleanTopic(BaseServerPtr,const char* topicName);//清空topic
TopicPtr useTopic(BaseServerPtr,const char*topicName);
ListPtr getTopicListByKeyword(BaseServerPtr ,const char* keyword);
int isMatchTopicName(TopicPtr , const char* keyword);
int tickTopics(BaseServerPtr );
#endif
