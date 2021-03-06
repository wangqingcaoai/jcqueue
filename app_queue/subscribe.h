/**
 * 订阅管理
 */
#ifndef AQ_SUBSCRIBE_H
#define AQ_SUBSCRIBE_H
#include "user.h"
#include "channel.h"
#include "server.h"
#include "../base_queue/message.h"
#include "../base_queue/topic.h"
#include "push.h"
#include "../parser/net_message.h"
#define SUBSCRIBE_SUCCESS 0
#define SUBSCRIBE_ERROR_PARAM_ERROR 1
#define SUBSCRIBE_ERROR_SUBSCRIBE_NOT_FOUND 2
#define SUBSCRIBE_ERROR_APP_SERVER_NOT_FOUND 3
#define SUBSCRIBE_ERROR_PARAM_EMPTY 7
#define SUBSCRIBE_TYPE_PUSH "push"
#define SUBSCRIBE_TYPE_GET "get"
#define SUBSCRIBE_POSITION_NAME "subscribe"
#define SUBSCRIBE_BUILD_SUBSCRIBE_TOPIC_FAILED 4
#define SUBSCRIBE_BUILD_SUBSCRIBE_FAILED 5
#define SUBSCRIBE_ERROR_SUBSCRIBE_TOPIC_NOT_FOUND 6
#define SUBSCRIBE_MAX_SUBSCRIBE_KEYWORDS_SIZE 512
#define SUBSCRIBE_MAX_SUBSCRIBE_REMOTEHOST_SIZE 124
#define SUBSCRIBE_MAX_SUBSCRIBE_PROTOCOL_SIZE 20
#define SUBSCRIBE_MAX_SUBSCRIBE_TYPE_SIZE 20

typedef struct Pusher* PusherPtr;
typedef struct AppServer * AppServerPtr;
typedef struct NetMessage * NetMessagePtr;
typedef struct User * UserPtr;
typedef struct Channel * ChannelPtr;
typedef struct Subscribe
{
    int64 subscribeId;
    char* subscribeKeyWord;//订阅,匹配的关键字
    char* remoteHost;//远端接收host 可以是ip
    unsigned int remotePort;//远端接收端口
    char* protocol;//远端支持的协议
    char* type;//订阅类型
    UserPtr user;//订阅的用户
    ChannelPtr channel;//订阅的用户频道
    ListPtr subscribedTopicLists;//指向订阅的topic
    PusherPtr pusher;
    long storePosition;
}Subscribe,*SubscribePtr;
typedef struct SubscribeStore
{
    int64 subscribeId;
    long subscribeKeyWord;//订阅,匹配的关键字
    long remoteHost;//远端接收host 可以是ip
    unsigned int remotePort;//远端接收端口
    long protocol;//远端支持的协议
    long type;//订阅类型
    long user;//订阅的用户
    long channel;//订阅的用户频道
    long subscribedTopicLists;//指向订阅的topic
    long pusher;
}SubscribeStore,*SubscribeStorePtr;
typedef struct SubscribeTopic{
    int64 id;
    TopicPtr topic;
    char* topicName;
    ListPtr subscribesList;//指向订阅
    long storePosition;
}SubscribeTopic, *SubscribeTopicPtr;

typedef struct SubscribeServer
{
    int serverId;
    ListPtr subscribeTopics;
    ListPtr subscribes;
    AppServerPtr appServer;
    long storePosition;
}SubscribeServer,* SubscribeServerPtr;
typedef struct SubscribeServerStore
{
    int serverId ;
    long subscribes;
    long appServer;
}SubscribeServerStore;
SubscribeServerPtr buildSubscribeServer(AppServerPtr appServer);
int freeSubscribeServer(SubscribeServerPtr *);
SubscribePtr buildSubScribe(const char* subscribeKeyWord,const char* remoteHost,const int remotePort,const char* protocol,const char* type, UserPtr user);
int freeSubscribe(SubscribePtr *);
int isSubscribeById(SubscribePtr ptr,const int64 *id);
SubscribeTopicPtr buildSubScribeTopic(const char* topicName,TopicPtr tptr);
int freeSubscribeTopic(SubscribeTopicPtr *pptr);
int isSubscribeTopicById(SubscribeTopicPtr ptr,const int64*  id);
int isSubscribeTopicByTopicName(SubscribeTopicPtr ptr,const char* topicName);
int addSubscribe(SubscribeServerPtr server, UserPtr userPtr , NetMessagePtr );

int delSubscribe(SubscribeServerPtr server,UserPtr userPtr,NetMessagePtr nmptr);
int delSubscribeByUser(SubscribeServerPtr server,UserPtr userPtr,NetMessagePtr);
int delSubscribebyKeywordAndUser(SubscribeServerPtr server,UserPtr userPtr,NetMessagePtr netPtr);
int getSubscribeIdsByUser(SubscribeServerPtr server,UserPtr user,NetMessagePtr);
int getSubscribeTopicIdsByUser(SubscribeServerPtr server,UserPtr user,NetMessagePtr);
int getSubscribeInfoById(SubscribeServerPtr server,UserPtr userPtr,NetMessagePtr );

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
int pushMessageToSubscribeList(SubscribeServerPtr server,ListPtr subscribes,MessagePtr message,TopicPtr topic);


long storeSubscribe(SubscribePtr ptr);
SubscribePtr restoreSubscribe(long storePosition);
long storeSubscribeServer(SubscribeServerPtr ptr);
SubscribeServerPtr restoreSubscribeServer(long storePosition,AppServerPtr appServer);
int tickSubscribeServer(SubscribeServerPtr);

SubscribePtr findSubscribeByStorePosition(SubscribeServerPtr server, long storePosition);
int isSubscribeByStorePosition(SubscribePtr ptr, long* storePosition);
ListPtr getAllPushingMessagesBySubscribe(SubscribePtr ptr);
#endif