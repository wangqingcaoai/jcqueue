#ifndef DATA_BASE_H
#define DATA_BASE_H 
//需要用到的常用函数定义
typedef int(*Free)(void*);
//查找函数，在链表中查找指定内容 后面可变参数为传入的查找参数
typedef int(*Find)(void*,const void* arg);
typedef void* (*Copy)(void*);
typedef int(*Less)(void*, void*);
typedef void(*Record)(void*, int);
typedef void (*Display)(void* , int );
#endif
