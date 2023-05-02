#include <fcntl.h>
#include <unistd.h>
#include "inode.h"
#include "block.h"
#include "free.h"
#include "image.h"

int ialloc(void) {
    unsigned char block[4096];

    bread(INODE_MAP_NUM, block);
    int free_num = find_free(block);
    if (free_num == -1) {
        return -1;
    }
    set_free(block, free_num, 1);
    bwrite(INODE_MAP_NUM, block);
    return free_num;
}