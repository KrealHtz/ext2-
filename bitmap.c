#include <stdio.h>

int find_first_free_bit(unsigned char* bitbuf, int num_bytes) {
    for (int i = 0; i < num_bytes; i++) {
        if (bitbuf[i] != 0xFF) { // ����ֽ��Ƿ��п���λ
            unsigned char mask = 1; // λ���룬�����λ��ʼ���

            for (int j = 0; j < 8; j++) {
                if ((bitbuf[i] & mask) == 0) { // ���λ�Ƿ�Ϊ0�����У�
                    return (i * 8) + j; // ����λ������
                }
                mask <<= 1; // ����λ���룬�����һλ
            }
        }
    }
    return -1; // û���ҵ�����λ
}

void set_bit(unsigned char* bitbuf, int bit_index) {
    int byte_index = bit_index / 8;     // ����λ������Ӧ���ֽ�����
    int bit_offset = bit_index % 8;     // ����λ����������ֽڵ�ƫ����

    unsigned char mask = 1 << bit_offset;  // ����һ�����룬���ض�λ����Ϊ1

    bitbuf[byte_index] |= mask;   // ��ָ��λ����Ϊ1
}

void clear_bit(unsigned char* bitbuf, int bit_index) {
    int byte_index = bit_index / 8;     // ����λ������Ӧ���ֽ�����
    int bit_offset = bit_index % 8;     // ����λ����������ֽڵ�ƫ����

    unsigned char mask = ~(1 << bit_offset);  // ����һ�����룬���ض�λ����Ϊ0

    bitbuf[byte_index] &= mask;   // ��ָ��λ����Ϊ0
}