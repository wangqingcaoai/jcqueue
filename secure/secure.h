#ifndef SECURE_H
#define SECURE_H 
char* buildRandomSecureKey();
char* buildSecureKey(const char* salt);
#endif