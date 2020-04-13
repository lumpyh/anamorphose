#include <stdio.h>
#include "bmp.h"

int main()
{
	struct bitmap bmp;
	int ret = readBitmap(&bmp, "test.bmp");
	if (ret)
		return -1;
	
	printf("width = %d\n", bmp.bih.biWidth);
	printf("height = %d\n", bmp.bih.biHeight);

	freeBitmap(&bmp);
	return 0;
}
