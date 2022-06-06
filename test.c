#include <stdio.h>
#include "fs.h"

void main(){
    //create_fs();
    //sync_fs();
    mount_fs();
    int file = allocate_file("another");

    set_filesize(0, 5000);
    print_fs();

    char data = 'b';

    int i;
    for (i = 0; i < 49; i++)
    {
        write_byte(file, i*100, &data);
    }
    
    sync_fs();
    printf("done\n");

}// main