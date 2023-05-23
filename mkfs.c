#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mkfs.h"
#include "inode.h"
#include "block.h"
#include "free.h"
#include "image.h"
#include "pack.h"
#include "directory.h"
#include "ls.h"

void mkfs(void) {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        unsigned char block[4096] = {0};
        write(image_fd, block, sizeof(block));
    }
    for (int i = 0; i < 7; i++) {
        alloc();
    }

    struct inode *rootnode = ialloc();
    int rootblock = alloc();
    rootnode->flags = 2;
    rootnode->size = DIRECTORY_ENTRY_LENGTH*2;
    rootnode->block_ptr[0] = rootblock;
    unsigned char block[BLOCK_SIZE];
    write_u16(block, rootnode->inode_num);
    strcpy((char*)block+2, ".");
    write_u16(block+DIRECTORY_ENTRY_LENGTH, rootnode->inode_num);
    strcpy((char*)block+2+DIRECTORY_ENTRY_LENGTH, "..");   
    bwrite(rootnode->inode_num, block);
    iput(rootnode);
}

// int main(void) {
//     image_open("mkfs_testfile", 0);
//     mkfs();
//     ls();
//     image_close();
//     return 0;   
// }