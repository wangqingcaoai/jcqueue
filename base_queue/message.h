#ifndef BQ_MESSAGE_H
#define BQ_MESSAGE_H 
//topic 中的消息体
typedef enum  MessageState{
    READY,
    DELAY,
    SLEEP,
    USING
}MessageState;
typedef struct Message
{
    int id;//消息id
    MessageState state;//消息状态
    void* data;//消息体
    int time;//时间
    int priority;//优先级
}Message,MessagePtr*;
MessagePtr buildMessage(MessageState state,time,priority,void* data,int length);
int setMessageState(MessagePtr , MessageState);
#endif