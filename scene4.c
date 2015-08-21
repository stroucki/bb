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

#include <stdlib.h>
#include <string.h>
#include "bb.h"
#define N 10
#define N1 20

#define XSIZ aa_imgwidth(context)
#define YSIZ (aa_imgheight(context))
#define MAXTABLE (256*5)
static unsigned char *bitmap;
static unsigned int *table;

static void gentable()
{
    unsigned int i, p2;
    int minus = 800 / YSIZ;
    bitmap = context->imagebuffer;
    if (minus == 0)
	minus = 1;
    for (i = 0; i < MAXTABLE; i++) {
	if (i > minus) {
	    p2 = (i - minus) / 5;
	    table[i] = p2;
	}
	else
	    table[i] = 0;
    }
}
#define MA 5

static void firemain()
{
    register unsigned int i;
    unsigned char *p;
    i = 0;
#define END (bitmap + XSIZ * YSIZ)
    for (p = bitmap;
	 p <= (unsigned char *) (END); p += 1) {
	*p = table[
		      (*(p + XSIZ - 1) + *(p + XSIZ + 1) + *(p + XSIZ)) +
		      (*(p + 2 * XSIZ - 1) + *(p + 2 * XSIZ + 1))];
    }
}

#define min(x,y) ((x)<(y)?(x):(y))
static void drawfire(int n)
{
    unsigned int i, last1, i1, i2;
    static int height = 0;
    register unsigned char *p;
    /*message("(EE)",starttime+200000); */
    for (; n; n--) {
	if (height > endtime - TIME - height)
	    height -= 6;
	else
	    height += 6;
	i1 = 1;
	i2 = 4 * XSIZ + 1;
	if (endtime - TIME > 2000000) {
	    for (p = (char *) bitmap + XSIZ * (YSIZ + 0); p < ((unsigned char *) bitmap + XSIZ * (YSIZ + 1)); p++, i1 += 4, i2 -= 4) {
		last1 = rand() % min(i1, min(i2, height));
		i = rand() % 6;
		for (; p < (unsigned char *) bitmap + XSIZ * (YSIZ + 1) && i != 0; p++, i--, i1 += 4, i2 -= 4)
		    *p = last1, last1 += rand() % 6 - 2,
			*(p + XSIZ) = last1, last1 += rand() % 6 - 2;
		*(p + 2 * XSIZ) = last1, last1 += rand() % 6 - 2;
	    }
	}
	else {
	    memset(bitmap + XSIZ * (YSIZ), 0, XSIZ * 4);
	}
	i = 0;
	firemain();
	params->bright -= 12;
	if (params->bright < 0)
	    params->bright = 0;
    }
}

static void mydraw()
{
    aa_render(context, params, 0, 0, aa_scrwidth(context), aa_scrheight(context));
    aa_flush(context);
}

#define MIN(x,y) ((x)<(y)?(x):(y))
static void clrinvaz()
{
    memset(context->textbuffer, ' ', aa_scrwidth(context) * MIN(15, aa_scrheight(context)));
    /*aa_display(context, 0, 0, aa_scrwidth(context), aa_scrheight(context)); */
}

