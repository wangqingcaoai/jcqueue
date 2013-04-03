#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../config_list.h"
#include "util.h"
#include "config.h"
/**
 * @param
 */
void* getConfig(char* configName){

    ConfigPtr ptr = globalConfig;
    while(ptr!=NULL){
        if(strcmp(configName,ptr->name)==0){
            return ptr->data;
        }
        ptr = ptr->next;
    }
    return NULL;
}

int initConfig(){
    // find conf file on current directory
    FILE * configFile = fopen("./jcqueue.conf","r");
    if(configFile ==NULL ){
        int error =errno;
        fprintf(stderr,"open config file error errno.%02d : %s\n", error, strerror(error));
    }else{

        char buf[BUF_SIZE];
        int length = 0,line = 0,hasError = 0;
        char *key = NULL,*value = NULL;
        while( fgets(buf,BUF_SIZE,configFile)!=NULL){
            length = strlen(buf);
            //读出的是一行
            line++;
            if(length >1)
            {
                if(buf[0] == ANNOTATION){
                    //just jump the annotation line
                    //here is jump a long line that length more than BUF_SIZE
                }else{
                    if(buf[length-1] == '\n'){
                        buf[length-1] = '\0';//去除末尾换行符号
                        key = strtok(buf,"=");
                        value = strtok(NULL,"=");
                        if(value==NULL){
                            hasError = 1;
                            printError(line," value is null ",buf);
                        }else{

                            setConfig(key,value);
                        }
                        continue;
                    }else{
                        hasError = 1;
                        printError(line," it is to long ",buf);
                    }
                }
                while(buf[length-1]!='\n'){
                    if(fgets(buf,BUF_SIZE,configFile)!= NULL){
                        length = strlen(buf);
                    }else{
                        break;
                    }
                }
            }else if(length==1){
                //just jump empty line;
            }else{

            }

        }
        fclose(configFile);
        if(hasError){
            exit(1);
        }
    }

    return 0;
}

int setConfig(char* configName, char* configData){
    configName = trim(configName);
    configData = trim(configData);
    ConfigPtr ptr = globalConfig;
    if(NULL == ptr){
        globalConfig= malloc(sizeof(Config));

        globalConfig->next = NULL;
        globalConfig->name = malloc(strlen(configName)+1);
        strcpy(globalConfig->name,configName);
        globalConfig->data = malloc(strlen(configData)+1);
        strcpy(globalConfig->data,configData);
        return 1;

    }
    ConfigPtr last = ptr;
    while(ptr!=NULL){
        if(strcmp(configName,ptr->name)==0){
            free(ptr->data);
            ptr->data = malloc(strlen(configData)+1);
            strcpy(ptr->data,configData);
            return 1;
        }
        last = ptr;
        ptr = ptr->next;
    }


    ptr = last->next = malloc(sizeof(Config));
    ptr->next = NULL;
    ptr->name = (char*)malloc(strlen(configName)+1);
    strcpy(ptr->name,configName);
    ptr->data = malloc(strlen(configData)+1);
    strcpy(ptr->data,configData);
    return 1;
}
int destoryConfig(){
    ConfigPtr ptr =globalConfig;
    ConfigPtr tmp = ptr;
    while(ptr!=NULL){
        tmp = ptr->next;
        free(ptr->data);
        free(ptr->name);
        free(ptr);
        ptr = tmp;
    }
    globalConfig = NULL;
    tmp=NULL;
    return 0;
}
int buildConfigFile(){
    return 0;
}
int displayConfig(FILE * file){
    if(file == NULL){
        file = stderr;
    }
    ConfigPtr ptr = globalConfig;
    while(ptr!=NULL){
        fprintf(file,"%s=>%s \n", ptr->name, (char*)ptr->data);
        ptr = ptr->next;
    }
    return 1;
}
int printError(int lineNum,const char* errorInfo,const char* data){
    fprintf(stderr, "error on line %d %s \"%s\"\n",lineNum,errorInfo,data );
    return 1;
}

Switch isOn(char* data){
    if(strcasecmp(data,"on")==0){
        return ON;
    }
    return OFF;
}
