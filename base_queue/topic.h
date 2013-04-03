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
TopicPtr buildTopic();//新建topic
int addMessage(TopicPtr topic,MessagePtr message);//添加消息
void** getReadyMessage(TopicPtr topic);//获取消息
int sleepMessage(TopicPtr topic,int messageid);
int reuseMessage(TopicPtr topic,int messageid,int time);
int delMessage(TopicPtr topic, int messageid);
int wakeUpMessage(TopicPtr topic,int messageid,int time);//唤醒某个消息。可指定唤醒时间
int processCheck(TopicPtr topic);//对延迟队列和休眠队列进行检查进行处理
int freeTopic(TopicPtr topic);
#endif