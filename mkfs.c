#include <fcntl.h>
#include <unistd.h>
#include "mkfs.h"
#include "inode.h"
#include "block.h"
#include "free.h"
#include "image.h"

void mkfs(void) {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        unsigned char block[4096] = {0};
        write(image_fd, block, sizeof(block));
    }
    for (int i = 0; i < 7; i++) {
        alloc();
    }
}