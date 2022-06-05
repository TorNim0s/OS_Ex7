#include "fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
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
*/

struct superblock sb;
struct inode *inodes;
struct disk_block *dbs;

// initialize new filesystem
void create_fs(){ 
    sb.num_inodes = 10;
    sb.num_blocks = 100;
    sb.size_blocks = sizeof(struct disk_block);
    
    int i;
    inodes = malloc(sizeof(struct inode) * sb.num_inodes);
    for(i=0; i<sb.num_inodes; i++){
        inodes[i].size = -1;
        strcpy (inodes[i].name, "emptyfi");
    } // init inodes

    dbs = malloc(sizeof(struct disk_block) * sb.num_blocks);
    for(i=0; i<sb.num_blocks; i++){
        dbs[i].next_block_num = -1;
    } // init disk blocks
}
// load a file system
void mount_fs(){

} 

// write the file system
void sync_fs(){
    FILE *file;
    file = fopen("fs_data", "w+");

    // superblock
    fwrite(&sb, sizeof(struct superblock), 1, file);

    //inodes
    int i;
    for(i=0; i<sb.num_inodes; i++){
        fwrite(&(inodes[i]), sizeof(struct inode), 1, file);
    } // write inodes

    for(i=0; i<sb.num_blocks; i++){
        fwrite(&(dbs[i]), sizeof(struct disk_block), 1, file);
    } // write db

    fclose(file);
}