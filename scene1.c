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
#include <stdlib.h>
#include <aalib.h>
#include "bb.h"
#include "image.h"

#ifdef ETIME
#undef ETIME
#endif
#define EFECT1 1
#define EFECT 2
#define EFECT2 6.0
#define ETIME EFECT*1000000
#define ETIME1 EFECT*1000000
#define MAXSHIFT (EFECT*400)
#define MAXRAND (EFECT*20)
#define N_STEP 30
#define MAXEFECT2 (N_STEP*EFECT2*20-N_STEP)

char *text;
struct font *font;
static int cursorx, cursory;
void (*drawptr) ();

#define HEXA (rand()&2?'A'+rand()%6:'0'+rand()%10)

static void drawwait()
{
    aa_puts(context, (aa_scrwidth(context) - strlen(text)) / 2, aa_scrheight(context) / 2, AA_SPECIAL, text);
    aa_flush(context);
}

static int bright = 255;

static void drawwait2()
{
    int i = bright;
    /*genwave(); */
    if (i < 0)
	i = 0;
    centerprint(aa_imgwidth(context) / 2, aa_imgheight(context) / 3, 2, i, "AA", 0);
    i = bright + 255;
    if (i < 0)
	i = 0;
    if (i > 255)
	i = 255;
    centerprinth(aa_imgwidth(context) / 2, 2 * aa_imgheight(context) / 3, /*4 */ 8, i, "PRESENTS", 3);
}

static double pos, delta, dist;

static void drawwait4()
{
    clrscr();
    centerprint(aa_imgwidth(context) / 4 - dist * aa_imgwidth(context), pos, 1.1 + delta / 2, 255, "B", 0);
    centerprint(3 * aa_imgwidth(context) / 4 + dist * aa_imgwidth(context), aa_imgheight(context) - pos, 1.1 + delta / 2, 255, "B", 0);
}
static void hlaska(char *text, float n)
{
    clrscr();
    centerprint(aa_imgwidth(context) / 2, aa_imgheight(context) / 2, n, 255, text, 3);
}
void draw(void)
{
    if (drawptr != NULL)
	drawptr();
    aa_render(context, params, 0, 0, aa_imgwidth(context), aa_imgheight(context));
    aa_puts(context, (aa_scrwidth(context) - strlen(text)) / 2, aa_scrheight(context) / 2, AA_SPECIAL, text);
    aa_flush(context);
}

static void calculateslow(int n)
{
    char a[2];
    cursorx++;
    if (cursorx >= aa_scrwidth(context)) {
	cursory++;
	cursorx = 1;
	if (cursory >= aa_scrheight(context))
	    cursory = 0;
    }
    aa_gotoxy(context, cursorx, cursory);
    a[0] = cursorx % 3 ? HEXA : ' ';
    a[1] = 0;

    aa_puts(context, cursorx - 1, cursory, AA_NORMAL, a);
}

static int randshift, randcharacters, randattrs;

static void drawline(int y)
{
    int x;
    int shift = rand() % MAXSHIFT < randshift ? rand() : -1;
    y *= aa_scrwidth(context);
    for (x = 0; x < aa_scrwidth(context); x++) {
	char c;
	if (randattrs && (rand() % (MAXRAND) < randattrs)) {
	    continue;
	}
	else
	    context->attrbuffer[x + y] = AA_NORMAL;
	if (randcharacters && rand() % MAXRAND < randcharacters)
	    continue;
	else
	    c = (x - shift) % 3 ? HEXA : ' ';
	context->textbuffer[x + y] = c;
    }
}

static void drawwait3()
{
    int y;
    aa_render(context, params, 0, 0, aa_imgwidth(context), aa_imgheight(context));
    for (y = 0; y < aa_scrheight(context); y++) {
	drawline(y);
    }
    aa_puts(context, (aa_scrwidth(context) - strlen(text)) / 2, aa_scrheight(context) / 2, AA_SPECIAL, text);
    aa_flush(context);
}

static void calculatefast(int n)
{
    int i;
    if (randshift)
	randshift += n;
    for (i = 0; i < n; i++) {
	drawline(cursory);
	cursory++;
	cursorx = 0;
	if (cursory >= aa_scrheight(context))
	    cursory = 0;
    }
    aa_gotoxy(context, cursorx, cursory);
}

static void calculatefastest(int n)
{
    if (randcharacters)
	randcharacters += n;
    if (randattrs)
	randattrs += n;

}

static void decrandom(int n)
{
    if (params->randomval > 0)
	params->randomval -= n * N_STEP;
    if (params->randomval < 60)
	params->randomval = 60;
}

static void decbright(int n)
{
    bright -= n * 16;
}

double f = -10.0;
static void makepos(int n)
{
    int i;
    for (i = 0; i < n; i++) {
	f += (aa_imgheight(context) / 2 - pos) / 60;
	f *= 0.95;
	pos += f;
    }
}

