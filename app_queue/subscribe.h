/**
 * 订阅管理
 */
#ifndef AQ_SUBSCRIBE_H
#define AQ_SUBSCRIBE_H
#include "user.h"
#include "channel.h"
typedef struct Subscribe
{
    int subscribeId;
    char* subscribeKeyWord;//订阅,匹配的关键字
    char* remoteHost;//远端接收host 可以是ip
    char* remotePort;//远端接收端口
    char* protocol;//远端支持的协议
    UserPtr user;//订阅的用户
    ChannelPtr channel;//订阅的用户频道
    List subscribedTopicIds;
}Subscribe,*SubscribePtr;
#endif