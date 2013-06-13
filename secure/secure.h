#ifndef SECURE_H
#define SECURE_H 
#define MD5_KEY_SIZE 33
char* buildRandomSecureKey();
char* buildSecureKey(const char* salt);
char * buildMd5Key(const char* string);
#endif