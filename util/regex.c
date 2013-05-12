#include "regex.h"
#include <regex.h>
#include <string>
int isMatchedString(const char* string,const char* keyword){
    if(string == NULL){
        return REGEX_PARAM_ERROR;
    }else if(keyword == NULL){
        return REGEX_PARAM_ERROR;
    }
    regex_t reg;
    int result = 0; 
    int ret = regcomp(&reg, keyword, REG_EXTENDED);
    if(!ret){
        ret = regexec(&reg, string, 0, NULL, 0);
        if(ret == 0){
            result = 1;
        }else if(ret == REG_NOMATCH){
            result = REGEX_NOMATCH;
        }else{
            result = REGEX_ERROR;
            char msgbuf[256];
            regerror(ret, &reg, msgbuf, sizeof(msgbuf));
            addLog(LOG_ERROR,"regex error :%s",msgbuf);
        }
    }else{
        char msgbuf[256];
        regerror(ret, &reg, msgbuf, sizeof(msgbuf));
        addLog(LOG_ERROR,"regex error :%s",msgbuf);
        result = REGEX_ERROR;
    }
    regfree(&reg);
    return result;  
}