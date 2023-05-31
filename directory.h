#ifndef DIRECTORY_H
#define DIRECTORY_H

#define DIRECTORY_ENTRY_LENGTH 32
#define DIRECTORY_ENTRIES_PER_BLOCK 128
#define ROOT_INODE_NUM 0

struct directory {
    struct inode *inode;
    unsigned int offset;
};

struct directory_entry {
    unsigned int inode_num;
    char name[16];
};

struct directory *directory_open(int inode_num);

int directory_get(struct directory *dir, struct directory_entry *ent);

void directory_close(struct directory *d);

struct inode *namei(char *path);

int directory_make(char *path);

#endif