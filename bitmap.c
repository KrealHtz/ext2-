#include <stdio.h>

int find_first_free_bit(unsigned char* bit_buf, int num_bytes) {
    for (int i = 0; i < num_bytes; i++) {
        if (bit_buf[i] != 0xFF) { 
            unsigned char mask = 1; 

            for (int j = 0; j < 8; j++) {
                if ((bit_buf[i] & mask) == 0) { 
                    return (i * 8) + j; 
                }
                mask <<= 1; 
            }
        }
    }
    return -1; 
}

void set_bit(unsigned char* bit_buf, int bit_index) {
    int byte_index = bit_index / 8;     
    int bit_offset = bit_index % 8;     

    unsigned char mask = 1 << bit_offset;  

    bit_buf[byte_index] |= mask;  
}

void clear_bit(unsigned char* bit_buf, int bit_index) {
    int byte_index = bit_index / 8;     
    int bit_offset = bit_index % 8;     

    unsigned char mask = ~(1 << bit_offset);  

    bit_buf[byte_index] &= mask;   
}