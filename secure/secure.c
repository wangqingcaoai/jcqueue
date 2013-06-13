#include "secure.h"
#include "md5.h"
#include <stdio.h>
#include <string.h>
#include "../util/util.h"
char* buildRandomSecureKey(){
    
}
char* buildSecureKey(const char* salt){

}
char * buildMd5Key(const char* string){
    unsigned char ss[16];
    char *buf = allocMem(sizeof(char)*MD5_KEY_SIZE);
    struct MD5Context md5c;
    MD5Init( &md5c );
    MD5Update( &md5c, string, strlen(string) );
    MD5Final( ss, &md5c );
    char tmp[3]={'\0'};
    int i =0;
    for( i=0; i<16; i++ ){
        sprintf(tmp,"%02X", ss[i] );
        strcat(buf,tmp);
    }
    return buf;
}
