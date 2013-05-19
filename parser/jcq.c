#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parserJCQMessage(NetMessage ptr,void * buf, int length){
	if(ptr == NULL|| buf == NULL || length <0){
		return PARSER_ERROR_PARAM_ERROR;
	}
    if(ptr->readState == NETMESSAGE_READSTATE_WAIT ){
	
	}else if(ptr->readState == NETMESSAGE_READSTATE_PARAM){
	
	}else if(ptr->readState == NETMESSAGE_READSTATE_DATA){
		if(ptr->readLength + length <= ptr->length){
		    memcpy(ptr->data + ptr->readLength,	
		}
	}else if(ptr->readState == NETMESSAGE_READSTATE_FINISH){
	
	
	}else if(ptr->readState == NETMESSAGE_READSTATE_PARAM){
	
	}	
}
int buildJCQMessage(NetMessage ptr,void * buf, int length);
