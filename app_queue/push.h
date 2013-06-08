/**
 * 消息推送
 */
#ifndef AQ_PUSH_H
#define AQ_PUSH_H 
#define PUSH_SUCCESS 0
#define PUSH_PARAM_ERROR 1;
#include "../transfar/connect.h"
#include "../data/list.h"
#include "subscribe.h"
#define PUSH_STATE_SENDING 1
#define PUSH_STATE_FINISH 2

#define PUSH_TO_CLIENT_CMD "push"
 typedef struct Connect * ConnectPtr;
 typedef struct Subscribe * SubscribePtr;
  typedef struct AppServer  * AppServerPtr;
 typedef struct PushServer
 {
 	int serverId;
 	ListPtr pushers;
    AppServerPtr appServer;
    long storePosition;
 }PushServer,*PushServerPtr;
 typedef struct PushServerStore
 {
    int serverId;
    long pushers;
    long appServer;
 }PushServerStore,*PushServerStorePtr;
typedef struct Pusher
{
	int pusherId;
	SubscribePtr subscribe;
	ListPtr messageReady;
	ConnectPtr connect;
    int state;
    long storePosition;
}Pusher,*PusherPtr;
typedef struct PusherStore
{
    int pusherId;
    long subscribe;
    long messageReady;
    //connect not need to store
    int state; 
}PusherStore,*PusherStorePtr;
PushServerPtr buildPushServer(AppServerPtr ptr);
int freePushServer(PushServerPtr * pptr);
PusherPtr buildPusher(SubscribePtr subscribe);
int freePusher(PusherPtr * pptr);
int addPusher(PushServerPtr server,SubscribePtr subscribe);
int addMessageToPusher(PusherPtr ,MessagePtr message);
int pushToTarget(PushServerPtr server, PusherPtr ptr);
int processPush(PushServerPtr );
int tickPushServer(PushServerPtr);
long storePusher(PusherPtr ptr);
PusherPtr restorePusher(long storePosition);
long storePushServer(PushServerPtr ptr);
PushServerPtr restorePushServer(long storePosition,AppServerPtr appServer);
#endif