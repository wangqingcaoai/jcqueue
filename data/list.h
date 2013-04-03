#ifndef LIST_H
#define LIST_H
#include <stdarg.h>
#define LIST_COUNT_ERROR -1;
#define LIST_ERROR 0;
//typedef int(*Less)(void*, void*);
typedef int(*Free)(void*);
//查找函数，在链表中查找指定内容 后面可变参数为传入的查找参数
typedef int(*Find)(void*,void* args[]);
typedef void* (*Copy)(void*);
typedef int (*Display)(void*,int);
typedef struct List{
    struct List * prev;
    struct List * next;
    void * data;
    Free freeMem;
    Copy copyData;
    int id,maxCount,length;
}List,*ListPtr;
typedef enum ListPosition {  
    LIST_PRE,
    LIST_NEXT,
}ListPosition;
ListPtr initList(Free freeMem, Copy copyData);

int insertToList(ListPtr header, void *data,int length);

void * getFromList(ListPtr header,Find find,void*args[]);
ListPtr getPtrFromList(ListPtr header,Find find,void*args[]);
//获取该节点下一个节点的 值，可以添加查找函数来查找匹配的下一个值 
//当遍历到最后一个节点时，会返回NULL，需要使用者进行判断来过滤到达最后一个节点时的数据使用
void * nextFromList(ListPtr *node, ListPtr endNode,Find find,void*args[]);//TODO
//获取该节点上一个节点的值，可以添加查找函数来查找匹配的上一个值 
//当遍历到最后一个节点时，会返回NULL，需要使用者进行判断来过滤到达最后一个节点时的数据使用
void * preFromList(ListPtr *node,ListPtr endNode,Find find,void*args[]);//TODO

void * getByIdFromList(ListPtr header,int id);
ListPtr getPtrByIdFromList(ListPtr header,int id);
int getCount(ListPtr header);
int displayList(ListPtr header,Display display,int lineShows);
ListPtr findNode(ListPtr startNode,ListPtr endNode,Find find , void * args[],ListPosition position );
int getListNodeId(ListPtr node);
int isEmptyList(ListPtr header);
void* getDataOfListNode(ListPtr node);


int removeFromList(ListPtr header,Find find,void*args[]);
int setToList(ListPtr header,void *data ,int length,Find find,void*args[]);
int freeList(ListPtr *header);
int removeByIdFromList(ListPtr header,int id);
int freeNode(ListPtr *node);


//todo 带条件连续查找
#endif
