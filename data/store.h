#ifndef DATA_STORE_H
#define DATA_STORE_H 
#define STORE_FILE_HEAD "jcq_bak_"
#define STORE_FILE_EXTENSION ".bak"
#define STORE_POSITION_NAME "store"
#define STORE_FILE_OPEN_MODE "wb+"
#define DEFAULT_STORE_PATH "~/.jcq/"
#define DEFAULT_STORE_PATH_SIZE 512
#define DEFAULT_STORE_HEADER "JCQ 1.0 "
static FILE * storeFile;
static int startOffset;
typedef struct Store{
    int offset;// to find where data is
    int state ;// data state;
    int length;//data length;
    char* readFormat;//
    char* writeFormat;//
}Store ,* StorePtr;
int store(StorePtr,...);
int initStore();
int restore(StorePtr,...);
int delStore(StorePtr);
FILE* findStoreFile();

static char*  getRealPath(char*buf,int length,char*path);
#endif