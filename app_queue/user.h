#ifndef AQ_USER_H
#define AQ_USER_H 
typedef struct User
{
    int userId;//用户id
    char* userName;//用户名
    char* userSecretKey;//鉴权key
    char* userPassWord;//用户密码
    List channels;//当前开辟的连接通道
}User,*UserPtr;
#endif