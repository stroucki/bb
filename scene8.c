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
#include <malloc.h>
#include "bb.h"
#define STATE1 (TIME-starttime1)
#define STATE (time-starttime)
#define IN(v2,v1) (((double)(v1)*mul1+(v2)*mul2)/div)
extern struct image zeb;
static int xs, xe, ys, ye, starttime1;
static int sxs, sys, sye, sxe, cs, ce, bs, be, rs, re;
static void mydraw(void)
{
    int cx, cy;
    int sx, sy;
    int time = TIME;
    int mul1, mul2, div = (endtime - starttime) >> 8;
    if (time < starttime)
	time = starttime;
    if (time > endtime)
	time = endtime;
    mul1 = STATE >> 8, mul2 = (endtime - time) >> 8, div = (endtime - starttime) >> 8;
    if (mul2 < 0)
	mul1 -= mul2, mul2 = 0;
    cx = IN(xs, xe) + sin(STATE1 / 300000.0) * 40;
    cy = IN(ys, ye) + cos(STATE1 / 500000.0) * 40;
    sx = IN(sxs, sxe) + sin(STATE1 / 520000.0) * 70;
    sy = IN(sys, sye) + cos(STATE1 / 700000.0) * 70;
    params->contrast = IN(cs, ce);
    params->bright = IN(bs, be);
    params->randomval = IN(rs, re);
    scale(&zeb, (cx - sx) * zeb.width / 1000,
	  (cy - sy) * zeb.height / 1000,
	  (cx + sx) * zeb.width / 1000,
	  (cy + sy) * zeb.height / 1000);
}
void scene8()
{
    decompressimg(&zeb);
    drawptr = mydraw;
    starttime1 = starttime;
    bs = -255;
    be = 0;
    rs = 100;
    re = 100;
    cs = 100, ce = 100;
    xs = xe = 300;
    ys = ye = 400;
    sxs = sys = sxe = sye = 100;
    timestuff(0, NULL, draw, 2 * 1000000);
    bs = be;
    re = 0;
    timestuff(0, NULL, draw, 1 * 1000000);
    rs = re;
    timestuff(0, NULL, draw, 5 * 1000000);
    sxe = 200;
    sye = 200;
    xs = xe;
    ys = ye;
    xe = 750;
    ye = 300;
    timestuff(0, NULL, draw, 3 * 1000000);
    xs = xe;
    ys = ye;
    sxs = sxe, sys = sye;
    ce = 20;
    sxe = 300;
    sye = 300;
    timestuff(0, NULL, draw, 1 * 1000000);
    cs = ce;
    xs = xe;
    ys = ye;
    sxs = sxe, sys = sye;
    ye = 400;
    sxe = 600;
    sye = 600;
    timestuff(0, NULL, draw, 2 * 1000000);
    xs = xe;
    ys = ye;
    sxs = sxe, sys = sye;
    sxe = 200;
    sye = 200;
    timestuff(0, NULL, draw, 1 * 1000000);
    xs = xe;
    ys = ye;
    sxs = sxe, sys = sye;
    be = 255;
    sxe = 120;
    sye = 120;
    timestuff(0, NULL, draw, 0.2 * 1000000);
    free(zeb.decompressed);
    params->bright = 0;
    params->contrast = 0;
}
