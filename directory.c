#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "mkfs.h"
#include "inode.h"
#include "block.h"
#include "free.h"
#include "image.h"
#include "pack.h"
#include "directory.h"

struct directory *directory_open(int inode_num) {
    struct inode *node = iget(inode_num);
    if (node == NULL) {
        return NULL;
    }
    struct directory *dir = malloc(sizeof(struct directory));
    dir->inode = node;
    dir->offset = 0;
    return dir;
}

int directory_get(struct directory *dir, struct directory_entry *ent) {
    if (dir->offset >= dir->inode->size) {
        return -1;
    }
    int data_block_index = dir->offset / 4096;
    int data_block_offset = dir->offset % 4096;
    int data_block_num = dir->inode->block_ptr[data_block_index];
    unsigned char block[BLOCK_SIZE];
    bread(data_block_num, block);
    ent->inode_num = read_u16(block+data_block_offset);
    strcpy(ent->name, (char*)block+data_block_offset+2);
    dir->offset += DIRECTORY_ENTRY_LENGTH;
    return 0;
}

void directory_close(struct directory *d) {
    iput(d->inode);
    free(d);
}