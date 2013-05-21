#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "jcq.h"
int parserNetMessage(NetMessagePtr ptr,void* buf,int length){
    if(ptr == NULL|| buf==NULL ||length <0){
        return PARSER_ERROR_PARAM_ERROR;
    }
    if(isJCQMessage(ptr,buf,length)){
        return parserJCQMessage(ptr,buf,length);
        
    }else{
        return PARSER_ERROR_DATA_NEED_MORE;
    }


}

int reparserNetMessage(NetMessagePtr ptr){
    if(!strcmp(ptr->protocolType,"jcq" )){
        reparserJCQMessage(ptr,ptr->protocolType,ptr->version);
    }
}
