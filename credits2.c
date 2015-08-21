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
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "bb.h"
#define STATE (TIME-starttime)
#undef MAXTIME
#define MAXTIME 2500000
static unsigned char *source, *target;

static void decrand()
{
    if (STATE < 4000000) {
	params->randomval = (4000000 - STATE) * 1600 / 4000000;
    }
    else
	params->randomval = 0;
}
static void pryc()
{
    float size = 3 + (STATE) * 10.0 / MAXTIME;
    float radius = (STATE) * aa_imgwidth(context) / MAXTIME;
    float yradius = (STATE) * aa_imgheight(context) / MAXTIME;
    int xm = aa_imgwidth(context) / 2;
    int ym = aa_imgheight(context) / 3;
    clrscr();
    centerprint(xm + radius * sin(STATE / 100000.0), ym + yradius * cos(STATE / 300000.0), size, 128, "The", 0);
    centerprint(xm + radius * sin(STATE / 150000.0), 2 * ym + yradius * cos(STATE / 400000.0), size, 128, "END", 0);
}

#define LOGOY ((aa_scrheight(context)-LOGOHEIGHT)/2)
#define LOGOHEIGHT 6
#define YSTART 9
#define MTIME 1000000

static void getsource()
{
    backconvert(0, YSTART, aa_scrwidth(context), aa_scrheight(context));
    memcpy(source, context->imagebuffer + aa_imgwidth(context) * 2 * YSTART, aa_imgwidth(context) * (aa_imgheight(context) - YSTART * 2));
}

static void gettarget()
{
    backconvert(0, YSTART, aa_scrwidth(context), aa_scrheight(context));
    memcpy(target, context->imagebuffer + aa_imgwidth(context) * 2 * YSTART, aa_imgwidth(context) * (aa_imgheight(context) - YSTART * 2));
}

static void morphdraw()
{
    int s = STATE > 0 ? (STATE < MTIME ? STATE : MTIME) : 0;
    int mul = s * 256 / MTIME;
    int mul1 = 256 - mul;
    int i;
    int size = aa_imgwidth(context) * (aa_imgheight(context) - YSTART * 2);
    char *c = context->imagebuffer + aa_imgwidth(context) * 2 * YSTART;
    for (i = 0; i < size; i++, c++) {
	*c = (source[i] * mul1 + target[i] * mul) >> 8;
    }
    aa_fastrender(context, 0, YSTART, aa_scrwidth(context), aa_scrheight(context));
    aa_flush(context);
}

static void morph()
{
    params->dither = AA_NONE;
    timestuff(0, NULL, morphdraw, MTIME);
}

static void displaya(int p)
{
    int xpos = aa_scrwidth(context) / 2;
    int i;
    for (i = 0; i < LOGOHEIGHT; i++) {
	if (xpos - p - 5 - i >= 0)
	    aa_puts(context, xpos - p - 5 - i, i + LOGOY, AA_NORMAL, " ");
	if (xpos - p - 4 - i >= 0)
	    aa_puts(context, xpos - p - 4 - i, i + LOGOY, AA_NORMAL, "d");
	if (xpos - p - 3 - i >= 0)
	    aa_puts(context, xpos - p - 3 - i, i + LOGOY, AA_NORMAL, "T");
	if (xpos + p + 4 + i < aa_scrwidth(context))
	    aa_puts(context, xpos + p + 4 + i, i + LOGOY, AA_NORMAL, " ");
	if (xpos + p + 3 + i < aa_scrwidth(context))
	    aa_puts(context, xpos + p + 3 + i, i + LOGOY, AA_NORMAL, "b");
	if (xpos + p + 2 + i < aa_scrwidth(context))
	    aa_puts(context, xpos + p + 2 + i, i + LOGOY, AA_NORMAL, "T");
    }
}

static void display8()
{
    int i;
    for (i = 0; i < LOGOHEIGHT; i++) {
	aa_puts(context, aa_scrwidth(context) / 2 - 2, LOGOY + i, AA_NORMAL, "8  8");
    }
}

static void displogo(int y)
{
    int x = aa_scrwidth(context) / 2 - 9;
    aa_puts(context, x, y + 0, AA_NORMAL, "     dT8  8Tb");
    aa_puts(context, x, y + 1, AA_NORMAL, "    dT 8  8 Tb");
    aa_puts(context, x, y + 2, AA_NORMAL, "   dT  8  8  Tb");
    aa_puts(context, x, y + 3, AA_NORMAL, "<PROJECT><PROJECT>");
    aa_puts(context, x, y + 4, AA_NORMAL, " dT    8  8    Tb");
    aa_puts(context, x, y + 5, AA_NORMAL, "dT     8  8     Tb");
}

static void displaytext(int p)
{
    int i;
    int x, x1, a = AA_NORMAL;
    char s[2] =
    {0, 0};
    textclrscr();
    displogo(1);
    for (i = 0; i < aa_scrwidth(context); i++) {
	context->textbuffer[aa_scrwidth(context) * (YSTART - 2) + i] = '_';
    }
    for (i = p; i < p + aa_scrheight(context) - YSTART; i++) {
	if (i < textsize) {
	    x = 0;
	    x1 = 0;
	    for (x1 = x = 0; x < strlen(line[i]); x++, x1++) {
		while (((unsigned char) line[i][x]) < 20)
		    a = line[i][x] - 1, x++;
		if (x < strlen(line[i])) {
		    s[0] = line[i][x];
		    aa_puts(context, x1, i - p + YSTART, a, s);
		}
	    }
	}
	else {
	    x = 0;
	    aa_puts(context, x, i - p + YSTART, AA_NORMAL, "~");
	}
    }
    if (dual) {
	int sh = aa_scrheight(context) - YSTART;
	for (i = p; i < p + (aa_scrheight(context) - YSTART); i++) {
	    if (i + sh < textsize) {
		x = 0;
		x1 = 0;
		for (x1 = x = 0; x < strlen(line[i + sh]); x++, x1++) {
		    while (((unsigned char) line[i + sh][x]) < 20)
			a = line[i + sh][x] - 1, x++;
		    if (x < strlen(line[i + sh])) {
			s[0] = line[i + sh][x];
			aa_puts(context, x1 + aa_scrwidth(context) / 2, i - p + YSTART, a, s);
		    }
		}
	    }
	    else {
		x = 0;
		aa_puts(context, x + aa_scrwidth(context) / 2, i - p + YSTART, AA_NORMAL, "~");
	    }
	}
    }


}

