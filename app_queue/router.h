#ifndef AQ_ROUTER_H
#define AQ_ROUTER_H
#include "server.h" 
/**
 * 应用级队列服务路由
 */
#define AQ_SUBSCRIBE_ADD "subscribe_add"
#define AQ_SUBSCRIBE_DEL "subscribe_del"
#define AQ_SUBSCRIBE_DEL_BY_USER "subscribe_del_all"
#define AQ_SUBSCRIBE_DEL_BY_KEYWORD_AND_USER "subscribe_del_by_keyword"
#define AQ_SUBSCRIBE_GET_IDS "subscribe_get_ids"
#define AQ_SUBSCRIBE_GET_BY_ID "subscribe_get_by_id"
#define AQ_SUBSCRIBE_GET_TOPICS_BY_ID "subscribe_get_topics_by_id"
#define AQ_SYSTEM_STATE "system_state"

#define AQ_CLIENT_SERVER_ADD  "client_server_add"
#define AQ_CLIENT_SERVER_LIST "client_server_list"
#define AQ_CLIENT_SERVER_DEL  "client_server_del"
#define AQ_CLIENT_SERVER_USE  "client_server_use"
#define AQ_CLIENT_CLOSE "client_close"

#define AQ_SERVER_ADD_USER "add_user"
#define AQ_SERVER_DEL_USER "del_user"
#define AQ_SERVER_GET_USER "get_user"

int aq_router(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);
static int aq_subscribe_add(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);
static int aq_subscribe_del(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);
static int aq_subscribe_del_by_user(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);
static int aq_subscribe_del_by_keyword_and_user(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);
static int aq_subscribe_get_ids(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);
static int aq_subscribe_get_by_id(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);
static int aq_subscribe_get_topics_by_id(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);

static int aq_system_state(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);
static int aq_unknow_cmd(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);
static int aq_empty_appServer(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);

static int aq_client_server_add(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);

static int aq_client_to_server(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr user);
int aq_client_router(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr);
int aq_push_server_router(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr);
int aq_push_client_router(AppServerPtr serverPtr,NetMessagePtr ptr,UserPtr uptr);

static int aq_server_add_user(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);
static int aq_server_del_user(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);
static int aq_server_get_user(AppServerPtr serverPtr, NetMessagePtr ptr,UserPtr uptr);


#endif
