#ifndef DATA_STORE_H
#define DATA_STORE_H 
#define STORE_FILE_HEAD "jcq_bak_"
#define STORE_FILE_EXTENSION ".bak"
#define STORE_POSITION_NAME "store"
#define STORE_FILE_OPEN_MODE "wb+"
#define DEFAULT_STORE_PATH "~/.jcq/"
#define DEFAULT_STORE_PATH_SIZE 512
#define DEFAULT_STORE_HEADER "JCQ 1.0 "
#define DEFAULT_STORE_BUF_SIZE 512
#define STORE_STATE_USING 1
#define STORE_STATE_DEL 2

#define STORE_SUCCESS 0
#define STORE_ERROR_PARAM_ERROR -1
#define STORE_ERROR_DATA_LENGTH_ERROR -2
static FILE * storeFile;
static int startOffset;
typedef struct Store{
    long offset;// to find where data is
    int state ;// data state;
    int length;//data length;
}Store ,* StorePtr;
long store(long offset,void* data,int length);
int initStore();
int restore(long offset,void *data,int length);
int delStore(long offset);
FILE* findStoreFile();
char* restoreString(long offset);
long storeString(char* string);
static char*  getRealPath(char*buf,int length,char*path);
#endif
