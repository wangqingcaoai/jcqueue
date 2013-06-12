#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "topic.h"
#include "server.h"
#include "../util/regex.h"
#include "../app_queue/subscribe.h"
int addTopic(BaseServerPtr ptr,const char* topicName){
    if(topicName == NULL){
        return -1;
    }
    if(ptr == NULL){
        return -1;
    }
    TopicPtr tptr = getFromList(ptr->topicList,(Find)isSameTopicName,(char*)topicName);
    if(tptr == NULL){
        printf("add new topic named %s\n", topicName);
        TopicPtr new = buildTopic(topicName);
        insertToList(ptr->topicList,new);
        UpdateSubscribeAfterAddTopic(ptr->appServer->subscribeServer,topicName);
        return 0;
    }else{
        return -1;
    }
}
int removeTopic(BaseServerPtr ptr,const char* topicName){
    if(topicName == NULL){
        return -1;
    }
    if(ptr == NULL){
        return -1;
    }
    int count = removeFromList(ptr->topicList,(Find)isSameTopicName,(char*)topicName,(Free)freeTopic);
    
    printf("remove topic named %s\n", topicName);
    UpdateSubscribeAfterRemoveTopic(ptr->appServer->subscribeServer,topicName);
    return count;   

}
int cleanTopic(BaseServerPtr ptr,const char* topicName){
    int count = removeTopic(ptr,topicName);
    if(count !=-1 ){
        return addTopic(ptr,topicName);
    }
    
    return -1;

}//清空topic
TopicPtr useTopic(BaseServerPtr ptr,const char*topicName){
    if(topicName == NULL){
        return NULL;
    }
    if(ptr == NULL){
        return NULL;
    }
    TopicPtr tptr = getFromList(ptr->topicList,(Find)isSameTopicName,(char*)topicName);
    return tptr;
}
int isMatchTopicName(TopicPtr ptr, const char* keyword){
    if(ptr==NULL){
        return 0;
    }else{
        if(!strcasecmp(ptr->topicName,keyword)){
            return 1;
        }
        int result = isMatchedString(ptr->topicName,keyword);
        if(result == REGEX_SUCCESS){
            return 1;
        }else{
            return 0;
        }
    }
}
ListPtr getTopicListByKeyword(BaseServerPtr ptr ,const char* keyword){
    if(ptr == NULL || keyword == NULL){
        return NULL;
    }
    ListNodePtr start = getListHeader(ptr->topicList);
    ListNodePtr end = getListEnd(ptr->topicList);
    if(start == NULL || end == NULL){
        return NULL;
    }
    TopicPtr tptr = nextFromList(&start,end,(Find)isMatchTopicName,(char*)keyword);
    ListPtr lptr = buildList();
    while(tptr!= NULL){
        insertToList(lptr,tptr);
        tptr = nextFromList(&start,end,(Find)isMatchTopicName,(char*)keyword);
    }
    return lptr;
}
int tickTopics(BaseServerPtr ptr){

    ListNodePtr start = getListHeader(ptr->topicList);
    ListNodePtr end = getListEnd(ptr->topicList);
    if(start == NULL || end == NULL){
        return -1;
    }
    TopicPtr tptr = nextFromList(&start,end,NULL,NULL);
    while(tptr!= NULL){
        tickTopic(tptr,nanoseconds());
        tptr = nextFromList(&start,end,NULL,NULL);
    }
    return 0;
}