#include "myfs.h"

struct superblock sb;
struct inode *inodes;
struct disk_block *dbs;
struct opened opened[MAX_FILES];

int find_empty_inode() //from video
{
    int i;
    for (i = 0; i < sb.num_inodes; i++)
    {
        if (inodes[i].first_block == -1)
        {
            return i;
        }
    }
    return -1;
} // find empty inode

int find_empty_block() //from video
{
    int i;
    for (i = 0; i < sb.num_blocks; i++)
    {
        if (dbs[i].next_block_num == -1)
        {
            return i;
        }
    }
    return -1;
} // find empty block

int get_block_num(int file, int offset) //from video
{
    int togo = offset;
    int bn = inodes[file].first_block;
    while (togo > 0)
    {
        bn = dbs[bn].next_block_num;
        togo--;
    }
    return bn;
} // get_block_num

// write the file system
void sync_fs(const char *target) //from video was called mount_fs
{
    FILE *file;
    file = fopen(target, "w+");
    if (!file)
    {
        printf("Error: could not open file \n");
        return;
    }

    // superblock
    fwrite(&sb, sizeof(struct superblock), 1, file);

    // inodes
    fwrite(inodes, sizeof(struct inode), sb.num_inodes, file);

    // disk blocks
    fwrite(dbs, sizeof(struct disk_block), sb.num_blocks, file);

    fclose(file);
}

void sync_again(const char *target) //same from video fun but for reading (mount_fs)
{
    FILE *file;
    file = fopen(target, "w+");
    if (!file)
    {
        printf("Error: could not open file \n");
        return;
    }
    // superblock
    fread(&sb, sizeof(struct superblock), 1, file);

    // inodes
    inodes = malloc(sizeof(struct inode) * sb.num_inodes);
    fread(inodes, sizeof(struct inode), sb.num_inodes, file);

    // disk blocks
    dbs = malloc(sizeof(struct disk_block) * sb.num_blocks);
    fread(dbs, sizeof(struct disk_block), sb.num_blocks, file);

    fclose(file);
}

int allocate_file(const char *name, int size) //from video
{
    // find an empty inode
    int in = find_empty_inode();
    if (in == -1)
    {
        printf("Error: no free inodes\n");
        return -1;
    }

    // find / claim a disk block
    int block = find_empty_block();
    if (block == -1)
    {
        printf("Error: no free blocks\n");
        return -1;
    }

    // claim them
    inodes[in].first_block = block;
    inodes[in].size = size;
    dbs[block].next_block_num = -2;

    strcpy(inodes[in].name, name);

    // return the file descriptor
    return in;
} // allocate_file

int create_file(const char *path, const char *name)
{
    int fd = allocate_file(name, 1);
    if (fd == -1)
    {
        return -1;
    }
    int dirfd = myopendir(path);
    struct mydirent *dir = myreaddir(dirfd);
    opened[fd].pos = 0;
    opened[fd].fd = fd;
    dir->fds[dir->size++] = fd;
    return fd;
}

int create_dir(const char *path, const char *name)
{
    int fd = myopendir(path);
    if (fd == -1)
    {
        printf("Error: could not open directory\n");
        return -1;
    }
    if (inodes[fd].is_dir != 1)
    {
        printf("Error: not a directory\n");
        return -1;
    }
    int dirb = inodes[fd].first_block;
    struct mydirent *dir = (struct mydirent *)dbs[dirb].data;
    if (dir->size == 10)
    {
        printf("Error: directory is full\n");
        return -1;
    }
    int fd2 = allocate_file(name, sizeof(struct mydirent));
    inodes[fd2].is_dir = 1;
    dir->fds[dir->size++] = fd2;
    struct mydirent *new = malloc(sizeof(struct mydirent));
    new->size = 0;
    for (int i = 0; i < 10; i++)
    {
        dir->fds[i] = -1;
    }
    write_byte(fd2, 0, (char *)new);
    strcpy(new->d_name, name);
    return fd2;
}

void write_byte(int filenum, int pos, char *data) // from video
{
    // calculate which block
    int relative_block = pos / BLOCKSIZE;
    // find the block number
    int bn = get_block_num(filenum, relative_block);
    // calculate the offset in the block
    int offset = pos % BLOCKSIZE;
    // write the data
    dbs[bn].data[offset] = (*data);
} // write_byte

char read_byte(int filenum, int pos) // recreating from write_byte fun
{
    int block = inodes[filenum].first_block;
    while (pos > BLOCKSIZE)
    {
        block = dbs[block].next_block_num;
        pos -= BLOCKSIZE;
        if (block == -1 || block == -2)
        {
            return - 1;
        }
    }
    return dbs[block].data[pos];
} // read_byte

