#ifndef PARSER_H
#define PARSER_H 
#include "net_message.h"
#define PARSER_READ_BUF_SIZE  1024
#define PARSER_SUCCESS 0
#define PARSER_ERROR_PARAM_ERROR 1
#define PARSER_ERROR_DATA_OUT_LINE 2
#define PARSER_ERROR_DATA_NEED_MORE 3
#define PARSER_ERROR_FORMAT_ERROR 4

int parserNetMessage(NetMessagePtr ptr,void* buf,int length);
int reparserNetMessage(NetMessagePtr ptr);


#endif
