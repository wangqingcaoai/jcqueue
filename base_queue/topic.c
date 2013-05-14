#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../data/heap.h"
#include "message.h"
#include "topic.h"
//新建topic
TopicPtr buildTopic(const char* topicName){
    static int topicid;
    topicid++;
    //创建topic内存对象
    TopicPtr ptr= (TopicPtr)malloc(sizeof(Topic));
    ptr->topicId = topicid;
    ptr->topicName = malloc(strlen(topicName)+1);
    strcpy(ptr->topicName,topicName);
    ptr->ready_queue = buildHeap((Record)MessageRecord,(Less)MessageLess);
    ptr->delay_queue = buildHeap((Record)MessageRecord,(Less)MessageLess);
    ptr->using_pool = buildHeap((Record)MessageRecord,(Less)MessageLess);
    ptr->sleep_queue = buildHeap((Record)MessageRecord,(Less)MessageLess);

    return ptr;
}
//添加消息
int addMessage(TopicPtr topic,MessagePtr message,int delay){
    setMessageDelay(message,delay>=0?delay:0);
    if(delay > 0){
        heapinsert(topic->delay_queue,message);
    }else{
        heapinsert(topic->ready_queue,message);
    }
    return 0;
}//获取消息 就绪
MessagePtr getReadyMessage(TopicPtr topic){
    if(topic == NULL){
        return NULL;
    }
    MessagePtr ptr = (MessagePtr)heapremove(topic->ready_queue,0);
    if(ptr==NULL){
        return ptr;
    }
    heapinsert(topic->using_pool,ptr);
    return ptr;

}

int sleepMessage(TopicPtr topic,int messageid){
    //休眠的消息在使用队列中
    int i= heapFindIndex(topic->using_pool,(Find)isMessage,&messageid);
    if(i==-1){
        return -1;
    }else{
        MessagePtr ptr = heapremove(topic->using_pool,i);
        if(ptr == NULL){
            return -1;
        }else{
            heapinsert(topic->sleep_queue,ptr);
        }
    }
    return 1;
}
int reuseMessage(TopicPtr topic,int messageid,int delay){
    int i= heapFindIndex(topic->using_pool,(Find)isMessage,&messageid);
    if(i==-1){
        return -1;
    }else{
        MessagePtr ptr = heapremove(topic->using_pool,i);
        if(ptr == NULL){
            return -1;
        }else{
            addMessage(topic,ptr,delay);
        }
    }
    return 1;
}
int delMessage(TopicPtr topic, int messageid){
    int i= heapFindIndex(topic->using_pool,(Find)isMessage,&messageid);
    if(i!=-1){
        MessagePtr ptr = heapremove(topic->using_pool,i);
        freeMessage(&ptr);
        return 1;
    }else if((i= heapFindIndex(topic->sleep_queue,(Find)isMessage,&messageid))!=-1){
        MessagePtr ptr = heapremove(topic->sleep_queue,i);
        freeMessage(&ptr);
        return 1;
    }
    return -1;
}
//唤醒某个消息。可指定唤醒时间
int wakeUpMessage(TopicPtr topic,int messageid,int delay){
    int i= heapFindIndex(topic->sleep_queue,(Find)isMessage,&messageid);
    if(i==-1){
        return -1;
    }else{
        MessagePtr ptr = heapremove(topic->sleep_queue,i);
        if(ptr == NULL){
            return -1;
        }else{
            addMessage(topic,ptr,delay);
        }
    }
    return 1;

}
//对延迟队列和休眠队列进行检查进行处理
int tickTopic(TopicPtr topic,int64 timestamp){
    MessagePtr ptr=NULL;
    //对延迟队列进行检查
    while((ptr=heapremove(topic->delay_queue,0))!=NULL){
        if(isMessageWaitFinished(ptr,timestamp)){
            addMessage(topic,ptr,0);
            continue;
        }else{
            heapinsert(topic->delay_queue,ptr);
            break;
        }
    }
    return 1;
}
int freeTopic(TopicPtr *topic){
    if(topic == NULL){
        return -1;
    }
    TopicPtr tptr = (*topic);
    if(tptr ==NULL){
        return -1;
    }
    MessagePtr ptr= NULL;
    freeHeap(&(tptr->ready_queue),(Free)freeMessage);
    freeHeap(&(tptr->delay_queue),(Free)freeMessage);
    freeHeap(&(tptr->using_pool),(Free)freeMessage);
    freeHeap(&(tptr->sleep_queue),(Free)freeMessage);

    free(tptr->topicName);
    free(tptr);
    (*topic)=NULL;
    tptr = NULL;
}
char* getTopicName(TopicPtr ptr){
    return ptr->topicName;
}
int backupTopic(TopicPtr ptr){

}//还未撰写
int isSameTopicName(TopicPtr ptr,const char* topicName){

}