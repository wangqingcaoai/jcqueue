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
#define USER_GROUP_USER 2
#define USER_GROUP_DEFAULT 3
#define USER_DEFAULT_UPDATE_TIME 3600*24

#define USER_MAX_USER_NAME_SIZE 60

#define USER_MAX_SECRET_KEY_SIZE 256

#define USER_MAX_PASSWORD_SIZE 128

#define USER_SUCCESS 0
#define USER_ERROR_PARAM_ERROR 1
#define USER_ERROR_USER_NOT_FOUND 2
#define USER_ERROR_USER_EXIST 3
#define USER_ERROR_ADD_FAILED 4
#define USER_ERROR_SECRET_FAILED 5
#define USER_ERROR_PASSWD_FAILED 6
#define USER_ERROR_PRIVILEGE_FAILED 7
#define USER_LIST_BUF_SIZE 1024
typedef struct Channel * ChannelPtr;
typedef struct User
{
    int userId;//用户id
    char* userName;//用户名
    char* userSecretKey;//鉴权key
    char* userPassword;//用户密码
    int privilege;//权限
    int group;
    int64 keyUpdateTime;//鉴权key 更新时间

    ListPtr channels;//当前开辟的连接通道
	long storePosition;
}User,*UserPtr;
typedef struct UserStore
{
    int userId;
    long userName;
    long userSecretKey;
    long userPassword;
    int privilege;
    int group;
    int64 keyUpdateTime;
    long channels;
}UserStore,UserStorePtr;
UserPtr buildUser(const char* userName, const char* userPassword);
int setUserPassWord(UserPtr  ptr,const char* userPassword);
int setUserSecretKey(UserPtr ptr,const char* userSecretKey);
int setUserPrivilege(UserPtr ptr,const int privilege);
int freeUser(UserPtr *);
int addUser(ListPtr userList,const char* userName,const char* userPassword ,int privilege,int group);
int checkUser(UserPtr userPtr,const char* userSecretKey,const char* userPassword);
UserPtr findUser(ListPtr userList,const char* userName);
int delUser(ListPtr userList,const char* userName);
int checkPrivilege(UserPtr user,int privilege);
int addChannel(UserPtr userPtr,ChannelPtr ptr );
int isUser(UserPtr userPtr,const char* userName);
int isUserByStorePosition(UserPtr userPtr,long * storePosition);
ListPtr restoreUsers(long storePosition);
long storeUsers(ListPtr userList);
long storeUser(UserPtr user);
UserPtr restoreUser(long storePosition);
int tickUser(ListPtr userList);
UserPtr findUserByStorePosition(ListPtr userList,long);
//更新时备份功能
#endif
