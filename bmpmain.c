#include <stdio.h>
#include "bmp.h"


int createbmp(const char* path)
{
	struct bitmap bmp;
	int ret = initBitmap(&bmp, 200, 200);
	printf("ret = %d\n", ret);
	struct pixel *pix = bmp.data;
	printf("sizeof(pixel)=%ld\n",sizeof(struct pixel));	
	printf("pix=%p\n", pix);	
	printf("%d\n",__LINE__);
	for (unsigned int i = 0; i < 200; ++i) {
		printf("%d:%d\n",__LINE__, i);
		for (unsigned int j = 0; j < 200; ++j) {
			printf("%d:%d\n",__LINE__, j);
			*((uint32_t *)pix) = 0;
			if (j < 100 && i < 100)
				pix->R = 255;
		//j	else if (j >= 100 && i < 100)
		//j		pix->G = 255;
		//j	else if (j < 100 && i >= 100)
		//j		pix->B = 255;
			++pix;
			printf("pix=%p\n", pix);	
		}
	}
	printf("%d\n",__LINE__);
	printFileHeader(&bmp.bfh);
	printInfoHeader(&bmp.bih);

	saveBitmap(&bmp, path);
	
	freeBitmap(&bmp);
	return 0;
}

int readbmp(const char* path)
{
	struct bitmap bmp;

	int ret = readBitmap(&bmp, path);
	if (ret)
		return -1;
	
	printf("width = %d\n", bmp.bih.biWidth);
	printf("height = %d\n", bmp.bih.biHeight);

	freeBitmap(&bmp);
	return 0;
}

int main(int argc, char** argv)
{
	if (argv[1][0] == '1') 
		readbmp(argv[2]);
	else
		createbmp(argv[2]);
	
	return 0;
}
