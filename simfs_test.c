#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "ctest.h"
#include "inode.h"
#include "block.h"
#include "free.h"
#include "image.h"
#include "mkfs.h"


void test_block(void) {
    image_open("block_testfile", 0);
    unsigned char outblock[4096];

    unsigned char inblock[4096] = "asdf";
    bwrite(15, inblock);
    CTEST_ASSERT(memcmp(inblock, bread(15, outblock), BYTES_IN_BLOCK) == 0, "testing block read and write");

    unsigned char inblock2[4096] = "ghjk";
    bwrite(15, inblock2);
    CTEST_ASSERT(memcmp(inblock2, bread(15, outblock), BYTES_IN_BLOCK) == 0, "testing block overwriting");

    CTEST_ASSERT(alloc() == 0, "testing alloc with empty block map");

    CTEST_ASSERT(alloc() == 1, "testing alloc with non-empty block map");

    image_close();
}

void test_free(void) {
    unsigned char block[4096] = {0};

    CTEST_ASSERT(find_free(block) == 0, "testing find_free for empty block");

    set_free(block, 0, 1);
    CTEST_ASSERT(find_free(block) == 1, "testing find_free and set_free to 1");

    set_free(block, 1, 1);
    CTEST_ASSERT(find_free(block) == 2, "testing find_free and set_free to 1 for block with preexisting 1s");

    set_free(block, 0, 0);
    CTEST_ASSERT(find_free(block) == 0, "testing set_free to 0 and find_free when 1s further in array");
}

void test_inode(void) {
    image_open("inode_testfile", 0);

    CTEST_ASSERT(ialloc() == 0, "testing ialloc with empty inode map");

    CTEST_ASSERT(ialloc() == 1, "testing ialloc with non-empty inode map");

    image_close();
}

void test_mkfs(void) {
    image_open("mkfs_testfile", 0);
    unsigned char outblock[4096];
    unsigned char block[4096] = {0};
    mkfs();

    CTEST_ASSERT(memcmp(bread(8, outblock), block, BYTES_IN_BLOCK) == 0, "testing mkfs sets all blocks to 0");

    CTEST_ASSERT(alloc() == 7, "testing mkfs allocates blocks correctly");
    
    image_close();
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_block();

    test_free();

    test_inode();

    test_mkfs();

    CTEST_RESULTS();

    CTEST_EXIT();
}