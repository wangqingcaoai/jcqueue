#ifndef UTIL_MAX_IDS
#define UTIL_MAX_IDS 

#include "../util/util.h"
typedef struct MaxIds
{
    int pusher_server_id;
     int pusher_id;
    int pusher_message_id;
    int request_server_id;
    int requester_id;
    int64 micro_net_message_id;
    int subscribe_server_id;
    int64 subscribe_id;
    int subscribe_topic_id;  
    int user_id;
    int64 message_id;
    int base_server_id;
    int64 topic_id;
    int list_id;
    int64 net_message_id;
    int connect_id;
    int transfar_server_id;

}MaxIds,MaxIdsPtr;
static MaxIds maxIds;
int getPusherServerNextId(); 
int getPusherNextId(); 
int getPusherMessageNextId(); 
int getRequestServerNextId();
int getRequesterNextId();
int64 getMicroNetMessageNextId();
int getSubscribeServerNextId();
int64 getSubscribeNextId();
int getSubscribeTopicNextId();
int getUserNextId();
int64 getMessageNextId();
int getBaseServerNextId();
int64 getTopicNextId();
int getListNextId();
int64 getNetMessageNextId();
int getConnectNextId();
int getTransfarServerNextId();

#endif