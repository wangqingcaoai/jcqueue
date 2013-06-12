#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "app_queue/server.h"
#include "util/config.h"
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
    initConfig("/etc/jcq/jcqueue_server.conf");
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
    //setConfig();
    char* host = getConfig("host","127.0.0.1");
    char* port = getConfig("port","12221");
    AppServerPtr serverPtr = buildAppServer(host,port);
    char* store_dir = getConfig("store_dir",NULL);
    if(store_dir == NULL){
        setConfig("store_dir","~/.jcq_server/");
    }
    // struct sigaction act;
    // act.sa_sigaction=serverStop();
    // act.sa_flags=SA_SIGINFO;
    // sigemptyset(&act.sa_mask);
    // sigaction(SIGINT,&act,NULL);
    // sigaction(SIGQUIT,&act,NULL); 

    //initAppServer(serverPtr);
    processAppServer(serverPtr);
    return 0;
} 