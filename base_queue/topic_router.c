#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "topic.h"
#include "server.h"
#include "../util/regex.h"
int addTopic(BaseServerPtr ptr,const char* topicName){
    if(topicName == NULL){
        return -1;
    }
    if(ptr == NULL){
        return -1;
    }
    TopicPtr tptr = getFromList(ptr->topicList,(Find)isSameTopicName,(char*)topicName);
    if(tptr == NULL){
        TopicPtr new = buildTopic(topicName);
        insertToList(ptr->topicList,new);
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

