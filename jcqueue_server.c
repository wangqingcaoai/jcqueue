#include "jcqueue_server.h"
int  Server(){
    initConfig();
    AppServerPtr serverPtr = buildAppServer();
    while(true){
        processServer();
        tick();
    }
}