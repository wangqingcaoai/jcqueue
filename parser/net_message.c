#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../util/type.h"
#include "../util/util.h"
#include "net_message.h"
int setNetMessageSendData(NetMessagePtr ptr,int errcode,char* sendCmd,void* sendData,int datalength){
    if(ptr==NULL){
        return NETMESSAGE_ERROR_PARAM_ERROR;
    }
    ptr->sendErrcode = errcode;
    if(ptr->sendCmd!=NULL){
        freeString(&(ptr->sendCmd));
    }
    ptr->sendCmd = allocString(sendCmd);
    
    if(ptr->sendData!=NULL){
        freeMem(&(ptr->sendData));
        
    }
    if(sendData!=NULL && datalength>0){
        ptr->sendData = allocMem(datalength);
        memcpy(ptr->sendData,sendData,datalength);
        ptr->sendLength = datalength;
    }else{
        ptr->sendData = 0;
        ptr->sendLength =0;
    }
    return NETMESSAGE_SUCCESS;
}

int setNetMessageSendUser(NetMessagePtr ptr,UserPtr uptr){
    if(ptr == NULL || uptr == NULL){
        return NETMESSAGE_ERROR_PARAM_ERROR;
    }
    if(ptr->currentUser!=NULL){
        freeString(&(ptr->currentUser));
    }
    if(ptr->currentUserKey !=NULL){
        freeString(&(ptr->currentUserKey));
    }

    if(ptr->currentPassword !=NULL){
        freeString(&(ptr->currentPassword));
    }
    ptr->currentUser = allocString(uptr->userName);
    ptr->currentUserKey = allocString(uptr->userSecretKey);
    ptr->currentPassword = allocString(uptr->userPassword);
    return NETMESSAGE_SUCCESS;
}
NetMessagePtr buildNetMessage(){
    static int64 id;
    NetMessagePtr ptr= allocMem(sizeof(NetMessage));
    if(ptr == NULL){
        return NULL;
    }
    id++;
    ptr->id = id; 
    ptr->cmd = NULL;
    ptr->user = NULL;
    ptr->password = NULL;
    ptr->key = NULL;
    ptr->extraParam = NULL;
    ptr->data = NULL;
    ptr->length = 0;

    ptr->version = NULL;
    ptr->protocolType = NULL;
    ptr->timestamp = 0;
    ptr->errcode = 0;
    
    ptr->host = NULL;
    ptr->port = NULL;
    ptr->lastParserBuf =NULL;
    ptr->lastParserBufLength =0;
    ptr->readLength = 0;
    ptr->readState = NETMESSAGE_READSTATE_WAIT;
    

    ptr->sendCmd = NULL;
    ptr->currentUserKey = NULL;
    ptr->currentUser = NULL;
    ptr->currentPassword = NULL;
    ptr->sendExtraParam = NULL;
    ptr->sendData = NULL;
    ptr->sendLength =0;
    ptr->sendTime = 0;

    ptr->sendState = NETMESSAGE_WRITESTATE_WAIT;
    ptr->sendBuf = NULL;
    ptr->sendBufLength =0;
    ptr->writedLength = 0;
    
    return ptr;
}
int setNetMessageError(NetMessagePtr ptr,int errcode,char* errorMessage,...){
    if(ptr == NULL || errcode <0 || errorMessage == NULL){
        return NETMESSAGE_ERROR_PARAM_ERROR;
    }
    ptr->errcode = errcode;
    ptr->sendData = allocMem(NETMESSAGE_DEFAULT_ERROR_BUF_SIZE);
    va_list va;
    va_start(va,errorMessage);
    vsnprintf(ptr->sendData,NETMESSAGE_DEFAULT_ERROR_BUF_SIZE,errorMessage,va);
    va_end(va);
    ptr->sendLength = strlen(ptr->sendData);
    return NETMESSAGE_SUCCESS;
}
char * getExtraParam(NetMessagePtr ptr,const char* paramName){
    if(ptr == NULL){
        return NULL;
    }
    return findParamValueByString(ptr->extraParam,paramName);
}

