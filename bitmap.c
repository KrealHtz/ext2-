#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bitmap.h"

// ����ָ����С��λͼ
Bitmap *createBitmap(unsigned int size) {
    Bitmap *bitmap = (Bitmap *)malloc(sizeof(Bitmap));

    // ����λͼ������ֽ���
    unsigned int bytes = size / 8;
    if (size % 8 != 0) {
        bytes++;
    }

    // �����ڴ沢��ʼ��Ϊ0
    bitmap->data = (unsigned char *)calloc(bytes, sizeof(unsigned char));
    bitmap->size = size;

    return bitmap;
}

// ����λͼ
void destroyBitmap(Bitmap *bitmap) {
    free(bitmap->data);
    free(bitmap);
}

// ����ָ��λ�õ�λΪ1
void setBit(Bitmap *bitmap, unsigned int position) {
    if (position >= bitmap->size) {
        return;
    }

    unsigned int byteIndex = position / 8;
    unsigned int bitIndex = position % 8;
    bitmap->data[byteIndex] |= (1 << bitIndex);
}

// ���ָ��λ�õ�λ�Ƿ�Ϊ1
bool checkBit(Bitmap *bitmap, unsigned int position) {
    if (position >= bitmap->size) {
        return false;
    }

    unsigned int byteIndex = position / 8;
    unsigned int bitIndex = position % 8;
    return (bitmap->data[byteIndex] & (1 << bitIndex)) != 0;
}

int main() {
    // ����һ����СΪ16��λͼ
    Bitmap *bitmap = createBitmap(16);

    // ���õ�3λ�͵�9λΪ1
    setBit(bitmap, 2);
    setBit(bitmap, 8);

    // ����5λ�͵�9λ�Ƿ�Ϊ1
    printf("Bit 4: %d\n", checkBit(bitmap, 4));
    printf("Bit 8: %d\n", checkBit(bitmap, 8));

    // ����λͼ
    destroyBitmap(bitmap);

    return 0;
}