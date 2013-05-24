#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net_message.h"
#include "parser.h"
#include "../util/util.h"
#include "console.h"
int parserCMDMessage(NetMessagePtr ptr,void * buf, int length){
    if(ptr == NULL || buf == NULL || length <0){
        return PARSER_ERROR_PARAM_ERROR;
    }
    char * temp = NULL;
    int realLength = 0;
    if(ptr->lastParserBuf != NULL && ptr->lastParserBufLength >0)
    {
        
        
        temp = (void*)allocMem(ptr->lastParserBufLength + length);
        memcpy(temp,ptr->lastParserBuf,ptr->lastParserBufLength);
        if(length !=0){
            memcpy(temp+ptr->lastParserBufLength,buf,length);    
        }
        
        realLength =length+ ptr->lastParserBufLength;
        freeMem((void**)&(ptr->lastParserBuf));
        ptr->lastParserBufLength =0;
        ptr->lastParserBuf =NULL;
    }else{
        if(length == 0){
            return PARSER_SUCCESS;
        }
        temp = allocMem(length);
        memcpy(temp,buf,length);
        realLength = length;    
    }
    char* line = strnstr(temp,"\n",realLength);
    int error = PARSER_SUCCESS;
    if(line == NULL){

        setLastParamBuffer(ptr,temp,realLength);
        error = PARSER_ERROR_DATA_NEED_MORE;
    }else{
        int readLength = line-temp+1;
        setLastParamBuffer(ptr,line+1,realLength-readLength);
        int leavLength = realLength-readLength; 
        char bufcmd[leavLength+1];
        char bufTarget[leavLength+1];
        char bufTargetType[leavLength+1];
        char bufExtraParam[leavLength+1];
        char bufData[leavLength+1];
        memset(bufcmd,'\0',leavLength+1);

        memset(bufExtraParam,'\0',leavLength+1);

        memset(bufData,'\0',leavLength+1);
        
        sscanf(temp,CMD_READ_FORMAT,bufcmd,bufExtraParam,bufData);
        if(isEmptyString(bufcmd)){
            error = PARSER_ERROR_FORMAT_ERROR;
        }else{
            if(isSameString(bufcmd,"-")){
                error = PARSER_ERROR_FORMAT_ERROR;
            }else{
                setNetMessageParam(ptr,"cmd",bufcmd);
                if(!isSameString(bufExtraParam,"-")){
                    if(isExtraParamFormatRight(bufExtraParam,strlen(bufExtraParam))){
                        setNetMessageParam(ptr,"extraParam",bufExtraParam);    
                    }else{
                        error = PARSER_ERROR_FORMAT_ERROR;
                    }    
                }else{
                    setNetMessageParam(ptr,"extraParam","");
                }
                
                
                if(ptr->data!=NULL){
                    freeMem(&(ptr->data));
                }
                if(isEmptyString(bufData)||isSameString(bufData,"-")){
                    ptr->data = allocString("");
                    ptr->length = 0;
                }else if(!strncmp(bufData,"data:",5)){
                    ptr->data = allocString(bufData+5);
                    ptr->length = strlen(bufData+5);
                }
            }
        }
            
    }
    freeMem((void**)&temp);
    return error;

}
int reparserCMDMessage(NetMessagePtr ptr,char * protocolType, char* version){
    if(ptr== NULL || protocolType == NULL || version == NULL){
        return PARSER_ERROR_PARAM_ERROR;
    }
    if(ptr->sendBuf == NULL){
        ptr->sendBuf = allocMem(NETMESSAGE_DEFAULT_SEND_BUF_SIZE);
    }
    ptr->sendTime = nanoseconds();
    int writedLength = snprintf(ptr->sendBuf,NETMESSAGE_DEFAULT_SEND_BUF_SIZE,CMD_WRITE_FORMAT,
        ptr->sendCmd,
        ptr->currentUser,
        ptr->currentPassword,
        ptr->currentUserKey,
        ptr->sendErrcode,
        ptr->sendTime,
        ptr->sendExtraParam,
        ptr->sendLength);

    int leavLength = NETMESSAGE_DEFAULT_SEND_BUF_SIZE  - writedLength;
    int endLength = strlen(JCQ_FORMAT_END)+1;
    
    if(ptr->sendData!=NULL || ptr->sendLength>0){
        if(leavLength< ptr->sendLength+endLength){   
            void* temp = allocMem(writedLength + ptr->sendLength + endLength);
            memcpy(temp,ptr->sendBuf,writedLength);
            freeMem(&(ptr->sendBuf));
            ptr->sendBuf = temp;
        } 
        memcpy(ptr->sendBuf+writedLength,ptr->sendData,ptr->sendLength);
        writedLength +=ptr->sendLength;
    }else{
        if(leavLength < endLength){
            void* temp = allocMem(writedLength + endLength);
            memcpy(temp,ptr->sendBuf,writedLength);
            freeMem(&(ptr->sendBuf));
            ptr->sendBuf = temp;
        }
    }
    memcpy(ptr->sendBuf+writedLength,JCQ_FORMAT_END,endLength);
    writedLength +=endLength;
    ptr->sendBufLength =  writedLength;
    return PARSER_SUCCESS;

}
int isCMDMessage(NetMessagePtr ptr,void* buf,int length){
    if(ptr==NULL){
        return 0;
    }
    if(isSameString(ptr->protocolType, NETMESSAGE_TYPE_CMD)){
        return 1;
    }
    return 0;
}
int getCMDParamNameAndData(char* ptr,ParamPtr param, int length){

}
int isCMDMessageData(char * buf,int length){
    if(length<5){
        return 0;
    }
    if(!memcmp(buf,"data:",5)){
        return 1;
    }
    return 0;
}
int isCMDMessageEnd(char* buf,int length){
    
}