int setExtraParam(NetMessagePtr ptr,const char* paramName,const char* paramValue){
    if(ptr==NULL){
        return NETMESSAGE_ERROR_PARAM_ERROR;
    }
    return setParamByValue(&(ptr->extraParam),paramName,paramValue);
}

char * getSendExtraParam(NetMessagePtr ptr,const char* paramName){
    if(ptr == NULL){
        return NULL;
    }
    return findParamValueByString(ptr->sendExtraParam,paramName);
}
static char* findParamValueByString(const char* string,const char* paramName ){
    if(string == NULL || paramName == NULL ){
        return NULL;
    }else{
        // this may have some error; need to check
        char*value = strstr(string,paramName);
        if(value == NULL){
            return NULL;
        }
        value+=strlen(paramName);
        if(value[0]=='['){
            int i=1;
            char* start = value+1;
            while(value[i]!='\0'&& value[i]!=']'){
                i++;
            }
            if(value[i]==']'){
                char* result= allocMem(i);
                memcpy(result,start,i-1);
                result[i-1]='\0'; 
                return result;
            }
        }
        return NULL;
    }
}
static int setParamByValue(char** pptr,const char* paramName,const char* paramValue){
    if(pptr == NULL ||paramName == NULL ||paramValue == NULL){
        return NETMESSAGE_ERROR_PARAM_ERROR;
    }
    if((*pptr) == NULL){
        int paramLength = strlen(paramName),valueLength=strlen(paramValue); 
        (*pptr) = allocMem(paramLength+valueLength+3);
        memcpy((*pptr),paramName,paramLength);
        (*pptr)[paramLength] = '[';
        memcpy((*pptr)+paramLength+1,paramValue,valueLength);
        (*pptr)[paramLength+valueLength+1] = ']';
        (*pptr)[paramLength+valueLength+2] = '\0';
        return NETMESSAGE_SUCCESS;
    }else{
        char*value = strstr((*pptr),paramName);
        if(value == NULL){
            //just jump to build new     
        }else{
            value+=strlen(paramName);
            if(value[0]=='['){
                int i=1;
                char* start = value+1;
                while(value[i]!='\0'&& value[i]!=']'){
                    i++;
                }
                if(value[i]==']'){
                    //change 
                    int extraLength = strlen((*pptr));
                    int valueLength = strlen(paramValue);
                    int size = extraLength+ valueLength - (i-1) +1;
                    int startLength =start-(*pptr);
                    int endLength = extraLength-startLength-(i-1);
                    char * temp = allocMem(size);
                    memcpy(temp,(*pptr),startLength);
                    memcpy(temp+startLength,paramValue,valueLength);
                    memcpy(temp+startLength+valueLength,value+i,endLength);
                    temp[size-1] = '\0';
                    value = NULL;
                    start = NULL;
                    freeString(&(*pptr));
                    (*pptr) = NULL;
                    (*pptr) = temp;
                    temp =NULL;
                    return NETMESSAGE_SUCCESS;
                }
                //other is build new
            }
            value = NULL;
        }
        // build new
        int extraLength = strlen((*pptr)),paramLength=strlen(paramName),valueLength = strlen(paramValue);
        int allocSize = extraLength+paramLength+valueLength+3;
        char* temp = allocMem(allocSize);
        memcpy(temp,(*pptr),extraLength);
        memcpy(temp+extraLength,paramName,paramLength);
        temp[extraLength+paramLength] = '[';
        memcpy(temp+extraLength+paramLength+1,paramValue,valueLength);
        temp[allocSize-2] = ']';
        temp[allocSize-1] = '\0';
        freeString(&(*pptr));
        (*pptr) = temp;
        temp =NULL;
        return NETMESSAGE_SUCCESS;
            
    }

}
int setSendExtraParam(NetMessagePtr ptr,const char* paramName,const char* paramValue){
    if(ptr == NULL ||paramName == NULL ||paramValue == NULL){
        return NETMESSAGE_ERROR_PARAM_ERROR;
    }
    return setParamByValue(&(ptr->sendExtraParam),paramName,paramValue);
    
}

