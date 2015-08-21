/*
 * BB: The portable demo
 *
 * (C) 1997 by AA-group (e-mail: aa@horac.ta.jcu.cz)
 *
 * 3rd August 1997
 * version: 1.2 [final3]
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licences as by published
 * by the Free Software Foundation; either version 2; or (at your option)
 * any later version
 *
 * This program is distributed in the hope that it will entertaining,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILTY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Publis License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <math.h>
#include "minilzo.h"
#include <string.h>
#include <stdlib.h>
#include "bb.h"
#include "image.h"
typedef double atype;
static void fastcscale(char *b1, char *b2, int x1, int x2, int y1, int y2, int width1, int width2)
{
    int ddx1, ddx, spx = 0, ex;
    int ddy1, ddy, spy = 0, ey;
    int x;
    char *bb1 = b1;
    width2 -= x2;
    if (!x1 || !x2 || !y1 || !y2)
	return;
    ddx = x1 + x1;
    ddx1 = x2 + x2;
    if (ddx1 < ddx)
	spx = ddx / ddx1, ddx %= ddx1;
    ddy = y1 + y1;
    ddy1 = y2 + y2;
    if (ddy1 < ddy)
	spy = (ddy / ddy1) * width1, ddy %= ddy1;
    ey = -ddy1;
    for (; y2; y2--) {
	ex = -ddx1;
	for (x = x2; x; x--) {
	    *b2 = *b1;
	    b2++;
	    b1 += spx;
	    ex += ddx;
	    if (ex > 0) {
		b1++;
		ex -= ddx1;
	    }
	}
	b2 += width2;
	bb1 += spy;
	ey += ddy;
	if (ey > 0) {
	    bb1 += width1;
	    ey -= ddy1;
	}
	b1 = bb1;
    }
}

void scale(struct image *image, int x1, int y1, int x2, int y2)
{
    aa_context *c = context;
    int width = aa_imgwidth(c);
    int height = aa_imgheight(c);
    int imgwidth = image->width;
    int imgheight = image->height;
    int d = 0;
    static atype xstep, ystep;
    char *imgdata;
    if (x2 < 0 || x1 >= imgwidth || y2 < 0 || y1 >= imgheight) {
	memset(c->imagebuffer, 0, aa_imgwidth(c) * aa_imgheight(c));
	return;
    }
    if (image->decompressed == NULL)
	decompressimg(image), d = 1;
    imgdata = image->decompressed;
    if (x1 < 0 || y1 < 0 || x2 >= imgwidth || y2 >= imgheight) {
	int xx1 = 0, xx2 = aa_imgwidth(c) - 1, yy1 = 0, yy2 = aa_imgheight(c) - 1;
	memset(c->imagebuffer, 0, aa_imgwidth(c) * aa_imgheight(c));
	xstep = ((atype) width) / (x2 - x1 - 1);
	ystep = ((atype) height) / (y2 - y1 - 1);
	if (x2 >= imgwidth) {
	    xx2 = (imgwidth - 1 - x1) * xstep;
	    x2 = imgwidth - 1;
	}
	if (y2 >= imgheight) {
	    yy2 = (imgheight - 1 - y1) * ystep;
	    y2 = imgheight - 1;
	}
	if (x1 < 0) {
	    xx1 = -xstep * x1;
	    x1 = 0;
	}
	if (y1 < 0) {
	    yy1 = -ystep * y1;
	    y1 = 0;
	}
	fastcscale(imgdata + x1 + imgwidth * y1, c->imagebuffer + xx1 + yy1 * width, x2 - x1, xx2 - xx1, y2 - y1, yy2 - yy1, imgwidth, aa_imgwidth(c));
	return;
    }
    else
	fastcscale(imgdata + x1 + imgwidth * y1, c->imagebuffer, x2 - x1, aa_imgwidth(c), y2 - y1, aa_imgheight(c), imgwidth, aa_imgwidth(c));
    if (d) {
	free(image->decompressed);
	image->decompressed = NULL;
    }
    return;
}
int dualmode = 0;
void scale2(struct image *image, int x1, int y1, int x2, int y2)
{
    aa_context *c = context;
    int width = aa_imgwidth(c) / 2;
    int height = aa_imgheight(c);
    int imgwidth = image->width;
    int imgheight = image->height;
    int d = 0;
    static atype xstep, ystep;
    char *imgdata;
    dualmode ^= 1;
    if (image->decompressed == NULL)
	decompressimg(image), d = 1;
    imgdata = image->decompressed;
    /*memset(c->imagebuffer, 0, aa_imgwidth(c) * aa_imgheight(c)); */
    if (x1 < 0 || y1 < 0 || x2 >= imgwidth || y2 >= imgheight) {
	int xx1 = 0, xx2 = aa_imgwidth(c) / 2 - 1, yy1 = 0, yy2 = aa_imgheight(c) - 1;
	xstep = ((atype) width) / (x2 - x1 - 1);
	ystep = ((atype) height) / (y2 - y1 - 1);
	if (x2 >= imgwidth) {
	    xx2 = (imgwidth - 1 - x1) * xstep;
	    x2 = imgwidth - 1;
	}
	if (y2 >= imgheight) {
	    yy2 = (imgheight - 1 - y1) * ystep;
	    y2 = imgheight - 1;
	}
	if (x1 < 0) {
	    xx1 = -xstep * x1;
	    x1 = 0;
	}
	if (y1 < 0) {
	    yy1 = -ystep * y1;
	    y1 = 0;
	}
	fastcscale(imgdata + x1 + imgwidth * y1, c->imagebuffer + width * dualmode + xx1 + yy1 * width, x2 - x1, xx2 - xx1, y2 - y1, yy2 - yy1, imgwidth, aa_imgwidth(c));
	return;
    }
    else
	fastcscale(imgdata + x1 + imgwidth * y1, c->imagebuffer + width * dualmode, x2 - x1, aa_imgwidth(c) / 2, y2 - y1, aa_imgheight(c), imgwidth, aa_imgwidth(c));
    if (d) {
	free(image->decompressed);
	image->decompressed = NULL;
    }
    return;
}

char *decompressimg(struct image *image)
{
    char *data = malloc(image->width * image->height + 5);
    int size = image->width * image->height;
    int r;
    r = lzo1x_decompress(image->data, image->size, data, &size, NULL);
    if (r != LZO_E_OK) {
	printf("compression failed: %d %i %i %i\n", r, image->size, image->width, image->height);
	exit(-1);
    }
    image->decompressed = data;

    return (data);
}
void dispimg(struct image *image, int dm)
{
    int x1, y1, x2, y2;
    int imgwidth = image->width;
    int imgheight = image->height;
    int width = aa_mmwidth(context);
    if (dm)
	width /= 2;
    x2 = imgwidth;
    y2 = imgheight;
    if (imgwidth * aa_mmheight(context) / width > imgheight)
	y2 = imgwidth * aa_mmheight(context) / width;
    else
	x2 = imgheight * width / aa_mmheight(context);
    x1 = (imgwidth - x2) / 2;
    x2 += x1;
    y1 = (imgheight - y2) / 2;
    y2 += y1;
    if (dm)
	scale2(image, x1, y1, x2, y2);
    else
	scale(image, x1, y1, x2, y2);

}
