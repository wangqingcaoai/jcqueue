#include "type.h"
#ifndef UTIL_H
#define UTIL_H
#define UTIL_UTIL_POSITION_NAME "util"
#define UTIL_UTIL_MEM_ALLOC "mem_alloc"
#define UTIL_NUM_BUF_SIZE 30
char * trim(char* _s);
void * buildAgrs(void* agrs[],int length,...);
int64 nanoseconds(void);

char * allocString(const char* s);
int freeString(char **s);
int isEmptyString(const char * s);
void* allocMem(int size);
char *strnstr(const char *s1, const char *s2, int len);
int freeMem(void**pptr);
char* int64ToString(int64 i,char *buf,int length);
char* intToString(int i,char *buf,int length);
char* getStrTime(char* buf,int length,char* format);
#endif