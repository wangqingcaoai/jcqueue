/**
 * 消息推送
 */
 #ifndef AQ_PUSH_H
 #define AQ_PUSH_H 
 typedef struct PushServer
 {
 	int serverId;
 	ListPtr Pushers;
 }PushServer,*PushServerPtr;
typedef struct Pusher
{
	int pusherId;
	SubscribePtr subscribe;
	ListPtr messageReady;
	ConnectPtr con;
}Pusher,*PusherPtr;
PushServerPtr buildPushServer();

PusherPtr buildPusher(SubscribePtr subscribe);
int freePusher(SubscribePtr * pptr);
int addPusher(PushServerPtr server,SubscribePtr subscribe);
int addMessage(PusherPtr ,MessagePtr message);
int pushToTarget(PusherPtr ptr);
int processPush(PushServerPtr );
 #endif