static void decbright(void)
{
    params->bright = -(TIME - starttime) * 256 / (endtime - starttime);
}

void credits2(void)
{
    int i, ch;
    int p;
    int plast = -1;
    clrscr();

    centerprint(aa_imgwidth(context) / 2, aa_imgheight(context) / 3, 3, 128, "The", 0);
    centerprint(aa_imgwidth(context) / 2, 2 * aa_imgheight(context) / 3, 3, 128, "END", 0);
    drawptr = decrand;
    params->randomval = 50;
    timestuff(0, NULL, draw, 5000000);
    drawptr = NULL;
    params->randomval = 0;
    drawptr = pryc;
    timestuff(0, NULL, draw, MAXTIME);
    drawptr = NULL;
    clrscr();
    draw();
    load_song("bb3.s3m");
    bbupdate();
    starttime = endtime = TIME;
    play();
    bbwait(1);
    for (i = 0; i < LOGOHEIGHT; i++) {
	aa_puts(context, aa_scrwidth(context) / 2 - 2, LOGOY + i, AA_BOLD, "8  8");
	if (i)
	    aa_puts(context, aa_scrwidth(context) / 2 - 2, LOGOY + i - 1, AA_NORMAL, "8  8");
	bbflushwait(100000);
    }
    aa_puts(context, aa_scrwidth(context) / 2 - 2, LOGOY + i - 1, AA_NORMAL, "8  8");

#define LWIDTH

    for (i = aa_scrwidth(context) / 2; i; i--) {
	display8();
	displaya(i);
	bbflushwait(10000);
    }
    for (; i < 100; i++) {
	textclrscr();
	displaya(10 * sin(i * M_PI / 100));
	display8();
	bbflushwait(10000);
    }
    aa_puts(context, aa_scrwidth(context) / 2 - 9, LOGOY + 3, AA_DIM, "<PROJECT><PROJECT>");
    bbflushwait(100000);
    aa_puts(context, aa_scrwidth(context) / 2 - 9, LOGOY + 3, AA_NORMAL, "<PROJECT><PROJECT>");
    bbflushwait(100000);
    aa_puts(context, aa_scrwidth(context) / 2 - 9, LOGOY + 3, AA_BOLD, "<PROJECT><PROJECT>");
    bbflushwait(100000);
    aa_puts(context, aa_scrwidth(context) / 2 - 9, LOGOY + 3, AA_NORMAL, "<PROJECT><PROJECT>");
    bbflushwait(100000);
    bbwait(1000000);
    for (i = LOGOY; i > 1; i--) {
	textclrscr();
	displogo(i);
	bbflushwait(30000);
    }

    source = malloc(aa_imgwidth(context) * (aa_imgheight(context)));
    target = malloc(aa_imgwidth(context) * (aa_imgheight(context)));
    params->dither = AA_NONE;
    format(dual ? aa_scrwidth(context) / 2 : aa_scrwidth(context));
    p = 0;
    while (1) {
	if (p != plast) {
	    getsource();
	    displaytext(p);
	    gettarget();
	    morph();
	    displaytext(p);
	    aa_flush(context);
	    plast = p;
	}
      again:
#ifndef __DJGPP__
	ch = aa_getkey(context, 1);
#else
	while ((ch = bbupdate()) == AA_NONE) ;
#endif

	switch (ch) {
	case '1':
	    load_song("bb.s3m");
	    bbupdate();
	    play();
	    break;
	case '2':
	    load_song("bb2.s3m");
	    bbupdate();
	    play();
	    break;
	case '3':
	    load_song("bb3.s3m");
	    bbupdate();
	    play();
	    break;

	case 'b':
	case 'k':
	case 'B':
	case 'K':
	case AA_BACKSPACE:
	case AA_UP:
	    p -= (aa_scrheight(context) - YSTART) / 2 * (dual + 1);
	    if (p < 0)
		p = 0;
	    break;
	case AA_DOWN:
	case AA_LEFT:
	case 'f':
	case 'F':
	case ' ':
	case 'j':
	case 'J':
	    p += (aa_scrheight(context) - YSTART) / 2 * (dual + 1);
	    if (p > textsize)
		p = textsize;
	    break;
	case 'q':
	case 'Q':
	case AA_ESC:
	    finish_stuff = 0;
	    backconvert(0, 0, aa_scrwidth(context), aa_scrheight(context));
	    bbupdate();
	    starttime = endtime = TIME;
	    drawptr = decbright;
	    timestuff(0, NULL, draw, 1000000);
	    textclrscr();
	    drawptr = NULL;
	    aa_flush(context);
	    free(source);
	    free(target);
	    return;
	default:
	    goto again;
	}
	bbupdate();
	starttime = endtime = TIME;
    }
}
