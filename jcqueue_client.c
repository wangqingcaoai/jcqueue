#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "parser/jcq.h"
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
    while((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
    {
        switch (opt)
        {
            case 'h':
                l_opt_arg = optarg;
                printf("%s\n", l_opt_arg);
                break;
            case 'p':
                l_opt_arg = optarg;
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
    initConfig(NULL);

    //sAppServerPtr serverPtr = buildAppServer();
    //initAppServer();
    // while(true){
    //     processServer();
    //     tick();
    // }

char *ptr= "cmd:\r\ntarget:test\r\ntargetType:test\r\nuser:test\r\npassword:test\r\nkey:test\r\ndelay:0\r\npriority:1\r\ntimestamp:\r\nextraParam:param1[value]param2[value2]参数3[]\r\nlength:18\r\ndata:eerttyytwwerttewww\r\n\r\n ";
    
    char *test = malloc(strlen(ptr)+1);
    strcpy(test,ptr);
    NetMessagePtr nm = buildNetMessage();
    //displayNetMessage(&nm);
    //parserJCQMessage(&nm,test,strlen(ptr)+1);
    //displayNetMessage(&nm);
    //getExtraParam(&nm,"参数3");
    setSendExtraParam(nm,"test","value");
    displayNetMessage(nm);
    setSendExtraParam(nm,"param1","test");
    displayNetMessage(nm);
    setSendExtraParam(nm,"param1","test2");
    displayNetMessage(nm);
    setSendExtraParam(nm,"test","new");
    displayNetMessage(nm);
    setSendExtraParam(nm,"test","long teste");
    displayNetMessage(nm);

    return 0;
}
