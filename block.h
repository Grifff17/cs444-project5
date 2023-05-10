#ifndef BLOCK_H
#define BLOCK_H

#define BLOCK_MAP_NUM 2
#define BYTES_IN_BLOCK 4096

unsigned char *bread(int block_num, unsigned char *block);
void bwrite(int block_num, unsigned char *block);
int alloc(void);

#endif