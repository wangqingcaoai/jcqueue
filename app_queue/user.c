#include <stdio.h>
#include <stdlib.h>
#include "user.h"
#include "../util/util.h"
#include "../data/list.h"
#include "../data/store.h"
#include "../util/maxids.h"
#include "../secure/secure.h"
UserPtr buildUser(const char* userName,const char* userPassword){
    if(isEmptyString(userName)|| isEmptyString(userPassword)){
        return NULL;
    }
    UserPtr ptr = (UserPtr)allocMem(sizeof(User));
    if(ptr == NULL){
        return NULL;
    }
    ptr->userId = getUserNextId();
    ptr->userName = allocString(userName);
    ptr->userSecretKey = NULL;
    ptr->userPassword = buildMd5Key(userPassword);
    ptr->privilege = USER_PRIVILEGE_DEFAULT;
    ptr->group = USER_GROUP_DEFAULT;
    ptr->keyUpdateTime = USER_DEFAULT_UPDATE_TIME;
    ptr->channels = buildList();
    ptr->storePosition =0L;
    return ptr;
}

int setUserPassWord(UserPtr ptr, const char* userPassword){
    if(ptr == NULL){
        return USER_ERROR_PARAM_ERROR;
    }else{
        if(ptr->userPassword != NULL){
            freeString(&(ptr->userPassword));
        }
        ptr->userPassword = allocString(userPassword);
        return USER_SUCCESS;
    }

}
int setUserSecretKey(UserPtr ptr,const char* userSecretKey){
    if(ptr == NULL){
        return USER_ERROR_PARAM_ERROR;
    }else{
        if(ptr->userSecretKey != NULL){
            freeString(&(ptr->userSecretKey));
        }
        ptr->userSecretKey = allocString(userSecretKey);
        return USER_SUCCESS;
    }
}
int setUserPrivilege(UserPtr ptr,const int privilege){
    if(ptr == NULL){
        return USER_ERROR_PARAM_ERROR;
    }else{
        ptr->privilege = privilege;
        return USER_SUCCESS;
    }
}
int setUserGroup(UserPtr ptr,const int group){
    if(ptr == NULL){
        return USER_ERROR_PARAM_ERROR;
    }else{
        ptr->group = group;
        return USER_SUCCESS;
    }
}
int freeUser(UserPtr *pptr){
    if(pptr == NULL){
        return USER_ERROR_PARAM_ERROR;
    }
    UserPtr ptr = (*pptr);
    if(ptr == NULL){
        return USER_ERROR_PARAM_ERROR;
    }else{
        freeString(&(ptr->userName));
        freeString(&(ptr->userPassword));
        freeString(&(ptr->userSecretKey));
        freeList(&(ptr->channels),(Free)freeChannel);
        freeMem((void**)&ptr);
        *pptr = NULL;
        return USER_SUCCESS;      
    }
}

int addUser(ListPtr userList,const char* userName,const char* userPassword ,int privilege , int group ){
    if(userList == NULL){
        return USER_ERROR_PARAM_ERROR;
    }
    if(userName == NULL){
        return USER_ERROR_PARAM_ERROR;
    }
    UserPtr user = findUser(userList,userName);
    if(user == NULL){
        UserPtr user = buildUser(userName,userPassword);
        setUserPrivilege(user,privilege);
        setUserGroup(user,group);
        int code = insertToList(userList,user);
        if(code == 1){
            return USER_SUCCESS;
        }else{
            return USER_ERROR_ADD_FAILED;
        }
    }else{
        return USER_ERROR_USER_EXIST;
    }
    //password can be empty
    
}
int checkUser(UserPtr userPtr,const char* userSecretKey,const char* userPassword){
    if(userPtr == NULL){
        return USER_ERROR_PARAM_ERROR;
    }
    int result = USER_ERROR_SECRET_FAILED,needCheckPasswd = 0;
    if(userSecretKey == NULL){
        needCheckPasswd = 1;
        
    }else{
        if(userPtr->userSecretKey == NULL){
            needCheckPasswd = 1;
        }else if(!strcmp(userPtr->userSecretKey,userSecretKey)){
            result = USER_SUCCESS;
            needCheckPasswd = 0;
        }else{
            result = USER_ERROR_SECRET_FAILED;
            needCheckPasswd = 1;
        }
    }
    if(needCheckPasswd){
        if(userPassword == NULL){
            if(userPtr->userPassword == NULL){
                result = USER_SUCCESS;
            }else{
                result = USER_ERROR_PASSWD_FAILED;
            }
        }else{
            if(userPtr->userPassword == NULL){
                result = USER_ERROR_PASSWD_FAILED;
            }else if(!strcmp(userPtr->userPassword,userPassword)){
                result = USER_SUCCESS;
            }else{
                result = USER_ERROR_PASSWD_FAILED;
            }
        }
    }
    return result;
}

