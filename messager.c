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
#include "bb.h"

static int cursor_x, cursor_y;
static int start;

static void newline()
{
    while (cursor_x < aa_scrwidth(context)) {
	context->textbuffer[cursor_x + cursor_y * aa_scrwidth(context)] = ' ';
	context->attrbuffer[cursor_x + cursor_y * aa_scrwidth(context)] = AA_NORMAL;
	cursor_x++;
    }
    start--;
    if (start < 0)
	start = 0;
    cursor_y++, cursor_x = 0;
    if (cursor_y >= aa_scrheight(context)) {
	memcpy(context->textbuffer + start * aa_scrwidth(context), context->textbuffer + (start + 1) * aa_scrwidth(context), aa_scrwidth(context) * (aa_scrheight(context) - start - 1));
	memcpy(context->attrbuffer + start * aa_scrwidth(context), context->attrbuffer + (start + 1) * aa_scrwidth(context), aa_scrwidth(context) * (aa_scrheight(context) - start - 1));
	memset(context->textbuffer + aa_scrwidth(context) * (aa_scrheight(context) - 1), ' ', aa_scrwidth(context));
	memset(context->attrbuffer + aa_scrwidth(context) * (aa_scrheight(context) - 1), 0, aa_scrwidth(context));
	cursor_y--;
    }
}

static void put(char c)
{
    if (c == '\n') {
	newline();
	return;
    }
    context->textbuffer[cursor_x + cursor_y * aa_scrwidth(context)] = c;
    context->attrbuffer[cursor_x + cursor_y * aa_scrwidth(context)] = AA_NORMAL;
    cursor_x++;
    if (cursor_x == aa_scrwidth(context))
	newline();
}

static void putcursor(void)
{
    context->attrbuffer[cursor_x + cursor_y * aa_scrwidth(context)] = AA_REVERSE;
    context->textbuffer[cursor_x + cursor_y * aa_scrwidth(context)] = ' ';
    aa_gotoxy(context, cursor_x, cursor_y);
}

void messager(char *c)
{
    int i, s = strlen(c);
    start = cursor_y = aa_scrheight(context) - 1;
    cursor_x = 0;
    for (i = 0; i < s; i++) {
	put(c[i]);
	putcursor();
	bbflushwait(0.03 * 1000000);
    }
    bbflushwait(1000000);
    aa_gotoxy(context, 0, 0);
}

void tographics()
{
    backconvert(0, start, aa_scrwidth(context), aa_scrheight(context));
}
static char *bckup;
static char *bckup1;

#define STAGE (TIME-starttime)

static void toblack()
{
    params->bright = -STAGE * 256 / (endtime - starttime);
}

static void towhite()
{
    params->bright = STAGE * 256 / (endtime - starttime);
}

static void decontr()
{
    params->contrast = -STAGE * 256 / (endtime - starttime);
}

static void incrandom()
{
    params->randomval = STAGE * 100 / (endtime - starttime);
}

static void toblack1()
{
    int x, y, mul1, mul2 = 0, pos;
    int minpos = 0;
    unsigned char *b1 = bckup, *b2 = bckup1;
    pos = STAGE * (aa_imgheight(context) + aa_imgheight(context)) / (endtime - starttime) - aa_imgheight(context);
    for (y = 0; y < aa_imgheight(context); y++) {
	mul1 = (y - pos);
	if (mul1 < 0)
	    mul1 = 0;
	else
	    mul1 = mul1 * 256 * 4 / aa_imgheight(context);
	if (mul1 > 256)
	    mul1 = 256;
	mul2 = (y - pos - aa_imgheight(context));
	if (mul2 < 0)
	    mul2 = 0;
	else
	    mul2 = mul2 * 256 * 8 / aa_imgheight(context);
	if (mul2 > 256)
	    mul2 = 256;
	if (mul2 == 0)
	    minpos = y;
	mul1 -= mul2;
	for (x = 0; x < aa_imgwidth(context); x++) {
	    aa_putpixel(context, x, y,
			(mul1 * (int) (*b1) + mul2 * (int) (*b2)) / 256);
	    b1++;
	    b2++;
	}
    }
    minpos = pos + 3 * aa_imgheight(context) / 4;
    if (minpos < 0)
	minpos = 0;
    if (minpos > aa_imgheight(context))
	minpos = aa_imgheight(context);
    aa_render(context, params, 0, 0, aa_imgwidth(context), minpos);
    aa_flush(context);
}

void devezen1()
{
    bckup = (char *) malloc(aa_imgwidth(context) * aa_imgheight(context));
    memcpy(bckup, context->imagebuffer, aa_imgwidth(context) * aa_imgheight(context));
    tographics();
    bckup1 = (char *) malloc(aa_imgwidth(context) * aa_imgheight(context));
    memcpy(bckup1, context->imagebuffer, aa_imgwidth(context) * aa_imgheight(context));
    drawptr = toblack1;
    timestuff(0, NULL, toblack1, 5000000);
    free(bckup);
    free(bckup1);
}

void devezen2()
{
    tographics();
    drawptr = toblack;
    timestuff(0, NULL, draw, 1000000);
}

void devezen3()
{
    tographics();
    drawptr = incrandom;
    timestuff(0, NULL, draw, 1000000);
    params->randomval = 100;
    drawptr = toblack;
    timestuff(0, NULL, draw, 1000000);
    params->randomval = 0;
    params->bright = 0;
}

void devezen4()
{
    tographics();
    drawptr = decontr;
    timestuff(0, NULL, draw, 500000);
    drawptr = towhite;
    timestuff(0, NULL, draw, 500000);
    params->bright = 0;
    params->contrast = 0;
}
