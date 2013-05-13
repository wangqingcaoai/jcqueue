
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include "log.h"
#include "config.h"

int addLog(LogLevel level,LogLayer layer,const char* position ,const char* logMessage,...){
    int logError = errno;
    char* errorStr = NULL;
    if(logError!=0){
        errorStr = strerror(logError);
    }
    if(isOn(getConfig("verbose"))){
        time_t timep;
        time(&timep);
        struct tm *p = localtime(&timep); /*取得当地时间*/    
        char timeStr[25] ;
        strftime (timeStr,25,"%F %T", p);
        
        fprintf(stderr, "%s %s %s %s : ", timeStr, logLevel[level],logLayer[layer],position);
        va_list va;
        va_start(va,logMessage);
        vfprintf(stderr,logMessage,va);
        va_end(va);
        if(logError){
            fprintf(stderr, "[errno:%d error:%s]",logError , errorStr);    
        }
        fprintf(stderr, "\n");
        fflush(stderr);
    }

    Switch onLog = isOn(getConfig("error_log"));
    if(onLog==ON){
        char* logPath = (char*)getConfig("log_path");
        if(NULL == logPath){
            logPath = "/var/log/jcq_log";   
        }
        
        
        FILE* file = fopen(logPath,"a+");
        if(file == NULL){
            int error =errno;
            printf("open log file error errno.%02d : %s\n", error, strerror(error));
            errno = 0;
            return 1;
        }else{
            time_t timep;
            time(&timep);
            struct tm *p = localtime(&timep); /*取得当地时间*/    
            char timeStr[25] ;
            strftime (timeStr,25,"%F %T", p);
            fprintf(file,  "%s %s %s %s : ", timeStr, logLevel[level],logLayer[layer],position);
            va_list va;
            va_start(va,logMessage);
            vfprintf(file,logMessage,va);
            va_end(va);
            if(logError){
                fprintf(file, "[errno:%d error:%s]",logError , errorStr);    
            }
            fprintf(file, "\n");
            fflush(file);
            fclose(file); /*关闭文件*/
        }   
    }
    errno = 0;
    return 1;

}
static char* getLogLevel(LogLevel level){
    return logLevel[level];
}
static char* getLogLayer(LogLayer layer){
    return logLayer[layer];
}