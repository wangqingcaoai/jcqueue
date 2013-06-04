#ifndef BQ_TOPIC_H
#define BQ_TOPIC_H
#include "message.h"
#include "../data/heap.h"
//topic
//指一类型消息的集合，
typedef struct Topic
{
    int64 topicId;
    char * topicName;
    Heap* ready_queue;//就绪队列 有优先级
    Heap* delay_queue;//延迟队列 有优先级
    Heap* using_pool;//使用池，正在处理的消息
    Heap* sleep_queue;//休眠队列 正在休眠的消息
	long storePosition;
}Topic, * TopicPtr;
typedef struct TopicStore{
	int64 topicId;
	long topicName;
	long ready_queue;
	long delay_queue;
	long using_pool;
	long sleep_queue;
}
TopicPtr buildTopic(const char* topicName);//新建topic
int addMessage(TopicPtr topic,MessagePtr message,int delay);//添加消息
MessagePtr getReadyMessage(TopicPtr topic);//获取消息
int sleepMessage(TopicPtr topic,int64 messageid);
int reuseMessage(TopicPtr topic,int64 messageid,int delay);
int delMessage(TopicPtr topic, int64 messageid);
int wakeUpMessage(TopicPtr topic,int64 messageid,int delay);//唤醒某个消息。可指定唤醒时间
int tickTopic(TopicPtr topic,int64 timestamp);//对延迟队列和休眠队列进行检查进行处理
int freeTopic(TopicPtr *topic);
char* getTopicName(TopicPtr ptr);
int backupTopic(TopicPtr ptr);//还未撰写
int isSameTopicName(TopicPtr ptr,const char* topicName);
long storeTopic(TopicPtr ptr);
TopicPtr restoreTopic(long storePosition);
#endif
