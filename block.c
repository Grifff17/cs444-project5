#include <fcntl.h>
#include <unistd.h>
#include "block.h"
#include "image.h"
#include "free.h"

unsigned char *bread(int block_num, unsigned char *block) {
    lseek(image_fd, 4096*block_num, SEEK_SET);
    read(image_fd, block, BYTES_IN_BLOCK);
    return block;
}

void bwrite(int block_num, unsigned char *block) {
    lseek(image_fd, 4096*block_num, SEEK_SET);
    write(image_fd, block, BYTES_IN_BLOCK);
}

int alloc(void) {
    unsigned char block[4096];
    bread(BLOCK_MAP_NUM, block);
    int free_num = find_free(block);
    if (free_num == -1) {
        return -1;
    }
    set_free(block, free_num, 1);
    bwrite(BLOCK_MAP_NUM, block);
    return free_num;
}