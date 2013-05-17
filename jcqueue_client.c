#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

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

    return 0;
}
