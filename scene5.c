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

#include <string.h>
#include <malloc.h>
#include <math.h>
#include "bb.h"
#include "tex.h"

static unsigned char *helpbuffer;
static unsigned char *text, *text1;
static int aaval = -1, colorval;
static int sstarttime;

#define DEANTIALIAS(a) (((a)!=0)*255*mul+acolor*(255-mul))>>8;

static void do3d()
{
    int t = pow((TIME - sstarttime) / 30000 / 200.0, 1.3) * 200;
    if (t < 0)
	t = 0;
    alfa = beta = gama = t;
    beta += 90;
    alfa %= 360;
    beta %= 360;
    gama %= 360;
    disp3d();
}

static void mydraw(void)
{
    int x, y;
    unsigned char *pos = context->imagebuffer;
    unsigned char *pos1 = helpbuffer;
    if (aaval >= 0) {
	for (y = 0; y < aa_imgheight(context); y += 2) {
	    int mul;
	    mul = (-y) * 8 + aaval;
	    if (mul < 0)
		mul = 0;
	    if (mul > 255)
		mul = 255;
	    for (x = 0; x < aa_imgwidth(context); x += 2) {
		int acolor;
		if (*pos1 || *(pos1 + 1) || *(pos1 + aa_imgwidth(context)) || *(pos1 + 1 + aa_imgwidth(context)))
		    acolor = 255;
		else
		    acolor = 0;

		*pos = DEANTIALIAS(*pos1);
		*(pos + 1) = DEANTIALIAS(*(pos1 + 1));
		*(pos + aa_imgwidth(context)) = DEANTIALIAS(*(pos1 + aa_imgwidth(context)));
		*(pos + 1 + aa_imgwidth(context)) = DEANTIALIAS(*(pos1 + 1 + aa_imgwidth(context)));
		pos += 2;
		pos1 += 2;

	    }
	    pos += aa_imgwidth(context);
	    pos1 += aa_imgwidth(context);

	}
    }
    else
	clrscr();
    drawprujezd(text, starttime);
}

static void mydraw1(void)
{
    int x, y;
    unsigned char *pos = context->imagebuffer;
    unsigned char *pos1 = helpbuffer;
    int mul2 = 255 * (255 - colorval);
    if (aaval >= 0) {
	for (y = 0; y < aa_imgheight(context); y += 1) {
	    for (x = 0; x < aa_imgwidth(context); x += 1) {
		*pos = (*pos1 * colorval + ((*pos1) != 0) * mul2) >> 8;
		pos++;
		pos1++;

	    }
	}
    }
    else
	clrscr();
    drawprujezd(text, starttime);
}

static void mydraw2(void)
{
    do3d();
    drawprujezd(text, starttime);
}

static void mydraw3(void)
{
    clrscr();
    drawlepic(text);
}

static void mydraw4(void)
{
    do3d();
    drawlepic(text);
}

static void mydraw5(void)
{
    do3d();
    drawlevotoc(text, text1, starttime);
}

static void mydraw6(void)
{
    do3d();
    drawhorotoc(text, text1, starttime);
}

static void mydraw8(void)
{
    do3d();
    /*params->bright=-(TIME-starttime)*256/(endtime-starttime); */
    centerx = (TIME - starttime) * 400 / (endtime - starttime);
}

static void mydraw7(void)
{
    do3d();
    drawpravotoc(text, text1, starttime);
}

static void decaaval(int i)
{
    aaval += 7 * i;
}

static void inccolor(int i)
{
    colorval = (TIME - starttime) * 256 / (endtime - starttime);
}

static void incgama(int i)
{
    params->gamma = 1.0 + (TIME - starttime) * 4.0 / (endtime - starttime);
}

static void decgama(int i)
{
    params->gamma = 1.0 + (endtime - TIME) * 4.0 / (endtime - starttime);
}

static void incrandom(int i)
{
    if (TIME < (endtime + starttime) / 2) {
	params->randomval = (TIME - starttime) * 256 / (endtime - starttime);
    }
    else
	params->randomval = (endtime - TIME) * 256 / (endtime - starttime);
}

static void pauzicka()
{
    drawptr = do3d;
    timestuff(0, NULL, draw, 2 * 1000000);
}

static void stmivac()
{
    drawptr = mydraw8;
    timestuff(0, NULL, draw, 2 * 1000000);
}

void scene5(void)
{
    aaval = -1;
    set_zbuff();
    helpbuffer = malloc(aa_imgwidth(context) * aa_imgheight(context));
    alfa = 0;
    beta = 90;
    gama = 0;
    centerx = 0;
    centery = 0;
    centerz = 0;
    zoom = 1.5;
    torusconstructor();
    disp3d();
    memcpy(helpbuffer, context->imagebuffer, aa_imgwidth(context) * aa_imgheight(context));
    params->dither = AA_NONE;

    text = "Supports";
    drawptr = mydraw3;
    initlepic();
    timestuff(-60, ctrllepic, draw, 2 * 1000000);

    drawptr = mydraw;
    aaval = 0;
    text = "ANTIALIASING";
    timestuff(60, NULL, draw, 5 * 1000000);

    text = "";
    timestuff(-60, decaaval, draw, 3 * 1000000);

    text = "256 colors-ascii";
    colorval = 0;
    drawptr = mydraw1;
    timestuff(-60, inccolor, draw, 6 * 1000000);

    sstarttime = TIME;
    pauzicka();
    initlepic();
    text = "dithering";
    drawptr = mydraw4;
    timestuff(-60, ctrllepic, draw, 2 * 1000000);

    pauzicka();
    text = "random";
    drawptr = mydraw2;
    timestuff(-60, incrandom, draw, 3 * 1000000);

    params->randomval = 0;
    pauzicka();
    text1 = " ";
    text = "Error";
    drawptr = mydraw6;
    timestuff(60, NULL, draw, 1 * 1000000);
    params->dither = AA_ERRORDISTRIB;

    text1 = "Error";
    text = "distribution";
    drawptr = mydraw6;
    timestuff(60, NULL, draw, 1 * 1000000);

    text1 = "distribution";
    text = " ";
    drawptr = mydraw6;
    timestuff(60, NULL, draw, 1 * 1000000);
    params->randomval = 0;

    pauzicka();
    text1 = " ";
    text = "Floyd-";
    drawptr = mydraw5;
    timestuff(60, NULL, draw, 1 * 1000000);
    params->dither = AA_FLOYD_S;

    text1 = "Floyd-";
    text = "Steinberg";
    drawptr = mydraw5;
    timestuff(60, NULL, draw, 1 * 1000000);

    text1 = "Steinberg";
    text = " ";
    drawptr = mydraw5;
    timestuff(60, NULL, draw, 1 * 1000000);

    pauzicka();
    text1 = " ";
    text = "Gamma ";
    drawptr = mydraw7;
    timestuff(-60, incgama, draw, 1 * 1000000);
    params->dither = AA_FLOYD_S;

    text1 = "Gamma ";
    text = "Correction";
    drawptr = mydraw7;
    timestuff(60, NULL, draw, 1 * 1000000);

    text1 = "Correction";
    text = " ";
    drawptr = mydraw7;
    timestuff(-60, decgama, draw, 1.5 * 1000000);
    params->gamma = 1.0;
    pauzicka();
    stmivac();
    centerx = 0;
}
