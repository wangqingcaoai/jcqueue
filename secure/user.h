#ifndef USER_H
#define USER_H
typedef enum UserCheckState {
    USER_CHECK_SUCCESS , 
    USER_CHECK_ERROR_PARAM_ERROR,
    USER_CHECK_ERROR_NOT_EXIST,
    USER_CHECK_ERROR_KEY_ERROR,
}UserCheckState;
typedef enum UserAddState {
    USER_ADD_SUCCESS , 
    USER_ADD_NULL,
    USER_ADD_USER_NAME_NULL,
}UserAddState;
typedef  struct User{
	char* userName;
	char* userKey;
	char* description;
}User;
UserCheckState checkUser(User user);

UserAddState addUser(User user);
int setUser(User user);
User getUser(char*  userName);
#endif
