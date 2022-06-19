#include "Mylibc.h"

// myfopen function
myFILE *myfopen(const char *pathname, const char *mode)
{
    int fd = myopen(pathname, -1);
    if(fd == -1){
        perror("file not found\n");
        return NULL;
    }

    myFILE * file = (myFILE*)malloc(sizeof(myFILE));
    file->file_descriptor = fd;
    file->pointer = 0;
    strcpy(file->name, inodes[fd].name);

    strcpy(file->mode, mode);

    if (strcmp(mode, "a") == 0){
        mylseek(fd, 0, SEEK_END);
    }

    return file;

}

int myfclose(myFILE *stream){
    if (myclose(stream->file_descriptor) == 0){
        free(stream);
        return 0;
    }
    if(stream != NULL){
        free(stream);
    }
    return -1;
}

size_t myfread(void *ptr, size_t size, size_t nmemb,myFILE *stream){
    return myread(stream->file_descriptor, ptr, size*nmemb);
}

size_t myfwrite(const void *ptr, size_t size, size_t nmemb, myFILE *stream){
    int pos = mywrite(stream->file_descriptor, ptr, size*nmemb);
    sync_fs(stream->name);
    return pos;
}

int myfseek(myFILE *stream, long offset, int whence){
    mylseek(stream->file_descriptor, offset, whence);
    return 0;
}
