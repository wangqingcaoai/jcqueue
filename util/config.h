#ifndef CONFIG_H
#define CONFIG_H
#define BUF_SIZE 512
#define ANNOTATION ';'
#include "../config_list.h"
typedef struct Config
{
    struct Config *next;
    char* name;
    void* data;
}Config,*ConfigPtr;

//开关
typedef enum Switch{
    OFF,
    ON,
}Switch;
static  ConfigPtr globalConfig =NULL;
void* getConfig(char* configName);
int initConfig();
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
