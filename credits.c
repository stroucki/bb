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
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "bb.h"
#define STAR 1
#define SLOWDOWN 2
#define NORMAL 3
#define WIND 4


#define MAXSTARS	450

#if 1
#define MAXFAR		1000
#define MAXFAR1		100000
#define GFARCOR		256
#define MAXNEAR		1
#define MAXLEFT		-(MAXFAR*255)
#define MAXRIGHT	(MAXFAR*255)
#define MAXTOP		-(MAXFAR*255)
#define MAXBOTTOM	(MAXFAR*255)
#endif
#if 0
#define G 1000
#define SSPEED 10
#define SSPEED1 20
#else
#define G 600
#define SSPEED 40
#define SSPEED1 50
#endif

#define DWIDTH aa_imgwidth(context)
#define DHEIGHT aa_imgheight(context)

#define NCREDITS 6
#define CTIME 3000000
#define CTIME1 (CTIME/NCREDITS)
#define TOTAL (CTIME1*CSIZE+CTIME)
#define STATE (TIME-starttime)
#define TABSIZE (60*2)

#define CSIZE (sizeof(text)/sizeof(char *))
#define RANDOM(x) (rand()%x)
#define RANDOM2(x,y) (((rand()/(float)RAND_MAX)*(2*(y)))+(x))
#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))
static int windx = 0;
static int windz = -30;
static int mode = STAR;
static float windspeed = 0, tospeed = 0;
static float windangle = 0, toangle = 0;

static char *text[] =
{
    "Thank you",
    "For",
    "watching",
    "BB",
    ".",
    "...",
    ".",
    "CREDITS:",
    " ",
    "FK:",
    "Music",
    " ",
    " ",
    "MS:",
    "3d engine",
    "tyre",
    " ",
    " ",
    "KT:",
    "Sound",
    "engine",
    "Sound",
    "synchro",
    "Intro",
    "Plasma",
    "Guard",
    "stone",
    "Texts",
    "Titles",
    "Stars",
    "Snowing",
    " ",
    " ",
    "HH:",
    "AAlib",
    "Intro",
    "Invaders",
    "Fire",
    "Greetings",
    "Photos",
    "XaoS",
    "Zebra",
    "Titeling",
    "Texts",
    "Snowing",
    "Timing",
    "system",
    "Outro",
    "Pc speaker",
    "driver",
    " ",
    ".",
    "...",
    ".",
    "Special",
    "Thanks",
    "to",
    ":",
    "Eva",
    "Hubickova",
    "for photos",
    " ",
    "Texas",
    "Linux",
    "users",
    "group",
    "Their",
    "logo",
    "- great",
    "inspiration",
    " ",
    "Thomas",
    "Marsh",
    "For help",
    "with",
    "fractal",
    "zooming",
    " ",
    "IBM",
    "for MDA",
    "the",
    "primary",
    "gfx",
    "target",
    " ",
    "Jiri",
    "Matousek",
    "for help",
    "with",
    "searching",
    "algorithm",
    " ",
    "0rfelyus",
    "for help",
    "with",
    "dithering",
    " ",
    "DJ",
    "for DJGPP",
    " ",
    "MikMak",
    "for MikMod",
    " ",
    "Richard",
    "Stallman",
    "for GNU",
    " ",
    "Linus",
    "for linux",
    ".",
    "...",
    ".",
    "This demo",
    "and",
    "ascii art",
    "library",
    "is free",
    "software",
    "...",
    "you can",
    "redistribute",
    "it",
    "and/or",
    "modify",
    "under",
    "terms",
    "of GNU",
    "General",
    "Public",
    "Licence",
    "as",
    "published",
    "by the",
    "Free",
    "Software",
    "Foundation",
    " ",
    ".",
    "...",
    ".",
    "see",
    "COPYING",
    "for",
    "details",
    "or",
    "README",
    "for",
    "pointer",
    "to",
    "sources",
    " ",
    " ",
    "! WARNING !",
    "Do NOT",
    "read",
    "the sources",
    "unless",
    "you really",
    "know",
    "what you",
    "are doing",
    ".",
    "...",
    ".",
    "(C)1997",
    "AA",
};
struct starTYPE {
    int x, y, z;
    int x2, y2;
};

static int *horiztab;
static int *verttab;

typedef struct starTYPE starTYPE;
typedef starTYPE starfieldTYPE[MAXSTARS];

static starfieldTYPE s;

static void precalculate()
{
    int i, p, p1;
    horiztab = malloc(TABSIZE * sizeof(*horiztab));
    verttab = malloc(TABSIZE * sizeof(*verttab));
    p1 = MAXLEFT / SSPEED;
    for (i = 0; i < TABSIZE; i++) {
	p = cos((double) i * 2.0 * M_PI / (TABSIZE - 1)) * MAXLEFT / SSPEED;
	horiztab[i] = p - p1;
	p1 = p;
    }
    p1 = 0;
    for (i = 0; i < TABSIZE; i++) {
	p = fabs(sin((double) i * 2 * M_PI / TABSIZE) * MAXLEFT / SSPEED1) + i * G;
	verttab[i] = p - p1;
	p1 = p;
    }
}



