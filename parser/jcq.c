#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net_message.h"
#include "jcq.h"
#include "../util/util.h"
int parserJCQMessage(NetMessagePtr ptr,void * buf, int length){
    if(ptr == NULL|| buf == NULL || length <0){
        return PARSER_ERROR_PARAM_ERROR;
    }
    if(length == 0){
        return PARSER_SUCCESS;
	}
	void * temp = NULL;
	int realLength = 0;
	if(ptr->lastParserBuf != NULL && ptr->lastParserBufLength >0)
	{
		
		
     	temp = (void*)allocMem(ptr->lastParserBufLength + length);
     	memcpy(temp,ptr->lastParserBuf,ptr->lastParserBufLength);
     	memcpy(temp,buf,length);
		realLength =length+ ptr->lastParserBufLength;
		free(ptr->lastParserBuf);
		ptr->lastParserBufLength =0;
		ptr->lastParserBuf =NULL;
	}else{
		temp = allocMem(length);
		memcpy(temp,buf,length);
		realLength = length;	
	}
    
    
    Param param;
	int readLength = 0,result =0,leavLength =0,error = PARSER_SUCCESS;
    while(readLength <realLength && !error){

        leavLength = realLength -readLength;
		switch(ptr->readState)
        {
            case NETMESSAGE_READSTATE_WAIT:
                ptr->readState = NETMESSAGE_READSTATE_PARAM;
            case NETMESSAGE_READSTATE_PARAM:
                param.paramName = NULL;
                param.paramData = NULL;

				if(isJCQMessageData(temp+readLength,leavLength)){
						ptr->readState = NETMESSAGE_READSTATE_DATA;
						//need jump to data process
						break;
					}
					
                
               result = getJCQParamNameAndData(temp+readLength,&param,leavLength);
                if(result == PARSER_ERROR_DATA_NEED_MORE){
                    // we need more data to parser the param so
					// just save it in buffer
					setLastParamBuffer(ptr,temp+readLength,leavLength);
                    readLength += leavLength;

					error = PARSER_ERROR_DATA_NEED_MORE;
                }else if(result == PARSER_SUCCESS){
                    readLength += param.readLength;
					setNetMessageParam(ptr,param.paramName,param.paramData);
					break;
                }else if(result == PARSER_ERROR_PARAM_ERROR){
				//save
					setLastParamBuffer(ptr,temp+readLength,leavLength);
                    readLength += leavLength;
					error= PARSER_ERROR_FORMAT_ERROR;
                }
                break;
            case NETMESSAGE_READSTATE_DATA:
				// start point of data it means the begin is "data:"
				if(ptr->length ==0){
					//check
					if(isJCQMessageData(temp+readLength,leavLength)){
						
                    	ptr->readState = NETMESSAGE_READSTATE_FINISH;
						readLength += 5;
						leavLength-= 5;
					}else{
					//could not come here;?
					
						setLastParamBuffer(ptr,temp+readLength,leavLength);
                    	readLength += leavLength;
						error= PARSER_ERROR_FORMAT_ERROR;
					
					}
					break;

				}else if(ptr->length<0)
				
				
				{
					
						setLastParamBuffer(ptr,temp+readLength,leavLength);
                    	readLength += leavLength;
						error= PARSER_ERROR_FORMAT_ERROR;
						break;
				}else	{
				
					if(ptr->data == NULL){
						if(isJCQMessageData(temp+readLength,leavLength)){
							ptr->data= allocMem(ptr->length);
							ptr->readLength =0;
							readLength+=5;
							leavLength-=5;
						}
					}

					if(ptr->readLength  < ptr->length){ 
                    	int copyLength = (ptr->readLength + leavLength<= ptr->length) ? leavLength:ptr->length - ptr->readLength;
                    	memcpy(ptr->data + ptr->readLength, temp+readLength, copyLength);
                    	ptr->readLength += copyLength;
                    	readLength += copyLength;
						leavLength -= copyLength;

                	}
				    if(ptr->readLength < ptr->length){
						// need more data;
						error= PARSER_ERROR_DATA_NEED_MORE;
						break;
					}
					else if (ptr->readLength == ptr->length) {
					
                   		ptr->readState = NETMESSAGE_READSTATE_FINISH;
						break;
					}else if(ptr->readLength > ptr->length){
				
						setLastParamBuffer(ptr,temp+readLength,leavLength);
                    	readLength += leavLength;
						error= PARSER_ERROR_DATA_OUT_LINE;
               	}
				}
                break;
            case NETMESSAGE_READSTATE_FINISH:
				//end with \r\n\r\n
				if(isJCQMessageEnd(temp+readLength,leavLength)){
					readLength+=4;
					leavLength-=4;
                    if(leavLength != 0){
                        setLastParamBuffer(ptr,temp+readLength,leavLength);
                        readLength +=leavLength;
                        leavLength = 0;
                    }
                    break;
				}else{
                    setLastParamBuffer(ptr,temp+readLength,leavLength);
                    readLength += leavLength;
                    error = PARSER_ERROR_FORMAT_ERROR;
                }
                break;
            default:
                ptr->readState = NETMESSAGE_READSTATE_WAIT;
                break;
        }
    }
    free(temp);
    return error;
        
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
            param->readLength = i+2;
            param->paramName = ptr;
            param->paramData = ptr+split+1;
            return PARSER_SUCCESS;
        }
        return PARSER_ERROR_DATA_NEED_MORE;
    }
    return PARSER_ERROR_DATA_NEED_MORE;
}
int isJCQMessageData(char * buf,int length){
    if(length<5){
        return 0;
    }
    if(!strncmp(buf,"data:",5)){
        return 1;
    }
    return 0;
}
int isJCQMessageEnd(char* buf,int length){
    if(length<4){
        return 0;
    }
    if(!strncmp(buf,"\r\n\r\n",4)){
        return 1;
    }
    return 0;   
}
int setLastParamBuffer(NetMessagePtr ptr,void * buf,int length){
	if(ptr == NULL || buf == NULL || length <0){
		return PARSER_ERROR_PARAM_ERROR;
	}
	ptr->lastParserBuf = allocMem(length);
    memcpy(ptr->lastParserBuf,buf,length);
    ptr->lastParserBufLength = length;
	return PARSER_SUCCESS;
}
int isJCQMessage(NetMessagePtr ptr,void* buf,length){
    if(ptr == NULL || buf == NULL || length<0){
        return 0;
    }
    if(ptr->lastParserBuf!=NULL&& ptr->lastParserBufLength>0){
        // has data in buffer
        if(ptr->lastParserBufLength > 3){

        }
        if(!strncmp(ptr->lastParserBuf,"jcq",3)){
            ptr->protocolType = allocString("jcq");
            //
            return 1;
        }
    }else{

    }
}
