#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net_message.h"
#include "jcq.h"

int parserJCQMessage(NetMessagePtr ptr,void * buf, int length){
    if(ptr == NULL|| buf == NULL || length <0){
        return PARSER_ERROR_PARAM_ERROR;
    }
    if(length == 0){
        return PARSER_SUCCESS;
    }
    int readLength = 0,result =0;
    while(readLength <length){

        switch(ptr->readState)
        {
            case NETMESSAGE_READSTATE_WAIT:
                ptr->readState = NETMESSAGE_READSTATE_PARAM;
            case NETMESSAGE_READSTATE_PARAM:
                Param param;
                param.paramName = NULL;
                param.paramData = NULL;
                // has data in last parserbuffer ,need to process it;
                if(ptr->lastParserBuf != NULL){
                    // alloc a mem to store last and new 
                    void * temp = allocMem(ptr->lastParserBufLength + length);
                    memcpy(temp,ptr->lastParserBuf,ptr->lastParserBufLength);
                    memcpy(temp,buf,length);
                    free(ptr->lastParserBuf);
                    ptr->lastParserBuf = NULL;
                    // now get the param 
                    getJCQParamNameAndData(temp,&param,ptr->lastParserBufLength + length);
                }
                int result = getJCQParamNameAndData(buf+readLength,&param,length - readLength);
                if(result == PARSER_ERROR_DATA_NEED_MORE){
                    // we need more data to parser the param so just save it in buffer
                    ptr->lastParserBuf = allocMem(length - readLength);
                    memcpy(ptr->lastParserBuf,buf+readLength,length -readLength);
                    ptr->lastParserBufLength = length -readLength;
                    readLength = length;
                }else if(result == PARSER_SUCCESS){
                    readLength += param.readLength;
                }else if(result == PARSER_ERROR_PARAM_ERROR){

                }
                break;
            case NETMESSAGE_READSTATE_DATA:
            //read data finish 
                if(ptr->readLength  == ptr->length){
                    ptr->readState = NETMESSAGE_READSTATE_FINISH;
                //also can read data
                }else if(ptr->readLength  < ptr->length){
                    int copyLength = length;
                    copyLength = (ptr->readLength + length <= ptr->length) ? length : ptr->length - ptr->readLength;
                    memcpy(ptr->data + ptr->readLength, buf, copyLength);
                    ptr->readLength += copyLength;
                    readLength += copyLength;
                //has error on read data
                }else{
                    return PARSER_ERROR_DATA_OUT_LINE;
                }
                break;
            case NETMESSAGE_READSTATE_FINISH:
                break;
        }
    }
        
}
int buildJCQMessage(NetMessagePtr ptr,void * buf, int length){

}

int getJCQParamNameAndData(char* ptr,ParamPtr param, int length){
    if(ptr == NULL || param == NULL){
        return PARSER_ERROR_PARAM_ERROR;
    }
    int i =0,split = 0;

    while(i<length && ptr[i]!=':'){
        i++;
    }
    if(ptr[i] == ':'){
        split = i;
        //end with \r\n so test like this 
        while(i+1 <length && ptr[i]!='\r'  ){
            i++;
        }
        if(ptr[i] == '\r' && ptr[i+1] == '\n'){
            ptr[i] = '\0';
            ptr[i+1] = '\0';
            ptr[split] = '\0';
            param->readLength = i+1;
            param->paramName = ptr;
            param->paramData = ptr+split+1;
            return PARSER_SUCCESS;
        }
        return PARSER_ERROR_DATA_NEED_MORE;
    }
    return PARSER_ERROR_DATA_NEED_MORE;
}