static void create_star(int i)
{
    s[i].x = RANDOM2(MAXLEFT, MAXRIGHT);
    s[i].y = RANDOM2(MAXTOP, MAXBOTTOM);
    s[i].x2 = DWIDTH / 2;
    s[i].y2 = DHEIGHT / 2;
    s[i].z = RANDOM2(0, MAXFAR) + 1;
}

static void draw_starfield(void)
{
    int i;

    clrscr();
    for (i = 0; i < MAXSTARS; i++) {
	if (s[i].x2 > 0 && s[i].x2 < DWIDTH && s[i].y2 > 0 && s[i].y2 < DHEIGHT)
	    aa_putpixel(context, s[i].x2, s[i].y2, MIN(MAXFAR1 / MAX(s[i].z - GFARCOR, 1), 255));
    }
    aa_putpixel(context, aa_imgwidth(context) / 2, aa_imgheight(context) / 2, 0);
}

static void move_starfield(int step)
{
    int i;
    static int counter;
    for (; step; step--) {
	if (mode == SLOWDOWN)
	    windz = -30 * (endtime - TIME) / (endtime - starttime);
	if (mode == WIND) {
	    if (windspeed >= tospeed) {
		windspeed -= 0.05;
		if (windspeed <= tospeed)
		    tospeed = rand() % 40 - 10;
		if (tospeed < 0)
		    tospeed = 0;
	    }
	    if (windspeed < tospeed)
		windspeed += 0.05;
	    if (windangle <= toangle) {
		windangle += 1.0 / 200.0;
		if (windangle >= toangle)
		    toangle = -M_PI * 2 / 3 + (rand() % 1000) * M_PI / 1000.0 * 4 / 3;
	    }
	    if (windangle >= toangle)
		windangle -= 1.0 / 200.0;
	    windx = sin(windangle) * windspeed * 100;
	    windz = -cos(windangle) * windspeed;
	}
	counter++;
	for (i = 0; i < MAXSTARS; i++) {
	    if (step == 1) {
		if (s[i].z) {
		    s[i].x2 = ((((s[i].x - 256) / s[i].z) * aa_imgwidth(context)) >> 8) + aa_imgwidth(context) / 2;
		    s[i].y2 = ((((s[i].y - 256) / s[i].z) * aa_imgwidth(context) * aa_mmheight(context) / aa_mmwidth(context)) >> 8) + aa_imgheight(context) / 2;
		}
		while (!(s[i].x2 > -DWIDTH / 3 && s[i].x2 < 4 * DWIDTH / 3 && s[i].y2 > -DHEIGHT / 3 && s[i].y2 < 4 * DHEIGHT / 3 && s[i].z > 0 && s[i].z <= MAXFAR)) {
		    create_star(i);
		    s[i].x2 = ((((s[i].x - 256) / s[i].z) * aa_imgwidth(context)) >> 8) + aa_imgwidth(context) / 2;
		    s[i].y2 = ((((s[i].y - 256) / s[i].z) * aa_imgwidth(context) * aa_mmheight(context) / aa_mmwidth(context)) >> 8) + aa_imgheight(context) / 2;
		}
	    }
	    if (mode != STAR) {
		s[i].x += horiztab[(counter + i) % TABSIZE] + windx;
		s[i].y += verttab[(counter + i) % TABSIZE];
	    }
	    s[i].z += windz;
	}
    }
}

static void drawcredits()
{
    int state = STATE;
    double pos = sin(state * M_PI / 900000);
    int xpos1 = aa_imgwidth(context) / 2 + pos * 0.16 * aa_imgwidth(context);
    int xpos2 = aa_imgwidth(context) / 2 - pos * 0.16 * aa_imgwidth(context);
    int i, time = 0;
    int height = aa_imgheight(context) / NCREDITS;
    draw_starfield();
    for (i = 0; i < CSIZE; i++) {
	if (state > time && state < time + CTIME) {
	    centerprint((i & 1) ? xpos1 : xpos2, -height + (aa_imgheight(context) + 2 * height) * (CTIME + time - state) / CTIME, NCREDITS - 2, 255, text[i], 0);
	}
	time += CTIME1;
    }
    if (state > TOTAL - CTIME)
	params->randomval = (state - TOTAL + CTIME) * 800 / CTIME;
}
static void mydraw1()
{
    if (mode == STAR) {
	if (TIME < endtime) {
	    params->bright = -255 + (TIME - starttime) * 255 / (endtime - starttime);
	}
	else
	    params->bright = 0;
    }
    else
	params->bright = 0;
    draw_starfield();
}
void credits(void)
{
    windx = 0;
    windz = -30;
    mode = STAR;
    windspeed = 0, tospeed = 0;
    windangle = 0, toangle = 0;
    textclrscr();
    aa_flush(context);
    load_song("bb2.s3m");
    precalculate();
    bbupdate();
    starttime = endtime = TIME;
    play();
    params->bright = -255;
    drawptr = mydraw1;
    params->dither = AA_NONE;
    mode = STAR;
    timestuff(-60, move_starfield, draw, 6000000);
    mode = SLOWDOWN;
    timestuff(-60, move_starfield, draw, 4000000);
    mode = NORMAL;
    timestuff(-60, move_starfield, draw, 8000000);
    mode = WIND;
    params->bright = 0;
    drawptr = drawcredits;
    timestuff(-60, move_starfield, draw, TOTAL);
    free(horiztab);
    free(verttab);
}
