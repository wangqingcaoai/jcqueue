#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net_message.h"
#include "jcq.h"
#include "parser.h"
#include "../util/util.h"
int parserJCQMessage(NetMessagePtr ptr,void * buf, int length){
    if(ptr == NULL|| length <0||(buf==NULL && length >0)){
        return PARSER_ERROR_PARAM_ERROR;
    }
	void * temp = NULL;
	int realLength = 0;
    //if have lastparser 
	if(ptr->lastParserBuf != NULL && ptr->lastParserBufLength >0)
	{
		
		//length must >=0
     	temp = (void*)allocMem(ptr->lastParserBufLength + length);
        //copy old data
     	memcpy(temp,ptr->lastParserBuf,ptr->lastParserBufLength);
        //copy buf data
        if(length !=0&& buf!=NULL){
            memcpy(temp+ptr->lastParserBufLength,buf,length);    
        }
     	
		realLength =length+ ptr->lastParserBufLength;
        //free lastParser 
		freeMem((void**)&(ptr->lastParserBuf));
		ptr->lastParserBufLength =0;
		ptr->lastParserBuf =NULL;
	}else{
        if(length == 0){
            return PARSER_ERROR_DATA_NEED_MORE;
        }
        if(buf == NULL){
            return PARSER_ERROR_PARAM_ERROR;
        }
		temp = allocMem(length);
		memcpy(temp,buf,length);
		realLength = length;	
	}
    
    
    Param param;
    int needMoreData = 0;
    char* startFlag = "jcq 1.0\r\n";
    char * protocol1 = NULL;
    int setState;
	int readLength = 0,result =0,leavLength =0,error = PARSER_SUCCESS;
    while(readLength <realLength && !error &&ptr->readState != NETMESSAGE_READSTATE_FINISH){
        leavLength = realLength -readLength;
		switch(ptr->readState)
        {
            //IT MEANS A NEW NETMESSAGE 
            case NETMESSAGE_READSTATE_WAIT:
                
                // clear last error format data
                
                if(leavLength < strlen(startFlag)){
                    needMoreData = 1;
                }else{
                    protocol1 = strnstr(temp+readLength,startFlag,leavLength);
                    if(protocol1 == NULL){
                        needMoreData = 1;
                    }else{
                        ptr->readState = NETMESSAGE_READSTATE_PARAM;
                        readLength += ((void*)protocol1 -(temp+readLength))+strlen(startFlag);
                        break; 
                    }    
                }
                if(needMoreData){
                    setLastParamBuffer(ptr,temp+readLength,leavLength);
                    readLength += leavLength;
                    error = PARSER_ERROR_DATA_NEED_MORE;
                    break;
                }
            case NETMESSAGE_READSTATE_PARAM:
                param.paramName = NULL;
                param.paramData = NULL;

				if(isJCQMessageData(temp+readLength,leavLength)){
						ptr->readState = NETMESSAGE_READSTATE_DATA;
						//need jump to data process
						ptr->readLength = 0;
                        freeMem((void**)&(ptr->data));
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
					setState = setNetMessageParam(ptr,param.paramName,param.paramData);
					if(setState!= NETMESSAGE_SUCCESS){

                        setLastParamBuffer(ptr,temp+readLength,leavLength);
                        readLength += leavLength;
                        error= PARSER_ERROR_FORMAT_ERROR;  
                    }
                    break;
                }else if(result == PARSER_ERROR_PARAM_ERROR){
				//save
					setLastParamBuffer(ptr,temp+readLength,leavLength);
                    readLength += leavLength;
					error = PARSER_ERROR_FORMAT_ERROR;
                }
                break;
            case NETMESSAGE_READSTATE_DATA:
				// start point of data it means the begin is "data:"
				if(ptr->length ==0){
					// it means that data is empty 
					if(isJCQMessageData(temp+readLength,leavLength)){
						
                    	ptr->readState = NETMESSAGE_READSTATE_END;
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
				}else{
				
					if(ptr->readLength == 0){
						if(isJCQMessageData(temp+readLength,leavLength)){
                            if(ptr->data!=NULL){
                                freeMem((void**)&(ptr->data));
                            }
							ptr->data= allocMem(ptr->length);
							ptr->readLength =0;
							readLength+=5;
							leavLength-=5;
						}else{
                            setLastParamBuffer(ptr,temp+readLength,leavLength);
                            readLength += leavLength;
                            error= PARSER_ERROR_FORMAT_ERROR;
                            break;
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
						error = PARSER_ERROR_DATA_NEED_MORE;
						break;
					}
					else if (ptr->readLength == ptr->length) {
					
                   		ptr->readState = NETMESSAGE_READSTATE_END;
						break;
					}else if(ptr->readLength > ptr->length){
				
						setLastParamBuffer(ptr,temp+readLength,leavLength);
                    	readLength += leavLength;
						error= PARSER_ERROR_DATA_OUT_LINE;
               	    }
				}
                break;
            case NETMESSAGE_READSTATE_END:
				//end with \r\n\r\n
				if(isJCQMessageEnd(temp+readLength,leavLength)){
					readLength+=4;
					leavLength-=4;
                    if(leavLength != 0){
                        setLastParamBuffer(ptr,temp+readLength,leavLength);
                        readLength +=leavLength;
                        leavLength = 0;
                    }
                    ptr->readState = NETMESSAGE_READSTATE_FINISH;
                    break;
				}else{
                    setLastParamBuffer(ptr,temp+readLength,leavLength);
                    readLength += leavLength;
                    error = PARSER_ERROR_FORMAT_ERROR;
                }
                break;
            case NETMESSAGE_READSTATE_FINISH:
                break;
            default:
                ptr->readState = NETMESSAGE_READSTATE_WAIT;
                freeMem((void**)&(ptr->data));
                ptr->length = 0;
                ptr->data = NULL;
                ptr->readLength = 0;
                break;
        }
    }
    freeMem((void**)&temp);
    return error;
        
}
int reparserJCQMessage(NetMessagePtr ptr,char * protocolType, char* version){
    if(ptr== NULL || protocolType == NULL || version == NULL){
        return PARSER_ERROR_PARAM_ERROR;
    }if(ptr->sendBuf == NULL){
        ptr->sendBuf = allocMem(NETMESSAGE_DEFAULT_SEND_BUF_SIZE);
    }ptr->sendTime = nanoseconds();
    int writedLength = snprintf(ptr->sendBuf,NETMESSAGE_DEFAULT_SEND_BUF_SIZE,JCQ_FORMAT,
        protocolType,
        version,
        ptr->sendCmd,
        ptr->currentUser,
        ptr->currentPassword,
        ptr->currentUserKey,
        ptr->sendErrcode,
        ptr->sendTime,
        ptr->sendExtraParam,
        ptr->sendLength);
    int leavLength = NETMESSAGE_DEFAULT_SEND_BUF_SIZE  -writedLength;
    int endLength = strlen(JCQ_FORMAT_END);
    
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
    if(!memcmp(buf,"data:",5)){
        return 1;
    }
    return 0;
}
int isJCQMessageEnd(char* buf,int length){
    if(length<4){
        return 0;
    }
    if(!memcmp(buf,"\r\n\r\n",4)){
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
int isJCQMessage(NetMessagePtr ptr,void* buf,int length){
    if(ptr == NULL || buf == NULL || length<0){
        return 0;
    }
    char * temp = NULL;
    int realLength = 0;
    if(ptr->lastParserBuf != NULL && ptr->lastParserBufLength >0)
    {
        
        
        temp = (char*)allocMem(ptr->lastParserBufLength + length);
        memcpy(temp,ptr->lastParserBuf,ptr->lastParserBufLength);
        memcpy(temp,buf,length);
        realLength =length+ ptr->lastParserBufLength;
    }else{
        if(length == 0){
            return 0;
        }
        temp = (char*)allocMem(length);
        memcpy(temp,buf,length);
        realLength = length;    
    }
    char* proFlag = "jcq 1.0\r\n";
    int result = 0;
    if(realLength<strlen(proFlag)){
        result =0;
    }else{
        char* protocol1 = strnstr(temp,proFlag,realLength);
        if(protocol1 == NULL){
            result = 0;
        }else{  
            ptr->protocolType = allocString("jcq");
            ptr->version = allocString("1.0");   
            result =1;
        }
    }
    free(temp);
    return result;
}
