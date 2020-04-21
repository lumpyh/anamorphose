#ifndef BMP_H
#define BMP_H
#include <stdint.h>

#pragma pack(push,1)
struct bitmapFileHeader {
	uint16_t bfType;
	uint32_t bfSize;
	uint32_t bfReserved;
	uint32_t bfOffBits;
};

struct bitmapInfoHeader {
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
};
#pragma pack(pop)

struct pixel {
	unsigned char A;
	unsigned char R;
	unsigned char G;
	unsigned char B;
};


struct bitmap {
	struct bitmapFileHeader bfh;
	struct bitmapInfoHeader bih;
	unsigned char *reserved;
	uint32_t *data;
};

int readBitmap(struct bitmap *bmp, const char* path);
int freeBitmap(struct bitmap *bmp);
int saveBitmap(const struct bitmap *bmp, const char *path);
#endif
