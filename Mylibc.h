#include "myfs.h"
#include <stdarg.h>

typedef struct myFILE
{
    char *name;
    int file_descriptor;
    
    int pointer;
    char mode[3];
    
    
} myFILE;

myFILE *myfopen(const char *pathname, const char *mode);
int myfclose(myFILE *stream);
size_t myfread(void *ptr, size_t size, size_t nmemb,myFILE *stream);
size_t myfwrite(const void *ptr, size_t size, size_t nmemb, myFILE *stream);
int myfseek(myFILE *stream, long offset, int whence);
int myfscanf(myFILE *stream, const char * format, ...);
int myfprintf(myFILE *stream, const char * format, ...);