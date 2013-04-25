#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "topic.h"
#include "server.h"
int addTopic(BaseServerPtr ptr,char* topicName){
    if(topicName == NULL){
        return -1;
    }
    if(ptr == NULL){
        return -1;
    }
    TopicPtr tptr = getFromList(ptr->topicList,(Find)isSameTopicName,topicName);
    if(tptr == NULL){
        TopicPtr new = buildTopic(topicName);
        insertToList(ptr->topicList,new);
        return 0;
    }else{
        return -1;
    }
}
int removeTopic(BaseServerPtr ptr,char* topicName){
    if(topicName == NULL){
        return -1;
    }
    if(ptr == NULL){
        return -1;
    }
    int count = removeFromList(ptr->topicList,(Find)isSameTopicName,topicName,(Free)freeTopic);
    return count;   

}
int cleanTopic(BaseServerPtr ptr,char* topicName){
    int count = removeTopic(ptr,topicName);
    if(count !=-1 ){
        return addTopic(ptr,topicName);
    }
    
    return -1;

}//清空topic
TopicPtr useTopic(BaseServerPtr ptr,char*topicName){
    if(topicName == NULL){
        return NULL;
    }
    if(ptr == NULL){
        return NULL;
    }
    TopicPtr tptr = getFromList(ptr->topicList,(Find)isSameTopicName,topicName);
    return tptr;
}