static void makepos1(int n)
{
    int i;
    for (i = 0; i < n; i++) {
	f += (aa_imgheight(context) / 2 - pos) / 60;
	f *= 0.95;
	pos += f;
	delta += 0.2;
    }
}

static void makepos2(int n)
{
    int i;
    for (i = 0; i < n; i++) {
	f += (aa_imgheight(context) / 2 - pos) / 60;
	f *= 0.95;
	pos += f;
	delta -= 0.2;
	if (delta <= 2)
	    delta = 0.1;
	dist += 0.08;
    }
}

static void strobikuj(int n)
{
    params->bright += n * 50;
}

void strobikstart(void)
{
    int ditherbckup = params->dither;
    if (context->driver->print != NULL)
	params->dither = AA_NONE;
    params->bright = 0;
    timestuff(-60, strobikuj, draw, 1000000 / 15);
    params->bright = 255;
    params->dither = ditherbckup;
    draw();
}

void destrobikuj(int n)
{
    params->bright >>= n;
}

void strobikend(void)
{
    /*timestuff(60, destrobikuj, draw, 1000000 / 5); */
    timestuff(-60, destrobikuj, draw, 1000000 / 3.5);
    params->bright = 0;
    draw();
}

static void blazinec()
{
    int i;

#define NTEXT sizeof(text)/sizeof(char *)

    char *text[] =
    {
	"the",
	"100 %",
	"ANSI C",
	"PORTABLE",
	"DEMO",
	";^D",
	"(^;",
	"FULL",
	"SVGA",
	"TEXT",
	"MODE",
	"",
	"DEVELOPED",
	"UNDER",
	"LINUX",
	"!",
	"!",
	"!",
	"?",
    };
    float sizes[] =
    {
	3,
	3.5,
	3.5,
	4.2,
	3,
	2,
	2,
	3,
	3,
	3,
	3,
	1,
	5,
	3,
	3,
	1,
	2,
	3,
	4.2,
    };
    if (context->driver->print != NULL)
	params->dither = AA_NONE;
    for (i = 0; i < NTEXT; i++) {
	strobikstart();
	params->randomval = 0;
	hlaska(text[i], sizes[i]);
	strobikend();
    }
    strobikstart();
    params->dither = AA_FLOYD_S;
    draw();
}

extern int dualmode;

void vezen(struct image *i1, struct image *i2, struct image *i3, struct image *i4)
{
    drawptr = NULL;
    dualmode = 0;
    strobikstart();
    clrscr();
    dispimg(i1, dual);
    strobikend();
    bbwait(500000);
    strobikstart();
    clrscr();
    dispimg(i2, dual);
    strobikend();
    bbwait(500000);
    strobikstart();
    clrscr();
    dispimg(i3, dual);
    strobikend();
    bbwait(500000);
    strobikstart();
    clrscr();
    dispimg(i4, dual);
    if (dual)
	dispimg(i4, dual);
    strobikend();
    draw();
    bbupdate();
    bbwait(1000000);
}

void scene1(void)
{
    aa_showcursor(context);
    textclrscr();
    clrscr();
    text = "Please wait. Precalculating data";
    cursorx = cursory = 0;
    bright = 255;
    pos = delta = dist = 0;
    f = -10.0;
    randshift = randcharacters = randattrs = 0;
    timestuff(-10, calculateslow, drawwait, ETIME1);
    timestuff(-40, calculateslow, drawwait, ETIME1);
    timestuff(-80, calculateslow, drawwait, ETIME1);
    timestuff(-30, calculatefast, drawwait, ETIME1);
    timestuff(-200, calculatefast, drawwait, ETIME1);
    timestuff(-420, calculatefast, drawwait, ETIME);
    randshift = 1;
    timestuff(600, calculatefast, drawwait, ETIME1);
    randshift = MAXSHIFT;
    params->randomval = MAXEFECT2 + 50;
    text = "";
    aa_gotoxy(context, 0, 0);
    aa_hidecursor(context);
    timestuff(20, calculatefastest, drawwait3, ETIME);
    timestuff(20, calculatefastest, drawwait3, ETIME);
    randcharacters = 1;
    timestuff(20, calculatefastest, drawwait3, ETIME);
    randcharacters = MAXRAND;
    randattrs = 1;
    timestuff(20, calculatefastest, drawwait3, ETIME);
    randattrs = MAXRAND;
    timestuff(20, calculatefastest, drawwait3, ETIME);
    drawptr = drawwait2;
    play();
    timestuff(20, decrandom, draw, EFECT2 * 1000000);
    timestuff(20, decbright, draw, ETIME);
    pos = aa_imgheight(context) * 2;
    drawptr = drawwait4;
    timestuff(60, makepos, draw, 5 * 1000000);
    timestuff(60, makepos1, draw, 0.2 * 1000000);
    timestuff(60, makepos2, draw, 0.3 * 1000000);
    drawptr = NULL;
    blazinec();
}

void introscreen(void)
{
    text = "Please wait. Precalculating data";
    drawwait();
    text = "";

}
