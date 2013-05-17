#ifndef CONFIG_H
#define CONFIG_H
#define BUF_SIZE 512
#define ANNOTATION ';'
#include "../config_list.h"
#define CURRENT_DIR_CONFIG_FILE "./jcqueue.conf"
#define DEFAULT_CONFIG_FILE "/etc/jcqueue/jcqueue.conf"
typedef struct Config
{
    struct Config *next;
    char* name;
    char* data;
}Config,*ConfigPtr;

//开关
typedef enum Switch{
    OFF,
    ON,
}Switch;
static  ConfigPtr globalConfig =NULL;
void* getConfig(char* configName, char* defaultValue);
int initConfig(char * fileName);
int setConfig(char* configName, char* configData );
/**
 * 删除配置信息
 */
int destoryConfig();
int buildConfigFile();
int printError(int lineNum,const char* errorInfo, const char* data);
/**
 * 显示配置信息
 */
int displayConfig();
Switch isOn(char* data);
#endif
