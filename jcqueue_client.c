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
    initConfig("/etc/jcq/jcqueue_client.conf");
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
    char* store_dir = getConfig("store_dir",NULL);
    if(store_dir == NULL){
        setConfig("store_dir","~/.jcq_client/");
    }
    //initClientAppServer(serverPtr);
    processClientAppServer(serverPtr);
    return 0;
}
