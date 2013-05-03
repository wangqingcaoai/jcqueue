#ifndef AQ_USER_H
#define AQ_USER_H 
#include "../data/list.h"
#include "../util/type.h"
#include "channel.h"
#define USER_PRIVILEGE_ADMIN 1
#define USER_PRIVILEGE_BUILDER 2
#define USER_PRIVILEGE_CUSTOMER 3
#define USER_PRIVILEGE_DEFAULT 4
#define USER_GROUP_ADMIN 1
#define USER_GROUP_DEFAULT 2
#define USER_GROUP_USER 3
#define USER_DEFAULT_UPDATE_TIME 3600*24

#define USER_SUCCESS 0
#define USER_ERROR_PARAM_ERROR 1
#define USER_ERROR_USER_NOT_FOUND 2
#define USER_ERROR_USER_EXIST 3
#define USER_ERROR_ADD_FAILED 4
#define USER_ERROR_SECRET_FAILED 5
#define USER_ERROR_PASSWD_FAILED 6
#define USER_ERROR_PRIVILEGE_FAILED 7

typedef struct User
{
    int userId;//用户id
    char* userName;//用户名
    char* userSecretKey;//鉴权key
    char* userPassWord;//用户密码
    int privilege;//权限
    int group;
    int64 keyUpdateTime;//鉴权key 更新时间

    ListPtr channels;//当前开辟的连接通道
}User,*UserPtr;
UserPtr buildUser(const char* userName, const char* userPassWord);
int setUserPassWord(UserPtr  ptr,const char* userPassWord);
int setUserSecretKey(UserPtr ptr,const char* userSecretKey);
int setUserPrivilege(UserPtr ptr,const int privilege);
int freeUser(UserPtr *);
int addUser(ListPtr userList,const char* userName,const char* userPassWord ,int privilege,int group);
int checkUser(UserPtr userPtr,const char* userSecretKey,const char* userPassWord);
UserPtr findUser(ListPtr userList,const char* userName);
int delUser(ListPtr userList,const char* userName);
int checkPrivilege(UserPtr user,int privilege);
int addChannel(UserPtr userPtr,ChannelPtr ptr );
int isUser(UserPtr userPtr,const char* userName);
#endif