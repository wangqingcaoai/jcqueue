#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "parser/jcq.h"
#include "util/config.h"
#include "app_queue/client.h"
#include <string.h>
char *l_opt_arg;  
char* const short_options = "h:p:c:";  
struct option long_options[] = {  
     { "host",  1,  NULL,   'h'     },  
     { "port",  1,  NULL,   'p'     },  
     { "config",1,  NULL,   'c'     },
     {      0,     0,     0,     0},  
};
int main(int argc, char  *argv[])
{
    int opt;
    initConfig(NULL);
    while((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
    {
        switch (opt)
        {
            
            case 'h':
                l_opt_arg = optarg;
                setConfig("host",l_opt_arg);
                printf("%s\n", l_opt_arg);
                break;
            case 'p':
                l_opt_arg = optarg;
                setConfig("port",l_opt_arg);
                printf("%s\n ",l_opt_arg);
                break;
            case 'c':
                l_opt_arg = optarg;
                printf("%s\n",l_opt_arg );
                break;
            default:
                printf("You should look for help! ");
                exit(1);
                break;            
        }
    }
    char* host = getConfig("host","127.0.0.1");
    char* port = getConfig("port","12222");
    AppServerPtr serverPtr = buildClientAppServer(host,port);
    initClientAppServer(serverPtr);
    processClientAppServer(serverPtr);
    // while(true){
    //     processServer();
    //     tick();
    // }

// char *ptr= "cmd:\r\ntarget:test\r\njcq 1.0\r\ntargetType:test\r\nuser:test\r\npassword:test\r\nkey:test\r\ndelay:0\r\npriority:1\r\ntimestamp:\r\nextraParam:param1[value]param2[value2]参数3[]\r\nlength:18\r\ndata:eerttyytwwerttewww\r\n\r\ncmd:\r\ntarget:test\r\njcq 1.0\r\ntargetType:test2\r\nuser:test\r\npassword:test\r\nkey:test\r\ndelay:0\r\npriority:1\r\ntimestamp:\r\nextraParam:param1[value]param2[value2]参数3[]\r\nlength:18\r\ndata:eerttyytwwerttewww\r\n\r\n ";

//     char *test = malloc(strlen(ptr)+1);

//     strcpy(test,ptr);
//     NetMessagePtr nm = buildNetMessage();
//     //displayNetMessage(&nm);
//     int result = isJCQMessage(nm,test,strlen(ptr));
//     printf("%d\n", result);

//     parserJCQMessage(nm,test,strlen(ptr)+1);
//     displayNetMessage(nm);
//     printf("%s\n","next message" );
//     parserJCQMessage(nm,test,strlen(ptr)+1);
//     displayNetMessage(nm);
//     parserJCQMessage(nm,test,0);
//     displayNetMessage(nm);
//     //getExtraParam(&nm,"参数3");
//     // setSendExtraParam(nm,"test","value");
//     // displayNetMessage(nm);
//     // setSendExtraParam(nm,"param1","test");
//     displayNetMessage(nm);
//     setSendExtraParam(nm,"param1","test2");
//     displayNetMessage(nm);
//     setSendExtraParam(nm,"test","new");
//     displayNetMessage(nm);
//     setSendExtraParam(nm,"test","long teste");
//     displayNetMessage(nm);
// int length = 10;
//     char * str[10]={
//         "af[]",
//         "fd[",
//         "ddd",
//         "[fd]fd",
//         "fdfd[fdfd]dfdf",
//         "fdfdf[fdfdf]fdfd[fds",
//         "dfdf[fdfs,]fdf[f]",
//         "[[[[[]",
//         "[fdfd",
//         "fdff]",
//     };
//     int i=0,r=0;
//     while(i<length){
//         r = isExtraParamFormatRight(str[i],strlen(str[i]));
//         printf("%s %s\n", str[i],r?"right":" wrong");   
//         i++;
//     }
    return 0;
}
