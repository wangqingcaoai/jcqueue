#ifndef BQ_TOPIC_H
#define BQ_TOPIC_H
#include "message.h" 
//topic
//指一类型消息的集合，
typedef struct Topic
{
    int topic_id;
    char * topic_desc;
    int ready_queue;//就绪队列 有优先级
    int delay_queue;//延迟队列 有优先级
    int using_pool;//使用池，正在处理的消息
    int sleep_queue;//休眠队列 正在休眠的消息
}Topic,TopicPtr*;
int addMessage(TopicPtr topic,MessagePtr message);//添加消息
void** getMessage(TopicPtr topic);//获取消息
int sleepMessage(TopicPtr topic,int messageid);
int reuseMessage(TopicPtr topic,int messageid,int time);
int delMessage(TopicPtr topic, int messageid);
int wakeUpMessage(TopicPtr topic,int messageid,int time);
#endif