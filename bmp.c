#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bmp.h"

void printFileHeader( const struct bitmapFileHeader *bfh)
{
	printf("FileHeader");
	printf("bfType %d\n", bfh->bfType);
	printf("bfSize %d\n", bfh->bfSize);
	printf("bfReserved %d\n", bfh->bfReserved);
	printf("bfOffBits %d\n", bfh->bfOffBits);
}


void printInfoHeader(const struct bitmapInfoHeader *bih)
{
	printf("Infoheader\n");
	printf("biSize = %d\n", bih->biSize);
	printf("biWidth = %d\n", bih->biWidth);
	printf("biHeight = %d\n", bih->biHeight);
	printf("biPlanes = %d\n", bih->biPlanes);
	printf("biBitCount = %d\n", bih->biBitCount);
	printf("biCompression = %d\n", bih->biCompression);
	printf("biSizeImage = %d\n", bih->biSizeImage);
	printf("biXPelsPerMeter = %d\n", bih->biXPelsPerMeter);
	printf("biYPelsPerMeter = %d\n", bih->biYPelsPerMeter);
	printf("biClrUsed = %d\n", bih->biClrUsed);
	printf("biClrImportant = %d\n", bih->biClrImportant);
}

int initBitmap(struct bitmap *bmp, unsigned int height, unsigned int wight)
{
	bmp->bfh->bfType = 0x424D;
	bmp->bfh->bfType = 0x424D;
	bmp->bfh->bfType = 0x424D;
mp->bfh->bfType = 0x424D;

}


int readBitmap(struct bitmap *bmp, const char* path)
{
	int ret = 0;
	bmp->reserved = NULL;
	bmp->data = NULL;
	unsigned int posCount = 0;
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		printf("%s: cant open file(%s) errno(%d)\n", __func__, path, errno); 
		return -1;
	}

	size_t n = read(fd, &bmp->bfh, sizeof(bmp->bfh));
	if (n < sizeof(bmp->bfh)) {
		printf("%s: cant read fileheader n(%ld), errno(%d)\n", __func__, n, errno); 
		ret = -1;
		goto close;
	}
	printFileHeader(&bmp->bfh);
	//TODO BM in first two bytes

	n = read(fd, &bmp->bih, sizeof(bmp->bih));
	if (n < sizeof(bmp->bfh)) {
		printf("%s: cant read bmp infoheader n(%ld), errno(%d)\n", __func__, n, errno); 
		ret = -1;
		goto close;
	}
	posCount += n;
	printInfoHeader(&bmp->bih);

	int toRead = bmp->bfh.bfOffBits - posCount; 
	bmp->reserved = malloc(toRead);
	if (bmp->reserved == NULL)
	{
		printf("%s: cant malloc reserved Data errno(%d)", __func__, errno);
		ret =  -1;
		goto close;
	}

	n = read(fd, bmp->reserved, toRead);
	if (n < toRead) {
		printf("%s: cant read reserved data n(%ld), errno(%d)\n", __func__, n, errno); 
		ret = -1;
		goto close;
	}
	posCount += n;

	size_t datalen = bmp->bih.biWidth * bmp->bih.biHeight * 4;
		
	bmp->data = malloc(datalen);
	if (bmp->data == NULL)
	{
		printf("%s: cant malloc data Data errno(%d)\n", __func__, errno);
		ret = -1;
		goto close;
	}
	n = read(fd, bmp->data, datalen);
	if (n < 0) {
		printf("%s: cant read data data n(%ld), errno(%d)\n", __func__, n, errno); 
		ret = -1;
		goto close;
	}
	posCount += n;
	
close:
	if (ret != 0 && bmp->data != NULL)
		free(bmp->data);	
	if (ret != 0 && bmp->reserved != NULL)
		free(bmp->reserved);

	close(fd);
	return ret;
}

int freeBitmap(struct bitmap *bmp)
{
	if (bmp->reserved)
		free(bmp->reserved);
	if (bmp->data)
		free(bmp->data);
	return 0;
}


int saveBitmap(const struct bitmap *bmp, const char *path)
{
	int fd = open(path, O_CREATE | OWDONLY);
	if (fd < 0) {
		printf("%s: cant open path(%s), errno(%d)", __func__, path, errno);	
		return -1;
	}
	int sum = 0;
	int n = 0;
	int ret = 0;
	do {
		n = write(fd, bmp + sum, bmp->bfh->bfSize - sum);
		if (n < 0 && errno != EINTER) {
			printf("%s: error accured while writing errno(%d)", __func__, errno);
			ret = -1;
			break;
		}
		sum += n > 0 ? 0 : n;	
	}while (sum != bmp->bfh->bfSize);
	
	close(fd);
	return ret;
}
