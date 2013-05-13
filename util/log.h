#ifndef LOG_H
#define LOG_H

typedef enum LogLevel { 
    LOG_WARNING,
    LOG_ERROR,
    LOG_NOTICE,
}LogLevel;
typedef enum LogLayer { 
    LOG_LAYER_APP_QUEUE,
    LOG_LAYER_BASE_QUEUE,
    LOG_LAYER_DATA,
    LOG_LAYER_PARSER,
    LOG_LAYER_SECURE,
    LOG_LAYER_TRANSFAR,
    LOG_LAYER_UTIL,
}LogLayer;
static char* logLevel[]={"WARNING","ERROR","NOTICE"};
static char* logLayer[]={"App_queue","Base_queue","Data","Parser","Secure","Transfar","Util"};
int addLog(LogLevel level,LogLayer layer,const char* position ,const char* logMessage,...);
static char* getLogLevel(LogLevel level);
static char* getLogLayer(LogLayer layer);

#endif