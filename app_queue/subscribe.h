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
#define SUBSCRIBE_TYPE_PUSH "push"
#define SUBSCRIBE_TYPE_GET "get"
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
    char* topicName;
    ListPtr subscribesList;//指向订阅

}subscribeTopic, *subscribeTopicPtr;

typedef struct SubscribeServer
{
    int serverId;
    ListPtr topics;
    ListPtr subscribes;

}SubscribeServer,* SubscribeServerPtr;

SubscribeServerPtr buildSubscribeServer();
int freeSubscribeServer(SubscribeServerPtr *);
SubscribePtr bulidSubScribe(const char* subscribeKeyWord,const char* remoteHost,const int remotePort,const char* protocol,const char* type, UserPtr user);
int freeSubscribe(SubscribePtr *);
int isSubscribeById(SubscribePtr ptr,int id);
subscribeTopicPtr bulidSubScribeTopic(const char* topicName);
int freeScribeTopic(SubscribeTopicPtr *pptr);
int isSubscribeTopicById(subscribeTopicPtr ptr,int id);

int addSubscribe(SubscribeServerPtr server, UserPtr userPtr , NetMessagePtr );
int delSubscribe(SubscribeServerPtr server,int  subscribeId);
int delSubscribeByUser(SubscribeServerPtr server,UserPtr UserPtr,NetMessagePtr);
int getSubscribeIdsByUser(SubscribeServerPtr server);
// int getSubscribeTopicIdsByUser(SubscribeServerPtr server);

int addSubscribeTopic(SubscribeServerPtr subscribeServer,const char* topicName, SubscribePtr subscribe);
int delSubscribeTopic(SubscribeServerPtr subscribeServer,const char* topicName, SubscribePtr subscribe);
int processSubscribeTopic(SubscribeServerPtr subscribeServer);//need add Push server 



#endif