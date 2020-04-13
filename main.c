#include <X11/Xlib.h>
#include <stdio.h>
#include <math.h>

#include <stdio.h>

#define mm *1366/256

#pragma puah(1)
struct bitmapFileHeader {
	uint16_t bfType;
	uint32_t bfSize;
	uint32_t bfReserved;
	uint32_t bfOffBits;
}

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
}
#pragma pop

struct bitmap {
	struct bitmapFileHeader bfh;
	struct bitmapInfoHeader bih;
	struct unsigned char *reserved;
	struct uint32_t *data;
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
		ret = -1;
	}

	size_t n = read(fd, &bmp->bfh, sizeof(bitmap->bfh));
	if (n < sizeof(bitmap->bfh)) {
		printf("%s: cant read fileheader n(%d), errno(%d)\n", __func__, n, errno); 
		ret = -1;
	}
	posCount += n;
	//TODO BM in first two bytes

	n = read(fd, &bmp->bfh, sizeof(bitmap->bfh));
	if (n < sizeof(bitmap->bfh)) {
		printf("%s: cant read fileheader n(%d), errno(%d)\n", __func__, n, errno); 
		ret = -1;
	}
	posCount += n;

	int toRead = bmp->bfh.bfOffBits - posCount; 
	bmp->reserved = malloc(toRead);
	if (bmp->reserved == NULL)
	{
		printf("%s: cant malloc reserved Data errno(%d)", __func__, errno);
		ret =  -1;
	}

	n = read(fd, bmp->reserved, toRead);
	if (n < toRead) {
		printf("%s: cant read fileheader n(%d), errno(%d)\n", __func__, n, errno); 
		ret = -1;
	}
	posCount += n;

	size_t datalen = bmp->bih.biWidth * bmp->bih.biHeight * 4;
		
	bmp->data = malloc(datalen);
	if (bmp->data == NULL)
	{
		printf("%s: cant malloc data Data errno(%d)\n", __func__, errno);
		ret = -1;
	}
	n = read(fd, bmp->data, datalen);
	if (n < 0) {
		printf("%s: cant read fileheader n(%d), errno(%d)\n", __func__, n, errno); 
		ret = -1;
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



int solveQuadratic(double a, double b, double c, double* ret)
{
	ret[0] = sqrt(b*b -4*a*c);
	ret[1] = - ret[0];
	ret[0] -= b;
	ret[1] -= b;
	ret[0] /= 2 * a;
	ret[1] /= 2 * a;
	return 0;
}

struct data {
	double r;
	double k[3];
};

int printPoint( struct data* d, double* x, double t)
{
	double res[3];	
	for (int i = 0; i < 3; ++i) {
		res[i] = x[i] * (1 - t) + d->k[i] * t;
	}
	printf("point %d,%d\n", res[0], res[1]);
}

int calc( struct data* d, double* x)
{
	double rr = d->r * d->r;
	double xx = x[0] * x[0] + x[1] * x[1];
	double k_dash[2];
	k_dash[0] = d->k[0] - x[0];
	k_dash[1] = d->k[1] - x[1];
	double kk = k_dash[0] * k_dash[0] + k_dash[1] * k_dash[1];
	double kx = k_dash[0] * x[0] + k_dash[1] * x[1];
	
	double t[2];
	solveQuadratic( kk, 2 * kx, xx -rr, t);

	printPoint( d, x, t[0]);	
	printPoint( d, x, t[1]);	
	return 0;
} 

int main()
{
	struct data d;
	d.r = 2;
	d.k[0] = 100;
	d.k[1] = 0;
	d.k[2] = 0;
	double x[] = { -10, 0 , 0};
	calc(&d, x);	
	x[1] = 1;
	calc(&d, x);	
	return 0;

	Display* dpy = XOpenDisplay(0);

	Window win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 200, 0, 800, 600, 0, 0, 0x0);

	XMapWindow(dpy, win);

	XFlush(dpy);

	XGCValues vals;
	vals.foreground = WhitePixel(dpy, 0);
	vals.background = BlackPixel(dpy, 0);
	vals.line_style = LineSolid;
	vals.line_width = 30;
	GC gc = XCreateGC(dpy, win, GCForeground | GCBackground, &vals);

	while (1) {
		int ret = XDrawArc(dpy, win, gc, 30 mm, 30 mm, 20 mm, 20 mm, 0, 360*64);
		//printf("ret = %d\n", ret);
		XFlush(dpy);
	}

	return 0;
}
