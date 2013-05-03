#ifndef AQ_CHANNEL_H
#define AQ_CHANNEL_H
#include "../transfar/connect.h"
typedef struct Channel
{
    ConnectPtr connect;//对应的连接
}Channel,*ChannelPtr; 
ChannelPtr buildChannel();
int freeChannel(ChannelPtr *);
#endif