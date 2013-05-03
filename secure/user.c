// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include "../util/config.h"
// #include "user.h"

// UserCheckState checkUser(User user ){
//     if(getConfig(SECURE_CHECK_USER)){
//         User userSaved = getUser(user.userName);
//         if(userSaved.userName == NULL){
//             return USER_CHECK_ERROR_NOT_EXIST;
//         }else if(strcmp(user.userKey,userSaved.userKey)){
//             return USER_CHECK_ERROR_KEY_ERROR;
//         }else{
//             return USER_CHECK_SUCCESS;
//         }
//     }
//     return USER_CHECK_SUCCESS;
// }

// UserAddState addUser(User user){

//     if(user.userName == NULL){
//         return USER_ADD_USER_NAME_NULL;
//     }else{
//         //TODO
//         return USER_ADD_SUCCESS;
//     }
// }
