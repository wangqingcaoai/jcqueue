#ifndef PARSER_H
#define PARSER_H 
#include "net_message.h"
int readMessage(int fd,NetMessagePtr ptr);
int writeMessage(int fd	,NetMessagePtr ptr);

#endif
