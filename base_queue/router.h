#ifndef BQ_ROUTER_H
#define BQ_ROUTER_H
#include "server.h"
#include "../parser/net_message.h"
/**
 * 基础队列服务的命令路由
 */
#define BQ_ADD "add"
#define BQ_GET "get"
#define BQ_SLEEP "sleep"
#define BQ_REUSE "resue"
#define BQ_DEL "del"
#define BQ_WAKEUP "wakeup"
#define BQ_ADD_TOPIC "add-topic"
#define BQ_DEL_TOPIC "del-topic"
#define BQ_CLEAN_TOPIC "clean-topic"
int bq_router(BaseServerPtr serverPtr, NetMessagePtr ptr);
static int bq_empty_baseServer(BaseServerPtr serverPtr,NetMessagePtr ptr);
static int bq_unknow_cmd(BaseServerPtr serverPtr,NetMessagePtr ptr);
static int bq_add(BaseServerPtr serverPtr,NetMessagePtr ptr);
static int bq_get(BaseServerPtr serverPtr,NetMessagePtr ptr);
static int bq_sleep(BaseServerPtr serverPtr,NetMessagePtr ptr);
static int bq_reuse(BaseServerPtr serverPtr,NetMessagePtr ptr);
static int bq_del(BaseServerPtr serverPtr,NetMessagePtr ptr);
static int bq_wakeup(BaseServerPtr serverPtr,NetMessagePtr ptr);
static int bq_add_topic(BaseServerPtr serverPtr,NetMessagePtr ptr);
static int bq_del_topic(BaseServerPtr serverPtr,NetMessagePtr ptr);
static int bq_clean_topic(BaseServerPtr serverPtr,NetMessagePtr ptr);

#endif