void scene4(void)
{
    int i, x, y, p, q, d, k, n;
    int wtime;
    i = 0;
    table = malloc(MAXTABLE * sizeof(int));
    textclrscr();
    /*aa_display(context, 0, 0, aa_scrwidth(context),aa_scrheight(context)); */
    n = (aa_scrwidth(context) - 5 - N1 + 5) / 6;
    wtime = 1.0 * 1000000 / n;
    for (x = 0; x < aa_scrwidth(context) - 5 - N1; x += 6) {
	for (y = 0; y < 10; y += 3)
	    /*aa_puts(context, x, y, AA_NORMAL, " /oo\\"); */
	    aa_puts(context, x, y, AA_NORMAL, " ----");
	bbflushwait(wtime);
    }
    n = (aa_scrwidth(context) - 7 + 7) / 8;
    wtime = 1.0 * 1000000 / n;
    for (x = 0; x < aa_scrwidth(context) - 7; x += 8) {
	aa_puts(context, x, aa_scrheight(context) - 3, AA_NORMAL, "/~~\\");
	bbflushwait(wtime);
    }
    for (y = 0; y < 10; y += 3) {
	for (x = 0; x < aa_scrwidth(context) - 5 - N1; x += 6)
	    aa_puts(context, x, y, AA_NORMAL, " -oo-");
	bbflushwait(0.1 * 1000000);
    }
    bbflushwait(0.2 * 1000000);
    p = aa_scrwidth(context) / 2;
    d = k = 0;
    for (q = 0; q < 5; q++) {
	clrinvaz();
	for (i = 0; i < N1; i++) {
	    d++;
	    if (d > 8)
		k ^= 1, d = 0;
	    for (x = 0; x < aa_scrwidth(context) - 5 - N1; x += 6)
		for (y = 0; y < 10; y += 3)
		    aa_puts(context, x + i, y + q, AA_NORMAL, k ? " \\oo/ " : " /OO\\ ");
	    aa_puts(context, p, aa_scrheight(context) - 1, AA_NORMAL, " [^] ");
	    p += rand() % 3 - 1;
	    bbflushwait(0.02 * 1000000);
	}
	q++;
	clrinvaz();
	for (i = N1; i; i--) {
	    d++;
	    if (d > 8)
		k ^= 1, d = 0;
	    for (x = 0; x < aa_scrwidth(context) - 5 - N1; x += 6)
		for (y = 0; y < 10; y += 3)
		    aa_puts(context, x + i, y + q, AA_NORMAL, k ? " \\oo/ " : " /OO\\ ");
	    aa_puts(context, p, aa_scrheight(context) - 1, AA_NORMAL, " [^] ");
	    p += rand() % 3 - 1;
	    bbflushwait(0.02 * 1000000);
	}
    }
    clrinvaz();
    for (i = 0; i < N; i++) {
	d++;
	if (d > 8)
	    k ^= 1, d = 0;
	for (x = 0; x < aa_scrwidth(context) - 5 - N1; x += 6)
	    for (y = 0; y < 10; y += 3)
		aa_puts(context, x + i, y + q, AA_NORMAL, k ? " \\oo/ " : " /OO\\ ");
	aa_puts(context, p, aa_scrheight(context) - 1, AA_NORMAL, " [^] ");
	p += rand() % 3 - 1;
	bbflushwait(0.01 * 1000000);
    }
    for (i = 0; i < 5; i++) {
	for (x = 0; x < aa_scrwidth(context) - 5 - N1; x += 6)
	    for (y = 0; y < 10; y += 3)
		aa_puts(context, x + N, y + q, AA_NORMAL, " /OO\\");
	bbflushwait(0.1 * 1000000);
	for (x = 0; x < aa_scrwidth(context) - 5 - N1; x += 6)
	    for (y = 0; y < 10; y += 3)
		aa_puts(context, x + N, y + q, AA_NORMAL, " /**\\");
	bbflushwait(0.1 * 1000000);
    }
    n = aa_scrheight(context) - 10 - q;
    if (aa_scrheight(context) / 2 < 10)
	n = aa_scrheight(context) / 2 - q;
    if (n < 1) {
	bbflushwait(0.7 * 1000000);
    }
    else {
	wtime = 0.7 * 1000000 / n;
	for (i = q; i < aa_scrheight(context) / 2; i++) {
	    for (x = 0; x < aa_scrwidth(context) - 5 - N1; x += 6)
		for (y = 0; y < 10; y += 3)
		    aa_puts(context, x + N, y + i, AA_NORMAL, "     "),
			aa_puts(context, x + N, y + i + 1, AA_NORMAL, " \\@@/");
	    aa_puts(context, p - 2, aa_scrheight(context) - 1, AA_NORMAL, "   [^]   ");
	    p += rand() % 5 - 2;
	    bbflushwait(wtime);
	}
	for (; i < aa_scrheight(context) - 10; i++) {
	    for (x = 0; x < aa_scrwidth(context) - 5 - N1; x += 6)
		for (y = 0; y < 10; y += 3)
		    aa_puts(context, x + N, y + i, AA_NORMAL, "     "),
			aa_puts(context, x + N, y + i + 1, AA_NORMAL, " \\--/");
	    aa_puts(context, p - 2, aa_scrheight(context) - 1, AA_NORMAL, "   (~)   ");
	    p += rand() % 7 - 3;
	    bbflushwait(wtime);
	}
    }
    free(context->imagebuffer);
    context->imagebuffer = malloc(aa_imgwidth(context) * (aa_imgheight(context) + 4));
    /*clrscr(); */
    backconvert(0, 0, aa_scrwidth(context), aa_scrheight(context));
    params->bright = 120;
    drawptr = 0;
    draw();
    bbflushwait(0.1 * 1000000);
    params->bright = 255;
    draw();
    bbflushwait(0.1 * 1000000);
    for (i = 20; i < aa_imgwidth(context) - 20; i++)
	aa_putpixel(context, i, aa_imgheight(context) - 10, 255),
	    aa_putpixel(context, i, aa_imgheight(context) - 11, 255),
	    aa_putpixel(context, i, aa_imgheight(context) - 12, 255),
	    aa_putpixel(context, i, aa_imgheight(context) - 13, 255),
	    gentable();
    timestuff(-25, drawfire, mydraw, 7 * 1000000);
    free(table);
}