void mymkfs(int size) // from video fun called create_fs with adaptation to our filesystem
{
    int no_superblock = size - sizeof(struct superblock);
    int indode_size = no_superblock / 10;
    sb.num_inodes = indode_size / sizeof(struct inode);
    sb.num_blocks = (no_superblock - indode_size) / sizeof(struct disk_block);
    sb.size_blocks = sizeof(struct disk_block);

    int i;
    inodes = malloc(sizeof(struct inode) * sb.num_inodes);
    for (i = 0; i < sb.num_inodes; i++)
    {
        inodes[i].size = -1;
        inodes[i].first_block = -1;
        inodes[i].is_dir = 0;
        strcpy(inodes[i].name, "emptyfi");
    } // init inodes

    dbs = malloc(sizeof(struct disk_block) * sb.num_blocks);
    for (i = 0; i < sb.num_blocks; i++)
    {
        dbs[i].next_block_num = -1;
    } // init disk blocks

    // create root directory
    int rootfd = allocate_file("root", sizeof(struct mydirent));
    if (rootfd)
    {
        printf("Error creating root directory");
    }
    struct mydirent *dir = malloc(sizeof(struct mydirent));
    for (int i = 0; i < 10; i++)
    {
        dir->fds[i] = -1;
    }
    inodes[rootfd].is_dir = 1;
    dir->size = 0;
    strcpy(dir->d_name, "root");
    write_byte(rootfd, 0, (char *)dir);
    opened[rootfd].pos += (sizeof(struct mydirent));

    free(dir);
}

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data)
{
    if (source == NULL && target == NULL)
    {
        printf("Error: source and target cannot be NULL\n");
        return -1;
    }
    if (source != NULL)
    {
        sync_again(source);
    }
    if (target != NULL)
    {
        sync_fs(target);
    }
}

int myopen(const char *pathname, int flags)
{
    if (pathname == NULL)
    {
        printf("Error: path cannot be NULL\n");
        return -1;
    }
    char *str;
    const char slash[2] = "/";
    char path[100];
    strcpy(path, pathname);  
    str = strtok(path, slash);
    char this_path[10] = "";
    char prev_path[10] = "";
    while (str != NULL)
    {
        strcpy(prev_path, this_path);
        strcpy(this_path, str);
        str = strtok(NULL, slash);
    }
    for (int i = 0; i < sb.num_inodes; i++)
    {
        if (strcmp(inodes[i].name, this_path) == 0)
        {
            if (inodes[i].is_dir != 0)
            {
                printf("Error: this is a directory\n");
                return -1;
            }
            opened[i].pos = 0;
            opened[i].fd = i;
            return i;
        }
    }
    int newfd = create_file(prev_path, this_path);
    opened[newfd].fd = newfd;
    opened[newfd].pos = 0;
    return newfd;
}

int myclose(int fd)
{
    if (fd == -1)
    {
        printf("Error: invalid file descriptor in my close\n");
        return -1;
    }
    opened[fd].fd = -1;
    opened[fd].pos = -1;
    return 0;
}

size_t myread(int myfd, void *buf, size_t count)
{
    if (inodes[myfd].is_dir == 1)
    {
        printf("Error: cannot read from directory\n");
        return -1;
    }
    if (opened[myfd].fd != myfd)
    {
        printf("Error: invalid file descriptor in myread\n");
        return -1;
    }
    char *buff = malloc(sizeof(char) * count + 1);
    for (size_t i = 0; i < count; i++)
    {
        buff[i] = read_byte(myfd, opened[myfd].pos);
        opened[myfd].pos++;
    }
    buff[count] = '\0';
    strncpy(buf, buff, count);
    free(buff);
    return opened[myfd].pos;
}

size_t mywrite(int myfd, const void *buf, size_t count)
{
    if (inodes[myfd].is_dir == 1)
    {
        printf("Error: cannot write to directory\n");
        return -1;
    }
    if (opened[myfd].fd != myfd)
    {
        printf("Error: invalid file descriptor in mywrite\n");
        return -1;
    }
    write_byte(myfd, opened[myfd].pos, (char *)buf);
    opened[myfd].pos += count;
    return opened[myfd].pos;
}

off_t mylseek(int myfd, off_t offset, int whence)
{
    if (inodes[myfd].is_dir == 1)
    {
        printf("Error: cannot seek in directory\n");
        return -1;
    }
    if (opened[myfd].fd != myfd)
    {
        printf("Error: invalid file descriptor in mylseek\n");
        return -1;
    }
    if (whence == SEEK_SET)
    {
        opened[myfd].pos = offset;
    }
    else if (whence == SEEK_CUR)
    {
        opened[myfd].pos += offset;
    }
    else if (whence == SEEK_END)
    {
        opened[myfd].pos = inodes[myfd].size + offset;
    }
    if (opened[myfd].pos < 0)
    {
        opened[myfd].pos = 0;
    }

    return opened[myfd].pos;
}

int myopendir(const char *name)
{
    char *str;
    const char slash[2] = "/";
    char path[100];
    strcpy(path, name);
    str = strtok(path, slash);
    char this_path[10] = "";
    char prev_path[10] = "";
    while (str != NULL)
    {
        strcpy(prev_path, this_path);
        strcpy(this_path, str);
        str = strtok(NULL, slash);
    }
    for (int i = 0; i < sb.num_inodes; i++)
    {
        if (strcmp(inodes[i].name, this_path) == 0)
        {
            if (inodes[i].is_dir != 1)
            {
                printf("Error: this is a file\n");
                return -1;
            }
            return i;
        }
    }
    int newfd = create_dir(prev_path, this_path);
    opened[newfd].pos += (sizeof(struct mydirent));
    return newfd;
}

struct mydirent *myreaddir(int dirp)
{
    if (inodes[dirp].is_dir != 1)
    {
        printf("Error: cannot read from file\n");
        return NULL;
    }
    return (struct mydirent *)dbs[inodes[dirp].first_block].data;
}

int myclosedir(int myfd)
{
    printf("Not implemented\n");
    return 0;
}