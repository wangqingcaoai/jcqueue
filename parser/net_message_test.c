#include "net_message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
    int length = 0;
    char * str[10]={
        "af[]",
        "fd[",
        "ddd",
        "[fd]fd",
        "fdfd[fdfd]dfdf",
        "fdfdf[fdfdf]fdfd[fds",
        "dfdf[fdfs,]fdf[f]",
        "[[[[[]",
        "[fdfd",
        "fdff]",
    };
    int i=0,r=0;
    while(i<length){
        r = isExtraParamFormatRight(str[i],strlen(str[i]));
        printf("%s %s\n", r?"right":" wrong");   
    }
}