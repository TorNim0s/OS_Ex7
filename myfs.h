// this code is based on https://www.youtube.com/watch?v=n2AAhiujAqs

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BLOCKSIZE 512
#define MAX_FILES 10000
#define O_CREAT 0100

struct superblock {
    int num_inodes;
    int num_blocks;
    int size_blocks;  
};

struct inode {
    int size;
    int first_block;
    char name[8];
    int is_dir;
};

struct disk_block {
    int next_block_num;
    char data[BLOCKSIZE];
};

struct mydirent{
    int fds[10];
    char d_name[10];
    int size;
};

struct opened{
    int fd;
    int pos;
};

void sync_fs(const char* target);   // write the file system
void sync_again(const char * target);

int allocate_file(const char * name, int size); //return filenumber
void set_filesize(int filenum, int size);void write_byte(int filenum, int pos, char * data);
char read_byte(int filenum, int pos);

int get_block_num(int file, int offset);
int create_file(const char * path, const char* name); // create a file in a path
int create_dir(const char * path, const char* name); // create a directory in a path

void mymkfs(int size); // create a new filesystem
int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data); // mount a filesystem
int myopen(const char *pathname, int flags); // open a file
int myclose(int fd); // close a file
size_t myread(int myfd, void *buf, size_t count); // read from a file
size_t mywrite(int myfd, const void *buf, size_t count); // write to a file
off_t mylseek(int myfd, off_t offset, int whence); // seek in a file
int myopendir(const char *name); // open a directory
struct mydirent *myreaddir(int dirp); // read a directory
int myclosedir(int myfd); // close a directory

extern struct superblock sb;
extern struct inode *inodes;
extern struct disk_block *dbs;
extern struct opened opened[MAX_FILES];