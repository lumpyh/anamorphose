#include <X11/Xlib.h>
#include <stdio.h>
#include <math.h>

#include <stdlib.h>
#include <string.h>

#include "bmp.h"

#define mm *1366/256

struct point {
	double x;
	double y;
}

struct circle {
	double r;
	struct point center;
};

struct setup {
	struct point view;
	struct circle;
}

struct size {
	int widthpix;
	int heightpix;
	double width;
	double height;
}


/**
 * put pixel in as pos get realxy out
 */
struct point firsttransorm( struct point pos, const struct size *size, const struct setup *setup)
{
	double x = setup.circle.center.x - setup.circle.r + pos * 2 * setup.circle.r / size.widthpix;
	double y = setup.circle.center.y + size.width / (setup.circle.r * 2) * size.hight * pos.y / size.height;
}

struct double 


int drawCircle( float x0, float y0, float r_min, float r_max, struct pixel color, struct pixel *data, unsigned int w, unsigned int h)
{
	unsigned int y_min = (unsigned int) (y0 - r_max);
	if (y_min >= h)
		return 0;
	y_min = y_min < 0 ? 0 : y_min;
	unsigned int y_max = (unsigned int) (y0 + r_max);
	if (y_max < 0)
		return 0;
	y_max = y_max > h ? h : y_max;
	
	for (unsigned int iy = y_min; iy < y_max; ++iy) {
		if ((r_min*r_min - (iy - y0)*(iy -y0)) > 0) {
			unsigned int x_min = x0 - sqrt(r_max*r_max - (iy - y0)*(iy -y0));	 
			unsigned int x_max = x0 - sqrt(r_min*r_min - (iy - y0)*(iy -y0));	 
			x_min = x_min < 0 ? 0: x_min;
			x_max = x_max > w ? w: x_max;
			for (int ix = x_min; ix < x_max; ++ix) {
				data[iy * w + ix] = color;
			}

			x_max = x0 + sqrt(r_max*r_max - (iy - y0)*(iy -y0));	 
			x_min = x0 + sqrt(r_min*r_min - (iy - y0)*(iy -y0));	 
			x_min = x_min < 0 ? 0: x_min;
			x_max = x_max > w ? w: x_max;
			for (int ix = x_min; ix < x_max; ++ix) {
				data[iy * w + ix] = color;
			}
		} else if ((r_max*r_max - (iy - y0)*(iy -y0)) > 0) {
			unsigned int x_max = x0 + sqrt(r_max*r_max - (iy - y0)*(iy -y0));	 
			unsigned int x_min = x0 - sqrt(r_max*r_max - (iy - y0)*(iy -y0));	 
			x_min = x_min < 0 ? 0: x_min;
			x_max = x_max > w ? w: x_max;
			for (int ix = x_min; ix < x_max; ++ix) {
				data[iy * w + ix] = color;
			}
		}
	}
	return 0;
}	

int transpixel(int xi, int yi, int *xo, int *yo, float x0, float y0, float xa, float ya)
{
	*xo = xi * xa + x0;
	*yo = yi * ya + y0;
	return 0;
}

struct floatCountPixel {
	unsigned int R;
	unsigned int G;
	unsigned int B;
	unsigned int count;
};

int transform(struct pixel *data, unsigned int w, unsigned int h)
{
	size_t npix = sizeof(struct floatCountPixel) * w * h;
	struct floatCountPixel *pix = malloc(npix);
	memset(pix, 0, npix);
	for (int ix = 0; ix < w; ++ix) {
		for (int iy = 0; iy < h; ++iy) {
			int xo, yo;
			transpixel(ix, iy, &xo, &yo, w / 3, h / 3, 0.333333, 0.33333);			
			pix[xo + w * yo].R += data[ix + w * iy].R;
			pix[xo + w * yo].G += data[ix + w * iy].G;
			pix[xo + w * yo].B += data[ix + w * iy].B;
			pix[xo + w * yo].count++;
		}
	}
	for (int ix = 0; ix < w; ++ix) {
		for (int iy = 0; iy < h; ++iy) {
			int count = pix[ix + w * iy].count;
			count = count == 0 ? 1 : count;
			data[ix + w * iy].R = pix[ix + w * iy].R / count;
			data[ix + w * iy].G = pix[ix + w * iy].G / count;
			data[ix + w * iy].B = pix[ix + w * iy].B / count;
		}
	}


	free(pix);
}

int main(int argc, char** argv)
{
	struct bitmap bmp;
	struct pixel pix = { 0, 0, 255, 0};
	readBitmap(&bmp, argv[1]);

	int h = bmp.bih.biHeight;
	int w = bmp.bih.biWidth;
	
	transform(bmp.data, w, h);
	//drawCircle(w / 2, h / 2, h / 7, h / 6, pix, bmp.data, w, h);


	saveBitmap(&bmp, argv[2]);
			
	freeBitmap(&bmp);
	return 0;
}
