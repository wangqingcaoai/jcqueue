#include <stdio.h>
#include <stdlib.h>
#include "user.h"
#include "../util/util.h"
#include "../data/list.h"

UserPtr buildUser(const char* userName,const char* userPassWord){
    if(isEmptyString(userName)|| isEmptyString(userPassWord)){
        return NULL;
    }
    static int user_id;
    UserPtr ptr = (UserPtr)allocMem(sizeof(User));
    if(ptr == NULL){
        return NULL;
    }
    user_id ++;
    ptr->userId = user_id;
    ptr->userName = allocString(userName);
    ptr->userSecretKey = NULL;
    ptr->userPassWord = allocString(userPassWord);
    ptr->privilege = USER_PRIVILEGE_DEFAULT;
    ptr->group = USER_GROUP_DEFAULT;
    ptr->keyUpdateTime = USER_DEFAULT_UPDATE_TIME;
    ptr->channels = buildList();
    return ptr;
}

int setUserPassWord(UserPtr ptr, const char* userPassWord){
    if(ptr == NULL){
        return USER_ERROR_PARAM_ERROR;
    }else{
        if(ptr->userPassWord != NULL){
            freeString(&(ptr->userPassWord));
        }
        ptr->userPassWord = allocString(userPassWord);
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
        freeString(&(ptr->userPassWord));
        freeString(&(ptr->userSecretKey));
        freeList(&(ptr->channels),(Free)freeChannel);
        free(ptr);
        return USER_SUCCESS;      
    }
}

int addUser(ListPtr userList,const char* userName,const char* userPassWord ,int privilege , int group ){
    if(userList == NULL){
        return USER_ERROR_PARAM_ERROR;
    }
    if(userName == NULL){
        return USER_ERROR_PARAM_ERROR;
    }
    UserPtr user = findUser(userList,userName);
    if(user == NULL){
        UserPtr user = buildUser(userName,userPassWord);
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
int checkUser(UserPtr userPtr,const char* userSecretKey,const char* userPassWord){
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
            needCheckPasswd = 0;
        }
    }
    if(needCheckPasswd){
        if(userPassWord == NULL){
            if(userPtr->userPassWord == NULL){
                result = USER_SUCCESS;
            }else{
                result = USER_ERROR_PASSWD_FAILED;
            }
        }else{
            if(userPtr->userPassWord == NULL){
                result = USER_ERROR_PASSWD_FAILED;
            }else if(!strcmp(userPtr->userPassWord,userPassWord)){
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

int restoreUsers(ListPtr userList){

}
int storeUsers(ListPtr userList){

}




