#include <stdio.h>

int find_first_free_bit(unsigned char* bitbuf, int num_bytes) {
    for (int i = 0; i < num_bytes; i++) {
        if (bitbuf[i] != 0xFF) { // 检查字节是否有空闲位
            unsigned char mask = 1; // 位掩码，从最低位开始检查

            for (int j = 0; j < 8; j++) {
                if ((bitbuf[i] & mask) == 0) { // 检查位是否为0（空闲）
                    return (i * 8) + j; // 返回位的索引
                }
                mask <<= 1; // 左移位掩码，检查下一位
            }
        }
    }
    return -1; // 没有找到空闲位
}

void set_bit(unsigned char* bitbuf, int bit_index) {
    int byte_index = bit_index / 8;     // 计算位索引对应的字节索引
    int bit_offset = bit_index % 8;     // 计算位索引相对于字节的偏移量

    unsigned char mask = 1 << bit_offset;  // 创建一个掩码，将特定位设置为1

    bitbuf[byte_index] |= mask;   // 将指定位设置为1
}

void clear_bit(unsigned char* bitbuf, int bit_index) {
    int byte_index = bit_index / 8;     // 计算位索引对应的字节索引
    int bit_offset = bit_index % 8;     // 计算位索引相对于字节的偏移量

    unsigned char mask = ~(1 << bit_offset);  // 创建一个掩码，将特定位设置为0

    bitbuf[byte_index] &= mask;   // 将指定位设置为0
}