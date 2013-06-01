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
int initStore(){
    if(isOn(getConfig("store","on"))){
        if(OFF==isOn(getConfig("store_ignore","off"))){
            FILE* file = findStoreFile();
        }else{
            //ignore backup file . dont read data
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