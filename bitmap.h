#ifndef BITMAP_H
#define BITMAP_H

#include <stdbool.h>

typedef struct {
    unsigned int size;  // 位图大小（以位为单位）
    unsigned char *data;  // 位图数据
} Bitmap;

Bitmap *createBitmap(unsigned int size);
void destroyBitmap(Bitmap *bitmap);
void setBit(Bitmap *bitmap, unsigned int position);
bool checkBit(Bitmap *bitmap, unsigned int position);

#endif  /* BITMAP_H */