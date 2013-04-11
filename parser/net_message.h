#ifndef PARSER_MESSAGE
#define PARSER_MESSAGE 
/**
 * 对解析后的消息进行定义
 */
typedef struct NetMessage
{
    char* cmd ;
    char* ip ;
    char* user;
    char* password;
    char* key;
    void* data;
    char* clientVersion;
    char* clientType;
    int length;
    int timestamp;
}NetMessage,* NetMessagePtr;

#endif