#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "mkfs.h"
#include "inode.h"
#include "block.h"
#include "free.h"
#include "image.h"
#include "pack.h"
#include "directory.h"
#include "dirbasename.h"

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
    int data_block_index = dir->offset / BLOCK_SIZE;
    int data_block_offset = dir->offset % BLOCK_SIZE;
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

struct inode *namei(char *path){
    if(strcmp(path, "/") == 0) {
        struct inode* root = iget(ROOT_INODE_NUM);
        return root;
    } else { //add more conditions later
        return NULL;
    }
}

int directory_make(char *path) {

    char dirpath[1024];
    get_dirname(path, dirpath);

    char dirname[1024];
    get_basename(path, dirname);

    struct inode* parentnode = namei(dirpath);
    struct inode* newnode = ialloc();
    int newnode_num = alloc();

    unsigned char block[BLOCK_SIZE];
    write_u16(block, newnode->inode_num);
    strcpy((char*)block+2, ".");
    write_u16(block+DIRECTORY_ENTRY_LENGTH, parentnode->inode_num);
    strcpy((char*)block+2+DIRECTORY_ENTRY_LENGTH, "..");

    newnode->flags = 2;
    newnode->size = DIRECTORY_ENTRY_LENGTH*2;
    newnode->block_ptr[0] = newnode_num;

    bwrite(newnode->inode_num, block);
    int numitems = parentnode->size/DIRECTORY_ENTRY_LENGTH;
    int parent_block_num = numitems/DIRECTORY_ENTRIES_PER_BLOCK;
    unsigned char parentblock[BLOCK_SIZE];
    bread(parentnode->block_ptr[parent_block_num], parentblock);

    write_u16(parentblock+(DIRECTORY_ENTRY_LENGTH*(numitems%128)), newnode->inode_num);
    strcpy((char*)parentblock+2+(DIRECTORY_ENTRY_LENGTH*(numitems%128)), dirname);

    bwrite(parentnode->block_ptr[parent_block_num], parentblock);
    parentnode->size += DIRECTORY_ENTRY_LENGTH;

    iput(parentnode);
    iput(newnode);

    return 0;
}
