/**
 * 订阅管理
 */
#ifndef AQ_SUBSCRIBE_H
#define AQ_SUBSCRIBE_H
#include "user.h"
#include "channel.h"
#define SUBSCRIBE_SUCCESS 0
#define SUBSCRIBE_ERROR_PARAM_ERROR 1
#define SUBSCRIBE_ERROR_SUBSCRIBE_NOT_FOUND 2
#define SUBSCRIBE_ERROR_APP_SERVER_NOT_FOUND 3
#define SUBSCRIBE_TYPE_PUSH "push"
#define SUBSCRIBE_TYPE_GET "get"
#define SUBSCRIBE_POSITION_NAME "subscribe"
#define SUBSCRIBE_BUILD_SUBSCRIBE_TOPIC_FAILED 4
#define SUBSCRIBE_BUILD_SUBSCRIBE_FAILED 5

typedef struct Subscribe
{
    int subscribeId;
    char* subscribeKeyWord;//订阅,匹配的关键字
    char* remoteHost;//远端接收host 可以是ip
    unsigned int remotePort;//远端接收端口
    char* protocol;//远端支持的协议
    char* type;//订阅类型
    UserPtr user;//订阅的用户
    ChannelPtr channel;//订阅的用户频道
    ListPtr subscribedTopicLists;//指向订阅的topic
}Subscribe,*SubscribePtr;
typedef struct subscribeTopic{
    int id;
    TopicPtr topic;
    char* topicName;
    ListPtr subscribesList;//指向订阅

}subscribeTopic, *subscribeTopicPtr;

typedef struct SubscribeServer
{
    int serverId;
    ListPtr subscribeTopics;
    ListPtr subscribes;
    AppServerPtr appServer;
}SubscribeServer,* SubscribeServerPtr;

SubscribeServerPtr buildSubscribeServer(AppServerPtr appServer);
int freeSubscribeServer(SubscribeServerPtr *);
SubscribePtr bulidSubScribe(const char* subscribeKeyWord,const char* remoteHost,const int remotePort,const char* protocol,const char* type, UserPtr user);
int freeSubscribe(SubscribePtr *);
int isSubscribeById(SubscribePtr ptr,int id);

subscribeTopicPtr bulidSubScribeTopic(const char* topicName,TopicPtr tptr);
int freeScribeTopic(SubscribeTopicPtr *pptr);
int isSubscribeTopicById(SubscribeTopicPtr ptr,int  id);
int isSubscribeTopicByTopicName(SubscribeTopicPtr ptr,const char* topicName);
int addSubscribe(SubscribeServerPtr server, UserPtr userPtr , NetMessagePtr );
int delSubscribe(SubscribeServerPtr server,int  subscribeId);
int delSubscribeByUser(SubscribeServerPtr server,UserPtr UserPtr,NetMessagePtr);
int getSubscribeIdsByUser(SubscribeServerPtr server);
// int getSubscribeTopicIdsByUser(SubscribeServerPtr server);
int delSubscribebyKeywordAndUser(SubscribeServerPtr server,UserPtr userPtr,NetMessagePtr netPtr);


int addSubscribeTopic(SubscribeServerPtr subscribeServer,TopicPtr tptr, SubscribePtr subscribe);
int addSubscribeTopicsByList(SubscribeServerPtr subscribeServer,ListPtr topicList, SubscribePtr subscribe);
int delSubscribeTopic(SubscribeServerPtr subscribeServer,const char* topicName, SubscribePtr subscribe);
int processSubscribeTopic(SubscribeServerPtr subscribeServer);//need add Push server 

int addSubscribeToSubscribeTopic(SubscribeTopicPtr ptr,SubscribePtr sptr);
int addSubscribeTopicToSubscribe(SubscribePtr ptr,SubscribeTopicPtr sptr);
//增加新的topic时检查 是否有匹配的订阅需要更新
int UpdateSubscribeAfterAddTopic(SubscribeServerPtr server, const char*topicName);
int UpdateSubscribeAfterRemoveTopic(SubscribeServerPtr server, const char*topicName);
int isMatchSubscribeByTopicName(SubscribePtr ,const char* topicName);
#endif