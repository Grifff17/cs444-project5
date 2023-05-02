#include <fcntl.h>
#include <unistd.h>
#include "free.h"

int find_low_clear_bit(unsigned char x)
{
    for (int i = 0; i < 8; i++)
        if (!(x & (1 << i)))
            return i;
    
    return -1;
}

void set_free(unsigned char *block, int num, int set) {
    int byte_num = num / 8; 
    int bit_num = num % 8;
    if (set) {
        block[byte_num] |= 1UL << bit_num;
    } else {
        block[byte_num] &= ~(1UL << bit_num);
    }
}   

int find_free(unsigned char *block) {
    for (int i = 0; i < 4096; i++) {
        int val = find_low_clear_bit(block[i]);
        if (val != -1) {
            return (i * 8) + val;
        }
    }
    return -1;
}