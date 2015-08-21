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
#include <math.h>
#include "bb.h"

#define DWIDTH aa_imgwidth(context)
#define DHEIGHT aa_imgheight(context)
#define STIME 11*1000000
#define TTIME 1000000
#define N_FRAMERATE 35
#define ZTABSIZE ((int)((float)(MAXZOOM-MINZOOM)/ZOOMSTEP))

#define ZOOMSTEP 0.01
#define MINZOOM 0.80
#define MAXZOOM 2.70
#define POW4(x) (x*x*x*x)

#define RAD(angle) ((double) angle * M_PI / 180)
#define RANDOM(x) (rand()%x)

static aa_palette TempPal, Pal[2];
static char *PlasmaTbl;
static unsigned char pos1, pos2, pos3, pos4;
static unsigned int pnum;
static int dir = 1;

static void compute_custom_palette()
{
    int i;

    for (i = 0; i < 64; i++) {
	Pal[0][i] = i * 4;
	Pal[1][i] = i * 1;
    }
    for (i = 64; i < 128; i++) {
	Pal[0][i] = (128 - i) * 4;
	Pal[1][i] = (128 - i) * 1;
    }
    for (i = 128; i < 192; i++) {
	Pal[0][i] = (i - 128) * 1;
	Pal[1][i] = (i - 128) * 4;
    }
    for (i = 192; i < 256; i++) {
	Pal[0][i] = (256 - i) * 1;
	Pal[1][i] = (256 - i) * 4;
    }
}

void initscene3(void)
{
    int i;
    float czoom;

    for (czoom = MINZOOM, pnum = 0; pnum < ZTABSIZE; czoom += ZOOMSTEP, pnum++)
	for (i = 0; i < 256; i++)
	    PlasmaTbl[(pnum << 8) + i] = (char) (((float) cos(RAD(i * 256 / 180))) * 256 / (POW4(czoom)));

    compute_custom_palette();
    pnum = 0;
}

static void draw_plasma(void)
{
    int i, j, color;
    unsigned char p1, p2, p3 = pos3, p4 = pos4;
    char *text[] =
    {
	"STILL",
	"WATCHING",
	"BB",
	"?",
	"GREAT",
	"",
	"",
	"NOW",
	"IT'S",
	"A GREAT",
	"TIME",
	"TO",
	"FILL",
	"IN",
	"YOUR",
	"REGISTRATION",
	"CARD",
	"",
	"????",
	"NEVER",
	"MORE",
	"",
	"(E.A. POE)",
	"..."
    };
    for (i = 0; i < DWIDTH; i++) {
	p1 = pos1;
	p2 = pos2;
	for (j = 0; j < DHEIGHT; j++) {
	    color = PlasmaTbl[(pnum << 8) + p1] + PlasmaTbl[(pnum << 8) + p2] + PlasmaTbl[(pnum << 8) + p3] + PlasmaTbl[(pnum << 8) + p4] + PlasmaTbl[(pnum << 8) + ((unsigned char) i)] + PlasmaTbl[(pnum << 8) + ((unsigned char) j)];
	    aa_putpixel(context, i, j, color);
	    p1 += 3;
	    p2 += 1;
	}
	p3 += 2;
	p4 += 3;
    }
    for (i = 0; i < sizeof(text) / sizeof(char *); i++) {
	message(text[i], starttime + STIME + i * TTIME);
    }

    aa_renderpalette(context, TempPal, params, 0, 0, aa_imgwidth(context), aa_imgheight(context));
    aa_flush(context);
}

static void move_plasma(void)
{

    pos1 -= 4 + RANDOM(2);
    pos3 += 4 + RANDOM(1);
    pos2 -= RANDOM(2);
    pos4 -= RANDOM(2);

    pnum += dir;
    if (pnum > (ZTABSIZE - 2))
	dir = -1;
    else if (pnum <= 0)
	dir = 1;
}

static void cplasma(aa_palette dest, aa_palette src, aa_palette temp, float step)
{
    int j;

    for (j = 0; j < 256; j++)
	temp[j] = src[j] + ((dest[j] - src[j]) * step / 64);

}


static void do_plasma(int step)
{
    static int m = 0, n = 0, f = 0;
    int i = 0;

    if (TIME > endtime - 128 * 1000000 / N_FRAMERATE)
	params->bright -= step * 2;
    else {
	params->bright -= step * 2;
	if (params->bright < 0)
	    params->bright = 0;
    }

    if ((f += step) > 64) {
	f = 0;
	m = n, n = RANDOM(2);
    }
    cplasma(Pal[n], Pal[m], TempPal, f);
    for (i = 0; i < step; i++)
	move_plasma();
}

void scene3(void)
{
    pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0, pnum = 0, dir = 1;
    PlasmaTbl = (char *) malloc(ZTABSIZE * 256);
    params->dither = AA_FLOYD_S;
    initscene3();
    bbwait(500000);
    timestuff(-N_FRAMERATE, do_plasma, draw_plasma, 42 * 1000000);
    params->bright = 0;
    free(PlasmaTbl);
    clrscr();
}
