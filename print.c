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

#include <aalib.h>
#include "config.h"
#include "bb.h"

void fastscale(char *b1, char *b2, int x1, int x2, int y1, int y2, int width1, int width2, int color)
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
	    if (*b1)
		*b2 = color;
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

static INLINE void pscale(int x1, int y1, int x2, int y2, char *data, int w, int h, int color)
{
    float step;
    int xx1, xx2, yy1, yy2;
    if (x1 >= 0 && x2 < aa_imgwidth(context) && y1 >= 0 && y2 <= aa_imgheight(context))
	fastscale(data, context->imagebuffer + x1 + aa_imgwidth(context) * y1, w, x2 - x1, h, y2 - y1, w, aa_imgwidth(context), color);
    if (x2 <= 0 || x1 >= aa_imgwidth(context) || y2 <= 0 || y1 >= aa_imgheight(context))
	return;
    step = w / (float) (x2 - x1);
    if (x1 < 0) {
	xx1 = -step * x1;
	x1 = 0;
    }
    else
	xx1 = 0;
    if (x2 > aa_imgwidth(context)) {
	xx2 = step * (aa_imgwidth(context) - x1);
	x2 = aa_imgwidth(context) - 1;
    }
    else
	xx2 = w;
    step = h / (float) (y2 - y1);
    if (y1 < 0) {
	yy1 = -step * y1;
	y1 = 0;
    }
    else
	yy1 = 0;
    if (y2 > aa_imgheight(context)) {
	yy2 = step * (aa_imgheight(context) - y1);
	y2 = aa_imgheight(context) - 1;
    }
    else
	yy2 = h;
    fastscale(data + xx1 + yy1 * w, context->imagebuffer + x1 + aa_imgwidth(context) * y1, xx2 - xx1, x2 - x1, yy2 - yy1, y2 - y1, w, aa_imgwidth(context), color);

}
void print(int x, int y, float width, int height, struct font *f, int color, char *text)
{
    int i;
    for (i = 0; text[i]; i++) {
	pscale(x + i * width, y, x + (i + 1) * width, y + height, f->data + f->width * (f->height * text[i]), f->width, f->height, color);
    }
}
