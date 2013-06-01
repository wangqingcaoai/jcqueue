#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
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
    int allocLength = strlen(s)+1;
    char* ptr = allocMem(allocLength);
    if(ptr == NULL){
        return NULL;
    }
    strcpy(ptr,s);
    ptr[allocLength-1] = '\0';
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
    char * ptr = allocMem(strlen(s)+1);
    if(ptr ==NULL){
        return 0;
    }
    strcpy(ptr,s);
    char* str =  trim(ptr);
    int result = 0;
    if(str[0]=='\0'){
        result = 1;
    }else{
        result = 0;
    }
    free(ptr);
    return result;
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
void* allocMem(int size){
    if(size <= 0){
        return NULL;
    }
    void* ptr = malloc(size);
    if(ptr == NULL){
        addLog(LOG_WARNING,LOG_LAYER_UTIL,UTIL_UTIL_POSITION_NAME,"allocMem memory(size:%d) failed! maybe app is out of memory!",size);
        return NULL;
    }
    return ptr;
}
int freeMem(void**s){
    if(s == NULL){
        return -1;
    }else{
        void* ptr = (*s);
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
#ifndef __HAVE_ARCH_STRNSTR   
/** 
 * strnstr - Find the first substring in a length-limited string 
 * @s1: The string to be searched 
 * @s2: The string to search for 
 * @len: the maximum number of characters to search 
 */  
char *strnstr(const char *s1, const char *s2, int len)  
{  
    size_t l2;  
  
    l2 = strlen(s2);  
    if (!l2)  
        return (char *)s1;  
    while (len >= l2) {  
        len--;  
        if (!memcmp(s1, s2, l2))  
            return (char *)s1;  
        s1++;  
    }  
    return NULL;  
}  
//EXPORT_SYMBOL(strnstr);  
#endif   


char* int64ToString(int64 i,char *buf,int length){
    if(buf == NULL||length<=0){
        return NULL;
    }
    snprintf(buf,length,"%" PRId64 "",i);
    return buf;
}
char* intToString(int i,char *buf,int length){
    if(buf == NULL||length<=0){
        return NULL;
    }
    snprintf(buf,length,"%d",i);
    return buf;
}

char* getStrTime(char* buf,int length,char* format){
    if(buf == NULL ||length<=0 || format == NULL){
        return NULL;
    }
    time_t timep;
    time(&timep);
    struct tm *p = localtime(&timep); /*取得当地时间*/    
    strftime (buf,length,format, p);
    return buf;
}