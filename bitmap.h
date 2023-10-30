#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>

int find_first_free_bit(unsigned char* bit_buf, int num_bytes);

void set_bit(unsigned char* bit_buf, int bit_index);

void clear_bit(unsigned char* bit_buf, int bit_index);

#endif  // BITMAP_H