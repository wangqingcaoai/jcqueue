
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctype.h>
#include <string.h>
#include "type.h"
#include "util.h"
#include "log.h"
char * trim(char* _s)
{
    if (_s)
    {
        while (isspace(*_s)) _s++;
        if (*_s)
        {
            char *end = _s + strlen(_s);
            while (isspace(end[-1])) end--;
            *end = '\0';
        }
    }
    return _s;
}


int64
nanoseconds(void)
{
    int r;
    struct timeval tv;

    r = gettimeofday(&tv, 0);
    if (r != 0) return addLog(LOG_WARNING,LOG_LAYER_UTIL,UTIL_UTIL_POSITION_NAME,"gettimeofday"), -1; // can't happen

    return ((int64)tv.tv_sec)*1000000000 + ((int64)tv.tv_usec)*1000;
}

char * allocString(const char* s){
    if(s==NULL){
        return NULL;
    }
    char* ptr = malloc(strlen(s)+1);
    strcpy(ptr,s);
    return ptr;
}

int  freeString( char** s){
    if(s == NULL){
        return -1;
    }else{
        char* ptr = (*s);
        if(ptr == NULL ){
            return -1;
        }else{
            free(ptr);
            ptr = NULL;
            *s = NULL;
            return 1;
        }
    }
}
int isEmptyString(const char* s){
    if(s == NULL){
        return 1;
    }
    char * ptr = malloc(strlen(s)+1);
    strcpy(ptr,s);
    char* str =  trim(ptr);

    if(str[0]=='\0'){
        return 1;
    }else{
        return 0;
    }
}

int isSameString(const char* first ,const char* second){
    if(first == NULL || second == NULL){
        return 0;
    }
    if(!strcmp(first,second)){
        return 1;
    }else{
        return 0;
    }
}