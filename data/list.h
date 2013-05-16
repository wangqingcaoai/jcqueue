#ifndef DATA_LIST_H
#define DATA_LIST_H
#include <stdarg.h>
#include "base.h"
#define LIST_COUNT_ERROR -1
#define LIST_ERROR 0
#define LIST_ERROR_MEM_ALLOC_FAILED 1
#define LIST_HEADER_ID -1

typedef struct ListNode{
    struct ListNode * prev;
    struct ListNode * next;
    void * data;
    int id;
}ListNode,*ListNodePtr;
typedef struct List 
{
    int listId;
    ListNodePtr header;
    int count,maxCount;
}List ,* ListPtr;
typedef enum ListPosition {  
    LIST_PRE,
    LIST_NEXT,
}ListPosition;
ListPtr buildList();

int insertToList(ListPtr list, void *data);

void * getFromList(ListPtr list,Find find,void*arg);
ListNodePtr getPtrFromList(ListPtr list,Find find,void*arg);
//获取该节点下一个节点的 值，可以添加查找函数来查找匹配的下一个值 
//当遍历到最后一个节点时，会返回NULL，需要使用者进行判断来过滤到达最后一个节点时的数据使用
void * nextFromList(ListNodePtr *node, ListNodePtr endNode,Find find,void*arg);//TODO
//获取该节点上一个节点的值，可以添加查找函数来查找匹配的上一个值 
//当遍历到最后一个节点时，会返回NULL，需要使用者进行判断来过滤到达最后一个节点时的数据使用
void * preFromList(ListNodePtr *node,ListNodePtr endNode,Find find,void*arg);//TODO

void * getByIdFromList(ListPtr list,int id);
ListNodePtr getPtrByIdFromList(ListPtr list,int id);
int getCountOfList(ListPtr list);
int displayList(ListPtr list,Display display,int lineShows);
ListNodePtr findNode(ListNodePtr startNode,ListNodePtr endNode,Find find , void * arg,ListPosition position );
int getListNodeId(ListNodePtr node);
int isEmptyList(ListPtr list);
void* getDataOfListNode(ListNodePtr list);
int removeFromList(ListPtr List,Find find,void*arg,Free freeMem);
//int setToList(ListPtr List,void *data ,int length,Find find,void*arg);
int freeList(ListPtr *l,Free freeMem);
int removeByIdFromList(ListPtr list,int id,Free freeMem);
int freeListNode(ListNodePtr *node,Free freeMem);
ListNodePtr getListHeader(ListPtr);
ListNodePtr getListEnd(ListPtr);
//todo 带条件连续查找
#endif
