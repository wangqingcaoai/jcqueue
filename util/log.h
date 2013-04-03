#ifndef LOG_H
#define LOG_H

typedef enum LogLevel { 
    LOG_WARNING,
    LOG_ERROR,
    LOG_NOTICE,
}LogLevel;
static char* logHeader[]={"WARNING","ERROR","NOTICE"};
int addLog(LogLevel level,const char* logMessage,...);
static char* getLogHeader(LogLevel level);

#endif