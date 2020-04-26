#include <X11/Xlib.h>
#include <stdio.h>
#include <math.h>

#include <stdio.h>

#include "bmp.h"

#define mm *1366/256

int drawCircle( float x0, float y0, float r_min, float, r_max, struct pixel color, struct pixel *data, unsigned int w, unsigned int h)
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
		unsigned int x_min = x0 - sqrt(r_max*r_max - (iy - y0)*(iy -y0));	 
		unsigned int x_max = x0 - sqrt(r_min*r_min - (iy - y0)*(iy -y0));	 
		x_min = x_min < 0 ? 0: x_min;
		x_max = x_max > w ? w: x_max;
		for (int ix = x_min; ix < x_max; ++ix) {
			data[iy * w + ix] = *data;
		}

		unsigned int x_max = x0 + sqrt(r_max*r_max - (iy - y0)*(iy -y0));	 
		unsigned int x_min = x0 + sqrt(r_min*r_min - (iy - y0)*(iy -y0));	 
		x_min = x_min < 0 ? 0: x_min;
		x_max = x_max > w ? w: x_max;
		for (int ix = x_min; ix < x_max; ++ix) {
			data[iy * w + ix] = *data;
		}
	}
	return 0;
	
}	

int main(int argc, char** argv)
{
	struct bitmap bmp;
	struct pixel pix = 
	readBitmap(&bmp, argv[1]);

	freeBitmap(&bmp);
	return 0;
}
