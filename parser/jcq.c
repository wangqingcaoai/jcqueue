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
	void * temp = NULL;
	int realLength = 0;
	if(ptr->lastParserBuf != NULL && ptr->lastParserBufLength >0)
	{
		
		
     	temp = allocMem(ptr->lastParserBufLength + length);
     	memcpy(temp,ptr->lastParserBuf,ptr->lastParserBufLength);
     	memcpy(temp,buf,length);
		realLength =length+ ptr->lastParserBufLength;
		free(ptr->lastParserBuf);
		ptr->lastParserBufLength =0;
		ptr->lastParserBuf =NULL;
	}else{
		temp = allocMem(length);
		memcpy(temp,buf,length)
		realLength = length;
		
	}
	int readLength = 0,result =0,leavLength =0,error = 0;
    while(readLength <realLength && !error){

        leavLength = realLength -readLength;
		switch(ptr->readState)
        {
            case NETMESSAGE_READSTATE_WAIT:
                ptr->readState = NETMESSAGE_READSTATE_PARAM;
            case NETMESSAGE_READSTATE_PARAM:
                Param param;l
                param.paramName = NULL;
                param.paramData = NULL;

				if(isReadData(temp,leavLength)){
						ptr->readState = NETMESSAGE_READSTATE_DATA;
						//need jump to data process
						break;
					}
					
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
					if(isReadData(temp+readLength,leavLength)){
						
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
						if(isReadData(temp+readLength,leavLength)){
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
				if(isMessageEnd(ptr,temp+readLength,leavLength)){
					readLength+=4;
					leavLength-=4;

				
				}
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
int isReadData(char * buf,int length){

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
