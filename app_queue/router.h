#ifndef AQ_ROUTER_H
#define AQ_ROUTER_H
#include "server.h" 
/**
 * 应用级队列服务路由
 */
#define AQ_SUBSCRIBE_ADD "subscribe_add"
#define AQ_SUBSCRIBE_DEL "subscribe_del"
#define AQ_SYSTEM_STATE "system_state"

int aq_router(AppServerPtr serverPtr, NetMessagePtr ptr);
static int aq_subscribe_add(AppServerPtr serverPtr, NetMessagePtr ptr);
static int aq_subscribe_del(AppServerPtr serverPtr, NetMessagePtr ptr);
static int aq_system_state(AppServerPtr serverPtr, NetMessagePtr ptr);
static int aq_unknow_cmd(AppServerPtr serverPtr, NetMessagePtr ptr);
static int aq_empty_appServer(AppServerPtr serverPtr, NetMessagePtr ptr);
int aq_client_router(AppServerPtr serverPtr,NetMessagePtr ptr);

#endif
