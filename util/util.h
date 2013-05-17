#include "type.h"
#ifndef UTIL_H
#define UTIL_H
#define UTIL_UTIL_POSITION_NAME "util"
#define UTIL_UTIL_MEM_ALLOC "mem_alloc"
char * trim(char* _s);
void * buildAgrs(void* agrs[],int length,...);
int64 nanoseconds(void);

char * allocString(const char* s);
int freeString(char **s);
int isEmptyString(const char * s);
void* allocMem(int size);

#endif