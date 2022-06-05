// this code is taken from https://www.youtube.com/watch?v=n2AAhiujAqs

// meta information about the filesystem
// number of inodex
// number of disk blocks
// size of the disk blocks

struct superblock {
    int num_inodes;
    int num_blocks;
    int size_blocks;  
};

struct inode {
    int size;
    char name[8];
};

struct disk_block {
    int next_block_num;
    char data[512];
};

void create_fs(); // initialize new filesystem
void mount_fs(); // load a file system
void sync_fs(); // write the file system
