/data/data/com.spartacusrex.spartacuside/files/system/bin/bash: q: command not found
#define AQ_ROUTER_H 
/**
 * 应用级队列服务路由
 */
int aq_router(AppServerPtr serverPtr, NetMessagePtr ptr);
int aq_(AppServerPtr serverPtr, NetMessagePtr ptr);
#endif
