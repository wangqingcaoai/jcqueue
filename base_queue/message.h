#ifndef BQ_MESSAGE_H
#define BQ_MESSAGE_H 
#include "../util/util.h"
//topic 中的消息体
typedef enum  MessageState{
    MS_READY,
    MS_DELAY,
    MS_SLEEP,
    MS_USING
}MessageState;
typedef struct Message
{
    int64 messageid;//消息id
    MessageState state;//消息状态
    void* data;//消息体
    int64 timestamp;//外部时间
    int64 createtime;//创建时间
    int priority;//优先级 数值越低优先级越高
    int length;//消息长度
    int delay ;//消息的延迟时间
    int64 activetime;//消息激活时间
    long storePosition;
}Message,* MessagePtr;
typedef struct MessageStore
{
    
    int64 messageid;//消息id
    MessageState state;//消息状态
    long data;//消息体
    int64 timestamp;//外部时间
    int64 createtime;//创建时间
    int priority;//优先级 数值越低优先级越高
    int length;//消息长度
    int delay ;//消息的延迟时间
    int64 activetime;//消息激活时间

}MessageStore,*MessageStorePtr;
MessagePtr buildMessage(MessageState state,int64 timestamp, int priority,void* data,int length,int delay);
int setMessageState(MessagePtr , MessageState);
MessagePtr cloneMessage(MessagePtr );
int freeMessage(MessagePtr *);//释放消息
void * getMessageData(MessagePtr );
MessageState getMessageState(MessagePtr);
int MessageLess(MessagePtr , MessagePtr);
void MessageRecord(MessagePtr, int);
int isMessage(MessagePtr,int64* messageid);
void setMessageDelay(MessagePtr ptr,int delay);
int isMessageWaitFinished(MessagePtr ptr,int64 timestamp);
long storeMessage(MessagePtr ptr);
MessagePtr restoreMessage(long storePosition);
#endif