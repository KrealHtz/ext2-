#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bitmap.h"

// 创建指定大小的位图
Bitmap *createBitmap(unsigned int size) {
    Bitmap *bitmap = (Bitmap *)malloc(sizeof(Bitmap));

    // 计算位图所需的字节数
    unsigned int bytes = size / 8;
    if (size % 8 != 0) {
        bytes++;
    }

    // 分配内存并初始化为0
    bitmap->data = (unsigned char *)calloc(bytes, sizeof(unsigned char));
    bitmap->size = size;

    return bitmap;
}

// 销毁位图
void destroyBitmap(Bitmap *bitmap) {
    free(bitmap->data);
    free(bitmap);
}

// 设置指定位置的位为1
void setBit(Bitmap *bitmap, unsigned int position) {
    if (position >= bitmap->size) {
        return;
    }

    unsigned int byteIndex = position / 8;
    unsigned int bitIndex = position % 8;
    bitmap->data[byteIndex] |= (1 << bitIndex);
}

// 检查指定位置的位是否为1
bool checkBit(Bitmap *bitmap, unsigned int position) {
    if (position >= bitmap->size) {
        return false;
    }

    unsigned int byteIndex = position / 8;
    unsigned int bitIndex = position % 8;
    return (bitmap->data[byteIndex] & (1 << bitIndex)) != 0;
}

int main() {
    // 创建一个大小为16的位图
    Bitmap *bitmap = createBitmap(16);

    // 设置第3位和第9位为1
    setBit(bitmap, 2);
    setBit(bitmap, 8);

    // 检查第5位和第9位是否为1
    printf("Bit 4: %d\n", checkBit(bitmap, 4));
    printf("Bit 8: %d\n", checkBit(bitmap, 8));

    // 销毁位图
    destroyBitmap(bitmap);

    return 0;
}