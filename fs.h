// this code is taken from https://www.youtube.com/watch?v=n2AAhiujAqs

// meta information about the filesystem
// number of inodex
// number of disk blocks
// size of the disk blocks

#define BLOCKSIZE 512

struct superblock {
    int num_inodes;
    int num_blocks;
    int size_blocks;  
};

struct inode {
    int size;
    int first_block;
    char name[8];
};

struct disk_block {
    int next_block_num;
    char data[BLOCKSIZE];
};

void create_fs(); // initialize new filesystem
void mount_fs();  // load a file system
void sync_fs();   // write the file system


int allocate_file(char name[8]); //return filenumber
void set_filesize(int filenum, int size);
void write_byte(int filenum, int pos, char * data);

void print_fs(); // print out info about the filesystem

// delete
// read byte
