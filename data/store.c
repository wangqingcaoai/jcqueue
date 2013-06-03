#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include "store.h"
#include "../util/config.h"
#include "../util/log.h"
#include "../util/util.h"
int initStore(){
    if(isOn(getConfig("store","on"))){
        if(OFF==isOn(getConfig("store_ignore","off"))){
            storeFile = findStoreFile();
            startOffset = strlen(DEFAULT_STORE_HEADER);
			char format[startOffset+1];
			int readNum = fread(format,startOffset,1,storeFile);
            //it means a new store file
            if(readNum == 0){
                fwrite(DEFAULT_STORE_HEADER,startOffset,1,storeFile);
                fflush(storeFile);
            }else{
                format[startOffset]='\0';
                if(!isSameString(format,DEFAULT_STORE_HEADER)){
                    addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME," store file format not support!");
                    exit(1);
                }    
            }
			
		}else{
            //ignore backup file . don't need read data
        } 
    }else{
        addLog(LOG_NOTICE,LOG_LAYER_DATA,STORE_POSITION_NAME," store function is closed ,the program while lose the data after you close it");
        storeFile = NULL;
    }  
}

FILE* findStoreFile(){

    char storeFileName[DEFAULT_STORE_PATH_SIZE]="";
    getRealPath(storeFileName,DEFAULT_STORE_PATH_SIZE,(char*)getConfig("store_file",NULL));
    
    DIR *storeDir;
    char storeDirName[DEFAULT_STORE_PATH_SIZE]="";
    getRealPath(storeDirName,DEFAULT_STORE_PATH_SIZE, (char*)getConfig("store_dir",DEFAULT_STORE_PATH));
        
    if(isEmptyString(storeFileName)){
        // need to load file from store_dir 
        if(isEmptyString(storeDirName)){
            addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME,"store dir is empty in config.");
            exit(1);  
        }
        //check dir
        storeDir = opendir(storeDirName);
        if(storeDir == NULL){
            addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME,"read store_dir[%s] failed!",storeDirName);
            exit(1);
        }
        // find file
        int headLength = strlen(STORE_FILE_HEAD);
        int extensionLength = strlen(STORE_FILE_EXTENSION);
        struct  dirent * dir;
        long long n=0,max=0;
        char*p;
        while ((dir = readdir(storeDir))) {
            if (strncmp(dir->d_name, STORE_FILE_HEAD, headLength) == 0) {
                n = strtoll(dir->d_name+headLength, &p, 10);
                if(p && strncmp(p,STORE_FILE_EXTENSION,extensionLength )==0 ) {
                    if(n>max)max =n;
                }
            }
        }
        closedir(storeDir);
        //means no new file
        char timeStr[25];
        if(max == 0){
            getStrTime(timeStr,25,"%Y%m%d%H%M%S");    
        }else{
            snprintf(timeStr,25,"%lld",max);
        }
        int newFileLength = strlen(storeDirName)+strlen(timeStr)+strlen(STORE_FILE_HEAD)+strlen(STORE_FILE_EXTENSION)+1;
        char newStoreFileName[newFileLength];
        snprintf(newStoreFileName,newFileLength,"%s%s%s%s",storeDirName,STORE_FILE_HEAD,timeStr,STORE_FILE_EXTENSION);
        printf("%s\n",newStoreFileName );
        FILE* file= fopen(newStoreFileName,STORE_FILE_OPEN_MODE);
        if(file == NULL){
            addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME,"read file[%s] failed!",newStoreFileName);
            exit(1);
        }
        return file;
        
    }else{
        if(access(storeFileName,F_OK)!=0){
            addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME,"store file [%s] not exist!",storeFileName);
            exit(1); 
        }
        if(isOn(getConfig("store_each_day","off"))){
            //we need copy it so check the read mode 
            if(access(storeFileName,R_OK)!=0){
                addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME,"store file [%s] could not read ,we couldn't copy it!",storeFileName);
                exit(1);
            }
            if(isEmptyString(storeDirName)){
                addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME,"store dir is empty in config.");
                exit(1);  
            }
            //check dir
            storeDir = opendir(storeDirName);
            if(storeDir == NULL){
                addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME,"read store_dir[%s] failed!",storeDir);
                exit(1);
            }
            closedir(storeDir);
            char timeStr[25];
            getStrTime(timeStr,25,"%Y%m%d%H%M%S");
            int copyFileLength = strlen(storeDirName)+strlen(timeStr)+strlen(STORE_FILE_HEAD)+strlen(STORE_FILE_EXTENSION)+1;
            char copyStoreFileName[copyFileLength];
            snprintf(copyStoreFileName,copyFileLength,"%s%s%s%s",storeDirName,STORE_FILE_HEAD,timeStr,STORE_FILE_EXTENSION);
            // cp file 
            int copyCmdLength = strlen("cp ")+strlen(storeFileName)+1+strlen(copyStoreFileName)+1;
            char copyCmd[copyCmdLength];
            snprintf(copyCmd,copyCmdLength,"cp %s %s",storeFileName,copyStoreFileName);
            system(copyCmd);
            FILE* file= fopen(copyStoreFileName,STORE_FILE_OPEN_MODE);
            if(file == NULL){
                addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME,"read file[%s] failed!",copyStoreFileName);
                exit(1);
            }
            return file;
        }else{
            FILE* file= fopen(storeFileName,STORE_FILE_OPEN_MODE);
            if(file == NULL){
                addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME,"read file[%s] failed!",storeFileName);
                exit(1);
            }
            return file;
        }
        
    }
    
}
static char*  getRealPath(char*buf,int length,char*path){
    if (length<=0||buf ==NULL){
        return NULL;
    }
    if(path  == NULL){
        buf[0]=='\0';
    }else{
        if(path[0]== '~'){

            extern char **environ;  
            char **env = environ;
            char* home = getenv("HOME");
            snprintf(buf,length,"%s%s",home,path+1);
        }else{
            strncpy(buf,path,length);
        }
    }
    
    return buf;
}

