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
#include "pack.h"
#include "directory.h"
#include "dirbasename.h"
#include "ls.h"


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

    struct inode *node5 = ialloc();
    CTEST_ASSERT(node5->inode_num == 0, "testing ialloc with empty inode map");

    struct inode *node6 = ialloc();
    CTEST_ASSERT(node6->inode_num == 1, "testing ialloc with non-empty inode map");

    struct inode *node = find_incore_free();
    node->size = 5;
    node->inode_num = 2;
    node->ref_count = 1;
    CTEST_ASSERT(find_incore(2)->size == 5, "testing find_icore_free and find_icore");

    write_inode(node);
    struct inode node2;
    read_inode(&node2, 2);
    CTEST_ASSERT(node2.size == 5, "testing write_inode and read_inode");

    CTEST_ASSERT(iget(2)->size == 5, "testing iget for node in core");

    struct inode* node3 = iget(3);
    CTEST_ASSERT(find_incore(3)->ref_count == 1, "testing iget for node not in core");

    iget(3);
    iput(node3);
    CTEST_ASSERT(node3->ref_count == 1, "testing iput for ref_count > 1");

    node3->size = 6;
    iput(node3);
    struct inode node4;
    read_inode(&node4, 3);
    CTEST_ASSERT(node4.size == 6, "testing iput for ref_count == 1");

    image_close();
}

void test_mkfs(void) {
    image_open("mkfs_testfile", 0);
    unsigned char outblock[4096];
    unsigned char block[4096] = {0};
    mkfs();

    CTEST_ASSERT(memcmp(bread(8, outblock), block, BYTES_IN_BLOCK) == 0, "testing mkfs sets all blocks to 0");

    CTEST_ASSERT(alloc() == 8, "testing mkfs allocates blocks correctly");

    struct directory *dir;
    struct directory_entry ent;
    dir = directory_open(0);
    directory_get(dir, &ent);
    CTEST_ASSERT(strcmp(ent.name, ".") == 0, "testing mkfs creates root directory, as well as open, get, and close directory");
    directory_close(dir);
    
    image_close();
}

void test_directory(void) {
    image_open("directory_testfile", 0);
    mkfs();

    struct inode *in = namei("/");
    CTEST_ASSERT(in->inode_num == 0, "testing namei on root directory");

    directory_make("/foo");
    struct directory *dir;
    struct directory_entry ent;
    dir = directory_open(0);
    directory_get(dir, &ent);
    directory_get(dir, &ent);
    directory_get(dir, &ent);
    CTEST_ASSERT(strcmp(ent.name, "foo") == 0, "testing directory_make creates directory");
    directory_close(dir);

    image_close();
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_block();

    test_free();

    test_mkfs();

    test_inode();

    test_directory();

    CTEST_RESULTS();

    CTEST_EXIT();
}