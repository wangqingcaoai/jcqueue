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
    unsigned char ss[30];
    char *s  = allocString(string);
    char *buf = allocMem(MD5_KEY_SIZE);
    memset(buf,'\0',MD5_KEY_SIZE);
    struct MD5Context md5c;
    MD5Init( &md5c );
    MD5Update( &md5c, s, strlen(s) );
    MD5Final( ss, &md5c );
    char tmp[3]={'\0'};
    freeString(&s);
    int i =0;
    for( i=0; i<16; i++ ){
        sprintf(tmp,"%02X", ss[i] );
        strcat(buf,tmp);
    }
    buf[MD5_KEY_SIZE-1]='\0';
    return buf;
}
