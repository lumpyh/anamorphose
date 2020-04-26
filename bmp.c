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

int initBitmap(struct bitmap *bmp, unsigned int height, unsigned int width)
{
	bmp->bfh.bfType = 0x4D42;
	bmp->bfh.bfSize = sizeof(bmp->bfh) + sizeof(bmp->bih) + height * width * sizeof(uint32_t);
	bmp->bfh.bfReserved = 0;
	bmp->bfh.bfOffBits = sizeof(bmp->bfh) + sizeof(bmp->bih);

	bmp->bih.biSize = sizeof(bmp->bih);
	bmp->bih.biWidth = width;
	bmp->bih.biHeight = height;
	bmp->bih.biPlanes = 1;
	bmp->bih.biBitCount = 32;
	bmp->bih.biCompression = 0; //BI_RGB no compression
	bmp->bih.biSizeImage = 0; 
	bmp->bih.biXPelsPerMeter = 20; //TODO
	bmp->bih.biYPelsPerMeter = 20; //TODO
	bmp->bih.biClrUsed = 0;
	bmp->bih.biClrImportant = 0;

	bmp->reserved = NULL;
	bmp->data = malloc(height * width * sizeof(uint32_t));
	if (bmp->data == NULL) {
		printf("%s: cant allocate mem for bitmap data errno=%d\n", __func__, errno);
		return -1;
	}	
	return 0;
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
	int fd = open(path, O_CREAT | O_WRONLY);
	if (fd < 0) {
		printf("%s: cant open path(%s), errno(%d)", __func__, path, errno);	
		return -1;
	}
	int sum = 0;
	int n = 0;
	int ret = 0;

	n = write(fd, &bmp->bfh, sizeof(bmp->bfh));
	if (n != sizeof(bmp->bfh)) {
		printf("%s: failed to write the fileheader n=%d (if n < 0 -> errno=%d)\n", __func__, n, errno);
	}
	
	n = write(fd, &bmp->bih, sizeof(bmp->bih));
	if (n != sizeof(bmp->bih)) {
		printf("%s: failed to write the infoheader n=%d (if n < 0 -> errno=%d)\n", __func__, n, errno);
	}

	int len = bmp->bfh.bfOffBits - sizeof(bmp->bfh) - sizeof(bmp->bih);
	if (bmp->reserved != NULL) {
		do {
			n = write(fd, ((char *)bmp->reserved) + sum, len - sum);
			if (n < 0 && errno != EINTR) {
				printf("%s: error accured while writing reserved errno(%d)", __func__, errno);
				ret = -1;
				break;
			}
			sum += n > 0 ? n : 0;	
		}while (sum < len);
	}
		
	len = bmp->bfh.bfSize - bmp->bfh.bfOffBits; 	
	
	do {
		n = write(fd, bmp->data + sum, len - sum);
		printf("n = %d\n", n); 
		if (n < 0 && errno != EINTR) {
			printf("%s: error accured while writing errno(%d)", __func__, errno);
			ret = -1;
			break;
		}
		sum += n > 0 ? n : 0;	
	}while (sum < len);
	
	close(fd);
	return ret;
}