int setNetMessageParam(NetMessagePtr ptr,const char* paramName,const char* paramValue){
    if(ptr == NULL || paramName==NULL || paramValue == NULL){
        return NETMESSAGE_ERROR_PARAM_ERROR;
    }
    if(!strcmp(paramName,"cmd")){
        if(ptr->cmd!=NULL){
            freeString(&(ptr->cmd));
        }
        ptr->cmd = allocString(paramValue);
    }else if(!strcmp(paramName,"user")){
        if(ptr->user!=NULL){
            freeString(&(ptr->user));
        }
        ptr->user = allocString(paramValue);
    }else if(!strcmp(paramName,"password")){
        if(ptr->password!=NULL){
            freeString(&(ptr->password));
        }
        ptr->password = allocString(paramValue);
    }else if(!strcmp(paramName,"key")){
        if(ptr->key!=NULL){
            freeString(&(ptr->key));
        }
        ptr->key = allocString(paramValue);
    }else if(!strcmp(paramName,"extraParam")){
        if(ptr->extraParam!=NULL){
            freeString(&(ptr->extraParam));
        }
        ptr->extraParam = allocString(paramValue);
    }else if(!strcmp(paramName,"length")){
        ptr->length = atoi(paramValue);
    }else if(!strcmp(paramName,"version")){
        if(ptr->version!=NULL){
            freeString(&(ptr->version));
        }
        ptr->version = allocString(paramValue);
    }else if(!strcmp(paramName,"protocolType")){
        if(ptr->protocolType!=NULL){
            freeString(&(ptr->protocolType));
        }
        ptr->protocolType = allocString(paramValue);
    }else if(!strcmp(paramName,"timestamp")){
        ptr->timestamp = atoll(paramValue);
    }else if(!strcmp(paramName,"errcode")){
        ptr->errcode = atoi(paramValue);
    }else if(!strcmp(paramName,"host")){
        if(ptr->host!=NULL){
            freeString(&(ptr->host));
        }
        ptr->host = allocString(paramValue);
    }else if(!strcmp(paramName,"port")){
        if(ptr->port!=NULL){
            freeString(&(ptr->port));
        }
        ptr->port = allocString(paramValue);
    }else if(!strcmp(paramName,"sendCmd")){
        if(ptr->sendCmd!=NULL){
            freeString(&(ptr->sendCmd));
        }
        ptr->sendCmd = allocString(paramValue);
    }else if(!strcmp(paramName,"currentUserKey")){
        if(ptr->currentUserKey!=NULL){
            freeString(&(ptr->currentUserKey));
        }
        ptr->currentUserKey = allocString(paramValue);
    }else if(!strcmp(paramName,"currentUser")){
        if(ptr->currentUser!=NULL){
            freeString(&(ptr->currentUser));
        }
        ptr->currentUser = allocString(paramValue);
    }else if(!strcmp(paramName,"currentPassword")){
        if(ptr->currentPassword!=NULL){
            freeString(&(ptr->currentPassword));
        }
        ptr->currentPassword = allocString(paramValue);
    }else if(!strcmp(paramName,"sendExtraParam")){
        if(ptr->sendExtraParam!=NULL){
            freeString(&(ptr->sendExtraParam));
        }
        ptr->sendExtraParam = allocString(paramValue);
    }else if(!strcmp(paramName,"sendLength")){
        ptr->sendLength = atoi(paramValue);
    }else if(!strcmp(paramName,"sendTime")){
        ptr->sendTime = atoll(paramValue);
    }else if(!strcmp(paramName,"sendState")){
        
        ptr->sendState = atoi(paramValue);
    }else if(!strcmp(paramName,"writedLength")){
        ptr->writedLength = atoi(paramValue);
    }else{
        return NETMESSAGE_ERROR_UNKNOW_PARAM;
    }
    return NETMESSAGE_SUCCESS;
}
 int displayNetMessage(NetMessagePtr ptr){
    if(ptr == NULL){
        printf("NULL\n");
    }

    printf("%s:%s\n", "cmd",ptr->cmd?ptr->cmd:"NULL");
    printf("%s:%s\n", "host",ptr->host?ptr->host:"NULL" );
    printf("%s:%s\n", "port",ptr->port ?ptr->port:"NULL");
    printf("%s:%s\n", "user",ptr->user ?ptr->user:"NULL");
    printf("%s:%s\n", "password",ptr->password ?ptr->password:"NULL");
    printf("%s:%s\n", "key",ptr->key ?ptr->key:"NULL");
    printf("%s:%s\n", "extraParam",ptr->extraParam ?ptr->extraParam:"NULL");
    if(ptr->data!=NULL&& ptr->length>0){
        char* buf = allocMem(ptr->length+1);
        memcpy(buf,ptr->data,ptr->length);
        buf[ptr->length] = '\0';
        printf("%s:<buf>%s</buf>\n","data",buf );
        freeString(&buf);    
    }else{
        printf("%s:NULL\n", "data");    
    }
    printf("%s:%d\n", "length",ptr->length );
    if(ptr->lastParserBuf!=NULL&& ptr->lastParserBufLength>0){
        char* buf = allocMem(ptr->lastParserBufLength+1);
        memcpy(buf,ptr->lastParserBuf,ptr->lastParserBufLength);
        buf[ptr->lastParserBufLength] = '\0';
        printf("%s:<buf>%s</buf>\n","lastParserBuf",buf );
        freeString(&buf);    
    }else{
        printf("%s:NULL\n", "lastParserBuf");    
    }
    
    
    printf("%s:%d\n", "lastParserBufLength",ptr->lastParserBufLength );
    printf("%s:%d\n", "readLength",ptr->readLength );
    printf("%s:%d\n", "readState",ptr->readState);
    printf("%s:%s\n", "version",ptr->version ?ptr->version:"NULL");
    printf("%s:%s\n", "protocolType",ptr->protocolType?ptr->protocolType:"NULL") ;
    printf("%s:%" PRId64 "\n", "timestamp",ptr->timestamp );
    printf("%s:%d\n", "errcode",ptr->errcode );
    printf("%s:%s\n", "sendCmd",ptr->sendCmd ?ptr->sendCmd:"NULL");
    printf("%s:%s\n", "sendExtraParam",ptr->sendExtraParam ?ptr->sendExtraParam:"NULL");
    //printf("%s:%s\n", "sendData",ptr->sendData ?(char*)ptr->sendData:"NULL");
    if(ptr->sendData!=NULL&& ptr->sendLength>0){
        char* buf = allocMem(ptr->sendLength+1);
        memcpy(buf,ptr->sendData,ptr->sendLength);
        buf[ptr->sendLength] = '\0';
        printf("%s:<buf>%s</buf>\n","sendData",buf );
        freeString(&buf);    
    }else{
        printf("%s:NULL\n", "sendData");    
    }
    printf("%s:%d\n", "sendLength",ptr->sendLength );
    printf("%s:%d\n", "sendState",ptr->sendState );
    printf("%s:%s\n", "sendBuf",ptr->sendBuf ?(char*)ptr->sendBuf:"NULL");
    if(ptr->sendBuf!=NULL&& ptr->sendBufLength>0){
        char* buf = allocMem(ptr->sendBufLength+1);
        memcpy(buf,ptr->sendBuf,ptr->sendBufLength);
        buf[ptr->sendBufLength] = '\0';
        printf("%s:<buf>%s</buf>\n","sendBuf",buf );
        freeString(&buf);    
    }else{
        printf("%s:NULL\n", "sendBuf");    
    }
    printf("%s:%d\n", "sendBufLength",ptr->sendBufLength );
    printf("%s:%d\n", "writedLength",ptr->writedLength );
    printf("%s:%s\n", "currentUserKey",ptr->currentUserKey ?ptr->currentUserKey:"NULL");
    printf("%s:%s\n", "currentUser",ptr->currentUser ?ptr->currentUser:"NULL");
    printf("%s:%s\n", "currentPassword",ptr->currentPassword ?ptr->currentPassword:"NULL");
    printf("%s:%" PRId64 "\n", "sendTime",ptr->sendTime );

 }
 int freeNetMessage(NetMessagePtr* pptr){
    if(pptr == NULL){
        return NETMESSAGE_ERROR_PARAM_ERROR;
    }
    NetMessagePtr ptr = *pptr;
    if(ptr == NULL){
        return NETMESSAGE_ERROR_PARAM_ERROR;
    }
    freeString(&(ptr->cmd ));
    freeString(&(ptr->host ));
    freeString(&(ptr->port ));
    freeString(&(ptr->user ));
    freeString(&(ptr->password ));
    freeString(&(ptr->key ));
    freeString(&(ptr->extraParam ));
    freeMem(&(ptr->data));
    ptr->length = 0;
    freeMem(&(ptr->lastParserBuf)); 
    ptr->lastParserBufLength =0;
    ptr->readLength = 0;
    ptr->readState = NETMESSAGE_READSTATE_WAIT;

    freeString(&(ptr->version ));
    freeString(&(ptr->protocolType ));
    ptr->timestamp = 0;
    ptr->errcode = 0;
    freeString(&(ptr->sendCmd ));
    freeString(&(ptr->sendExtraParam ));
    freeMem(&(ptr->sendData ));
    ptr->sendLength =0;
    ptr->sendState = NETMESSAGE_WRITESTATE_WAIT;
    freeMem(&(ptr->sendBuf));
    ptr->sendBufLength =0;
    ptr->writedLength = 0;
    freeString(&(ptr->currentUserKey ));
    freeString(&(ptr->currentUser ));
    freeString(&(ptr->currentPassword ));
    ptr->sendTime = 0;
    freeMem((void**)&ptr);
    *pptr = NULL;
    return NETMESSAGE_SUCCESS;  
 }

