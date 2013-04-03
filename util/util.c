
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctype.h>
#include <string.h>
#include "type.h"
#include "util.h"
#include "log.h"
char * trim(char* _s)
{
  if (_s)
  {
    while (isspace(*_s)) _s++;
    if (*_s)
    {
      char *end = _s + strlen(_s);
      while (isspace(end[-1])) end--;
      *end = '\0';
    }
  }
  return _s;
}


int64
nanoseconds(void)
{
    int r;
    struct timeval tv;

    r = gettimeofday(&tv, 0);
    if (r != 0) return addLog(LOG_WARNING,"gettimeofday"), -1; // can't happen

    return ((int64)tv.tv_sec)*1000000000 + ((int64)tv.tv_usec)*1000;
}
