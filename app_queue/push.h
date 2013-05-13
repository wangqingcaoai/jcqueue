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

 #endif