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
#define
int bq_router(NetMessagePtr ptr);
#endif