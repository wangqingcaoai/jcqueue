#ifndef BQ_TOPIC_H
#define BQ_TOPIC_H
#include "message.h"
#include "../data/heap.h"
//topic
//指一类型消息的集合，
typedef struct Topic
{
    int topic_id;
    char * topic_name;
    Heap* ready_queue;//就绪队列 有优先级
    Heap* delay_queue;//延迟队列 有优先级
    Heap* using_pool;//使用池，正在处理的消息
    Heap* sleep_queue;//休眠队列 正在休眠的消息
}Topic, * TopicPtr;
TopicPtr buildTopic(const char* topicName);//新建topic
int addMessage(TopicPtr topic,MessagePtr message,int delay);//添加消息
MessagePtr getReadyMessage(TopicPtr topic);//获取消息
int sleepMessage(TopicPtr topic,int messageid);
int reuseMessage(TopicPtr topic,int messageid,int delay);
int delMessage(TopicPtr topic, int messageid);
int wakeUpMessage(TopicPtr topic,int messageid,int delay);//唤醒某个消息。可指定唤醒时间
int tickTopic(TopicPtr topic,int64 timestamp);//对延迟队列和休眠队列进行检查进行处理
int freeTopic(TopicPtr *topic);
char* getTopicName(TopicPtr ptr);
int backupTopic(TopicPtr ptr);//还未撰写
int isSameTopicName(TopicPtr ptr,const char* topicName);


#endif