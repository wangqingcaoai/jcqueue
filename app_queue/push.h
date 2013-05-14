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
 typedef struct Subscribe * SubscribePtr;
 typedef struct PushServer
 {
 	int serverId;
 	ListPtr pushers;
 }PushServer,*PushServerPtr;
typedef struct Pusher
{
	int pusherId;
	SubscribePtr* subscribe;
	ListPtr messageReady;
	ConnectPtr con;
}Pusher,*PusherPtr;

PushServerPtr buildPushServer();
int freePushServer(PushServerPtr * pptr);
PusherPtr buildPusher(SubscribePtr subscribe);
int freePusher(PusherPtr * pptr);
int addPusher(PushServerPtr server,SubscribePtr subscribe);
int addMessageToPusher(PusherPtr ,MessagePtr message);
int pushToTarget(PusherPtr ptr);
int processPush(PushServerPtr );
 #endif