#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../data/heap.h"
#include "../data/list.h"
#include "message.h"
#include "topic.h"
#include "../data/store.h"
#include "../util/maxids.h"
//新建topic
TopicPtr buildTopic(const char* topicName){
    if(isEmptyString(topicName)){
        return NULL;
    }
    
    //创建topic内存对象
    TopicPtr ptr= (TopicPtr)allocMem(sizeof(Topic));
    if(ptr == NULL){
        return ptr;
    }
    ptr->topicId = getTopicNextId();
    ptr->topicName = allocString(topicName);
    ptr->ready_queue = buildHeap((Record)MessageRecord,(Less)MessageLessByPriority);
    ptr->delay_queue = buildHeap((Record)MessageRecord,(Less)MessageLessByActiveTime);
    ptr->using_pool = buildHeap((Record)MessageRecord,(Less)MessageLessByPriority);
    ptr->sleep_queue = buildHeap((Record)MessageRecord,(Less)MessageLessByPriority);
    ptr->storePosition = 0L;
    return ptr;
}
//添加消息
int addMessage(TopicPtr topic,MessagePtr message,int delay){
    setMessageDelay(message,delay>=0?delay:0);
    if(delay > 0){
        printf("add  message to (%s) delay queue messageid:%" PRId64 " delay:%d\n",topic->topicName,message->messageid,delay);
        heapinsert(topic->delay_queue,message);
    }else{

        printf("add  message to (%s) ready queue messageid:%" PRId64 " delay:%d\n",topic->topicName,message->messageid,delay);
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
    printf("get (%s)ready messageid:%" PRId64 "\n",topic->topicName,ptr->messageid );
    heapinsert(topic->using_pool,ptr);
    return ptr;

}

int sleepMessage(TopicPtr topic,int64 messageid){
    //休眠的消息在使用队列中
    int i= heapFindIndex(topic->using_pool,(Find)isMessage,&messageid);
    if(i==-1){
        return -1;
    }else{
        MessagePtr ptr = heapremove(topic->using_pool,i);
        if(ptr == NULL){
            return -1;
        }else{
            printf("sleep (%s) message messageid:%" PRId64 "\n",topic->topicName,ptr->messageid);
            heapinsert(topic->sleep_queue,ptr);
        }
    }
    return 1;
}
int reuseMessage(TopicPtr topic,int64 messageid,int delay){
    int i= heapFindIndex(topic->using_pool,(Find)isMessage,&messageid);
    if(i==-1){
        return -1;
    }else{
        MessagePtr ptr = heapremove(topic->using_pool,i);
        if(ptr == NULL){
            return -1;
        }else{
            printf("reuse (%s) message messageid:%" PRId64 "\n",topic->topicName,ptr->messageid);
            
            addMessage(topic,ptr,delay);
        }
    }
    return 1;
}
int delMessage(TopicPtr topic, int64 messageid){
    int i= heapFindIndex(topic->using_pool,(Find)isMessage,&messageid);
    if(i!=-1){
        MessagePtr ptr = heapremove(topic->using_pool,i);
        printf("remove  message  from (%s)using_pool messageid:%" PRId64 "\n",topic->topicName,ptr->messageid);
        freeMessage(&ptr);
        return 1;
    }else if((i= heapFindIndex(topic->sleep_queue,(Find)isMessage,&messageid))!=-1){
        
        MessagePtr ptr = heapremove(topic->sleep_queue,i);
        printf("remove message from (%s)sleep_queue messageid:%" PRId64 "\n",topic->topicName,ptr->messageid);
        freeMessage(&ptr);
        return 1;
    }
    return -1;
}
//唤醒某个消息。可指定唤醒时间
int wakeUpMessage(TopicPtr topic,int64 messageid,int delay){
    int i= heapFindIndex(topic->sleep_queue,(Find)isMessage,&messageid);
    if(i==-1){
        return -1;
    }else{
        MessagePtr ptr = heapremove(topic->sleep_queue,i);
        if(ptr == NULL){
            return -1;
        }else{
            printf("wake  (%s)message messageid:%" PRId64 " delay:%d\n",topic->topicName,ptr->messageid,delay);
            
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
            printf("delay message in (%s)is ready  messageid:%" PRId64 "\n",topic->topicName,ptr->messageid);
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

    freeString(&(tptr->topicName));
    freeMem((void**)&tptr);
    (*topic)=NULL;
}
char* getTopicName(TopicPtr ptr){
    return ptr->topicName;
}
int backupTopic(TopicPtr ptr){

}//还未撰写
int isSameTopicName(TopicPtr ptr,const char* topicName){
    if(ptr == NULL || topicName == NULL){
        return 0;
    }
    return isSameString(ptr->topicName,topicName);
}
long storeTopic(TopicPtr ptr){
	if(ptr== NULL){
		return -1L;
	}
	TopicStore tstore;
	if(ptr->storePosition>0){
		restore(ptr->storePosition,&tstore,sizeof(TopicStore));
	}else{
		tstore.topicName=0;
		tstore.ready_queue=0;
		tstore.delay_queue=0;
		tstore.using_pool=0;
		tstore.sleep_queue=0;
	}
	tstore.topicId = ptr->topicId;
	tstore.topicName = storeString(tstore.topicName,ptr->topicName,TOPIC_MAX_TOPIC_NAME);
	tstore.ready_queue= storeHeap(ptr->ready_queue,(StoreHandle)storeMessage);

	tstore.delay_queue= storeHeap(ptr->delay_queue,(StoreHandle)storeMessage);

	
	tstore.using_pool= storeHeap(ptr->using_pool,(StoreHandle)storeMessage);

	tstore.sleep_queue= storeHeap(ptr->sleep_queue,(StoreHandle)storeMessage);
	ptr->storePosition=store(ptr->storePosition,&tstore,sizeof(TopicStore));
	return ptr->storePosition;
}

TopicPtr restoreTopic(long storePosition){
    if(storePosition <=0 ){
        return NULL;
    }
    TopicStore tstore;
    restore(storePosition,&tstore,sizeof(TopicStore));
    TopicPtr ptr= (TopicPtr)allocMem(sizeof(Topic));
    ptr->topicId = tstore.topicId;
    ptr->topicName = restoreString(tstore.topicName);
    ptr->ready_queue = restoreHeap(tstore.ready_queue,(RestoreHandle)restoreMessage,(Record)MessageRecord,(Less)MessageLessByPriority);
    ptr->delay_queue = restoreHeap(tstore.delay_queue,(RestoreHandle)restoreMessage,(Record)MessageRecord,(Less)MessageLessByActiveTime);
    ptr->using_pool = restoreHeap(tstore.using_pool,(RestoreHandle)restoreMessage,(Record)MessageRecord,(Less)MessageLessByPriority);
    ptr->sleep_queue = restoreHeap(tstore.sleep_queue,(RestoreHandle)restoreMessage,(Record)MessageRecord,(Less)MessageLessByPriority);
    ptr->storePosition = storePosition;      
    return ptr;
}

ListPtr getUsingMessages(TopicPtr ptr){
    if(ptr == NULL){
        return NULL;
    }
    ListPtr list = buildList();
    int i =0,count = getHeapLength(ptr->using_pool); 
    for ( i = 0; i < count; ++i)
    {
        insertToList(list, getHeapDataByIndex(ptr->using_pool,i));      
    }
    return list;
}
