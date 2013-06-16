#include "maxids.h"
#include "../data/store.h"
int getPusherServerNextId(){
    return ++(maxIds.pusher_server_id);
}
int getPusherNextId(){
    return ++(maxIds.pusher_id);
} 
int getPusherMessageNextId(){
    return ++(maxIds.pusher_message_id);
}
int getRequestServerNextId(){
    return ++(maxIds.request_server_id);
}
int getRequesterNextId(){
    return ++(maxIds. requester_id);
}
int64 getMicroNetMessageNextId(){
    return ++(maxIds.micro_net_message_id);
}
int getSubscribeServerNextId(){
    return ++(maxIds.subscribe_server_id);
}
int64 getSubscribeNextId(){
    return ++(maxIds.subscribe_id);
}
int getSubscribeTopicNextId(){
    return ++(maxIds.subscribe_topic_id);
}
int getUserNextId(){
    return ++(maxIds.user_id);
}
int64 getMessageNextId(){
    return ++(maxIds.message_id);
}
int getBaseServerNextId(){
    return ++(maxIds.base_server_id);
}
int64 getTopicNextId(){
    return ++(maxIds.topic_id);
}
int getListNextId(){
    return ++(maxIds.list_id);
}
int64 getNetMessageNextId(){
    return ++(maxIds.net_message_id);
}
int getConnectNextId(){
    return ++(maxIds.connect_id);
}
int getTransfarServerNextId(){
    return ++(maxIds.transfar_server_id);
}
long initMaxIds(long position){
    restore(position,&maxIds,sizeof(MaxIds));
    return position;
}
long storeMaxIds(long position ){
    return store(position,&maxIds,sizeof(MaxIds));
}