int isExtraParamFormatRight(char*buf,int length){
    if(buf == NULL || length<0){
        return 0;
    }
    if(isEmptyString(buf)){
        return 1;
    }
    //param[value]
    char* temp = allocMem(length+1);
    memcpy(temp,buf,length);
    temp[length]='\0';
    char *ptr = temp,*key,*value,*split1,*split2,*more;
    int result = 0;
    while(ptr!=NULL){
        key = ptr;
        split1 = strstr(key,"[");
        if(split1 == NULL){
            result = 0;
            break;
        }
        if(split1 == key){
            result = 0;
            break;
        }
        split1[0]='\0';
        //printf("%s\n",key );
        more = strnstr(key,"]",split1-key);//between start and first end of '[' has other ']'
        if(more!=NULL){
            result = 0;
            break;
        }
        value = ++split1;
        if(value[0]=='\0'){
            result = 0;
            break;
        }
        split2 = strstr(value,"]");
        if(split2 == NULL){
            result = 0;
            break;
        }
        split2[0]='\0';
       // printf("%s\n",value );
        more = strnstr(value,"[",split2-value);
        if(more!=NULL){
            result =0;
            break;
        }
        ptr = ++split2;
        if(ptr[0]=='\0'){
            result =1;
            break;
        }
    }
    freeMem((void*)&temp);
    return result;
}
 

int setNetMessageJustSendData(NetMessagePtr ptr,void *data,int length){
    if(ptr == NULL || data == NULL ||length <0){
        return NETMESSAGE_ERROR_PARAM_ERROR;
    }
    if(ptr->sendData!=NULL){
        freeMem(&ptr->sendData);
    }
    ptr->sendData = allocMem(length);
    ptr->sendLength = length;
    memcpy(ptr->sendData,data,length);
    return NETMESSAGE_SUCCESS;
}
int setNetMessageSendCMDData(NetMessagePtr ptr,int errcode,char* sendCmd){
    if(ptr==NULL){
        return NETMESSAGE_ERROR_PARAM_ERROR;
    }
    ptr->sendErrcode = errcode;
    if(ptr->sendCmd!=NULL){
        freeString(&(ptr->sendCmd));
    }
    ptr->sendCmd = allocString(sendCmd);
    
}
int setNetMessageSendState(NetMessagePtr ptr,int state){
    if(ptr == NULL){
        return NETMESSAGE_ERROR_PARAM_ERROR;
    }
    ptr->sendState = state;
}