UserPtr findUser(ListPtr userList,const char* userName){
    if(userList == NULL){
        return NULL;
    }
    if(userName == NULL){
        return NULL;
    }
    
    UserPtr user = getFromList(userList,(Find)isUser,(void*)userName);
    return user;
}

int delUser(ListPtr userList,const char* userName){
    if(userList == NULL){
        return USER_ERROR_PARAM_ERROR;
    }
    if(userName == NULL){
        return USER_ERROR_PARAM_ERROR;
    }
    int count = removeFromList(userList,(Find)isUser,(void*)userName,(Free)freeUser);
    if(count == -1){
        return USER_ERROR_PARAM_ERROR;
    }
    if(count == 0 ){
        return USER_ERROR_USER_NOT_FOUND;
    }else if(count  >1 ){//移除了多个?
        return USER_SUCCESS;
    }else{
        return USER_SUCCESS;
    }

}
int checkPrivilege(UserPtr user,int privilege){
    if(user == NULL){
        return USER_ERROR_PARAM_ERROR;
    }
    if(user->privilege!= privilege){
        return USER_ERROR_PRIVILEGE_FAILED;
    }else{
        return USER_SUCCESS;
    }
}
int addChannel(UserPtr userPtr,ChannelPtr ptr ){
    if(userPtr == NULL){
        return USER_ERROR_PARAM_ERROR;
    }
    if(ptr == NULL){
        return USER_ERROR_PARAM_ERROR;
    }
    if(userPtr->channels!=NULL){
       insertToList(userPtr->channels,ptr);
       return USER_SUCCESS; 
    }else{
        return USER_ERROR_PARAM_ERROR;
    }
    
}
int isUser(UserPtr userPtr,const char* userName){
    if(userPtr == NULL){
        return 0;
    }
    if(userPtr->userName == NULL){
        return 0;
    }
    if(!strcmp(userName,userPtr->userName)){
        return 1;
    }else{
        return 0;
    }
}

ListPtr restoreUsers(long storePosition){
    return restoreList(storePosition,(RestoreHandle)restoreUser);
}
long storeUsers(ListPtr userList){
    return storeList(userList,(StoreHandle)storeUser);
}
UserPtr restoreUser(long storePosition){
    if(storePosition <=0){
        return NULL;
    }
    UserStore user;
    int result= restore(storePosition,&user,sizeof(UserStore));
    if(result != STORE_SUCCESS){
        return NULL;
    }
    UserPtr ptr = allocMem(sizeof(User));
    ptr->userId = user.userId;
    ptr->userName = restoreString(user.userName);
    ptr->userSecretKey = restoreString(user.userSecretKey);
    ptr->userPassword = restoreString(user.userPassword);
    ptr->group = user.group;
    ptr->privilege = user.privilege;
    ptr->keyUpdateTime = user.keyUpdateTime;
    //ptr->channels = buildList();
    ptr->storePosition = storePosition;
    return ptr;
}
long storeUser(UserPtr ptr){
    if(ptr == NULL){
        return 0;
    }
    UserStore user ;
    if(ptr->storePosition >0){
        restore(ptr->storePosition,&user,sizeof(UserStore));
    }else{
        user.userName = 0;
        user.userSecretKey = 0;
        user.userPassword = 0;
    }
    //don't use the point to access data it will make error
    user.userId = ptr->userId;
    user.userName = storeString(user.userName,ptr->userName,USER_MAX_USER_NAME_SIZE);
    user.userSecretKey = storeString(user.userSecretKey, ptr->userSecretKey,USER_MAX_SECRET_KEY_SIZE);
    user.userPassword = storeString(user.userPassword, ptr->userPassword,USER_MAX_PASSWORD_SIZE);
    user.privilege = ptr->privilege;
    user.group = ptr->group;
    user.keyUpdateTime = ptr->keyUpdateTime;
    user.channels = 0;
    return ptr->storePosition = store(ptr->storePosition,&user,sizeof(UserStore));
}
int tickUser(ListPtr userList){
    //need used to update the key
}




UserPtr findUserByStorePosition(ListPtr userList,long storePosition){
    if(userList == NULL){
        return NULL;
    }
    if(storePosition <= 0){
        return NULL;
    }
    
    UserPtr user = getFromList(userList,(Find)isUserByStorePosition,(void*)&storePosition);
    return user;
}

int isUserByStorePosition(UserPtr userPtr,long * storePosition){
    if(userPtr == NULL ||storePosition == NULL|| *(storePosition)<=0){
        return 0;
    }else{
        return userPtr->storePosition == (*storePosition);
    }
}