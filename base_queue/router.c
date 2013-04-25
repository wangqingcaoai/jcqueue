#include <stdio.h>
#include <stdlib.h>
#include "router.h"
int bq_router(NetMessagePtr ptr){
    if(ptr == NULL){
        return -1;
    }
    if(ptr->cmd == NULL){
        return -1;
    }
    if(strcasecmp(ptr->cmd,))

}

