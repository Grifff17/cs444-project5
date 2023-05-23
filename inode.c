#include <fcntl.h>
#include <unistd.h>
#include "inode.h"
#include "block.h"
#include "free.h"
#include "image.h"
#include "pack.h"

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

struct inode *ialloc(void) {
    unsigned char block[4096];

    bread(INODE_MAP_NUM, block);
    int free_num = find_free(block);
    if (free_num == -1) {
        return NULL;
    }
    set_free(block, free_num, 1);
    bwrite(INODE_MAP_NUM, block);

    struct inode *node = iget(free_num);
    if (node == NULL) {
        return NULL;
    }
    node->size = 0;
    node->owner_id = 0;
    node->permissions = 0;
    node->flags = 0;
    for(int i = 0; i < INODE_PTR_COUNT; i++) {
        node->block_ptr[i] = 0;
    }
    node->inode_num = free_num;
    write_inode(node);

    return node;
}

struct inode *find_incore_free(void) {
    int ref = 1;
    int i = -1;
    while (ref != 0 && i < MAX_SYS_OPEN_FILES) {
        i++;
        ref = incore[i].ref_count;
    }
    if (i < MAX_SYS_OPEN_FILES) {
        struct inode *node = &incore[i];
        return node;
    } else {
        return NULL;
    }
}

struct inode *find_incore(unsigned int inode_num) {
    int ref = 0;
    unsigned int num = -1;
    int i = -1;
    while ((ref == 0 || num != inode_num) && i < MAX_SYS_OPEN_FILES) {
        i++;
        ref = incore[i].ref_count;
        num = incore[i].inode_num;
    }
    if (i < MAX_SYS_OPEN_FILES) {
        struct inode *node = &incore[i];
        return node;
    } else {
        return NULL;
    }
}

void read_inode(struct inode *in, int inode_num) {
    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset = inode_num % INODES_PER_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE;
    
    unsigned char readblock[4096];
    bread(block_num, readblock);
    
    in->size = read_u32(readblock + block_offset_bytes + 0);
    in->owner_id = read_u16(readblock + block_offset_bytes + 4);
    in->permissions = read_u8(readblock + block_offset_bytes + 6);
    in->flags = read_u8(readblock + block_offset_bytes + 7);
    in->link_count = read_u8(readblock + block_offset_bytes + 8);
    for(int i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = read_u16(readblock + block_offset_bytes + 9 + (i*2));
    }
}

void write_inode(struct inode *in) {
    int block_num = in->inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset = in->inode_num % INODES_PER_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE;
    
    unsigned char readblock[4096];
    bread(block_num, readblock);
    
    write_u32(readblock + block_offset_bytes + 0, in -> size);
    write_u16(readblock + block_offset_bytes + 4, in -> owner_id);
    write_u8(readblock + block_offset_bytes + 6, in -> permissions);
    write_u8(readblock + block_offset_bytes + 7, in -> flags);
    write_u8(readblock + block_offset_bytes + 8, in -> link_count);
    
    bwrite(block_num, readblock);
}

struct inode *iget(int inode_num) {
    struct inode *incore_node = find_incore(inode_num);
    if (incore_node != NULL) {
        incore_node->ref_count++;
        return incore_node;
    }
    struct inode *incore_node_new = find_incore_free();
    if (incore_node_new == NULL) {
        return NULL;
    }
    read_inode(incore_node_new, inode_num);
    incore_node_new->ref_count = 1;
    incore_node_new->inode_num = inode_num;
    return incore_node_new;
}

void iput(struct inode *in) {
    if (in->ref_count == 0) {
        return;
    }
    in->ref_count--;
    if (in->ref_count == 0) {
        write_inode(in);
    }
}