//make sure the length is not change.
long store(long offset,void * data,int length){
    if(storeFile==NULL||data == NULL ||length <=0){
        return -1;
    }
    Store store;
    store.offset = offset;
    store.length = length;
    store.state = STORE_STATE_USING;
    if(offset<=0){
        int result = fseek(storeFile,0L,SEEK_END);
        if(result == -1){
            addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME,"seek offset failed");
            return -3;
        }
        store.offset=ftell(storeFile);
        //write StoreHeader first
        fwrite(&store,sizeof(Store),1,storeFile);
    }else{
        fseek(storeFile,offset,SEEK_SET);
        fread(&store,sizeof(Store),1,storeFile);
        if(store.length!= length){
            // length change 
            addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME,"store data at offset[%ld] failed ,data length not same",offset);
            return -2;
        }
    }
    
    //then write data;
    fwrite(data,length,1,storeFile);
    fflush(storeFile);
    return store.offset;
}

int restore(long offset,void * data,int length){
    if(offset <=0||storeFile==NULL||data==NULL||length <=0){
        return -1;
    }
    Store store;
    fseek(storeFile,offset,SEEK_SET);
    //readStore StoreHeader first
    fread(&store,sizeof(Store),1,storeFile);
    //read data
    //
    if (store.length!= length)
    {
        addLog(LOG_ERROR,LOG_LAYER_DATA,STORE_POSITION_NAME,"restore data at offset[%ld] failed ,data length not same ",offset);
        return -2;
    }
    fread(data,length,1,storeFile);
    return 0;
}
int delStore(long offset){
    if(offset<=0){
        return STORE_ERROR_PARAM_ERROR;
    }else{
        Store s;
        fseek(storeFile,offset,SEEK_SET);
        fread(&s,sizeof(Store),1,storeFile);
        s.state = STORE_STATE_DEL;
        fseek(storeFile,offset,SEEK_SET);
        fwrite(&s,sizeof(Store),1,storeFile);
    }
}


long storeString(char* string){
    if(string==NULL){
        return -1;
    }
    return store(0,string,strlen(string));
}

char* restoreString(long offset){
    if(offset<=0){
        return NULL;  
    }
    Store s;
    fseek(storeFile,offset,SEEK_SET);
    fread(&s,sizeof(Store),1,storeFile);
    char * buf = allocMem(s.length+1);
    fread(buf,s.length,1,storeFile);
    buf[s.length+1]='\0';
    return buf;
}
