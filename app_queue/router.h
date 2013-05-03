#ifndef AQ_ROUTER_H
#define AQ_ROUTER_H 
/**
 * 应用级队列服务路由
 */
int aq_router(AppServerPtr serverPtr, NetMessagePtr ptr);
int aq_(AppServerPtr serverPtr, NetMessagePtr ptr);
#endif