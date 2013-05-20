#ifndef UTIL_REGEX_H
#define UTIL_REGEX_H 
#define REGEX_SUCCESS 0
#define REGEX_PARAM_ERROR 1
#define REGEX_KEYWORD_FORMAT_ERROR 2
#define REGEX_NOMATCH 3
#define REGEX_ERROR 4
#define REGEX_LOG_POSITION_NAME "regex"
int isMatchedString(const char* string,const char* keyword);
#endif