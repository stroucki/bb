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
#include <string.h>
#include "bb.h"

#define ETIME 650000
#define RATE 60
static char *mesg;
static char *lastmesg;
static char pos, lastpos;
#define MAXPOS (ETIME1)
#define STATE (TIME-starttime)
#define N 10000
#define ETIME1 2500000


void blur(int n)
{
    unsigned char *pos = context->imagebuffer, *posend = pos + aa_imgwidth(context) * aa_imgheight(context);

    while (pos < posend) {
	(*pos) >>= n;
	pos++;
    }
}

void dvojprujezd(int starttime, char *text2, char *text1)
{
    int pos = (getwidth(2) * strlen(text1) + 1);
    centerprint(-pos / 2 + (aa_imgwidth(context) + pos * 1.2) * STATE / (endtime - starttime), 2 * aa_imgheight(context) / 3, 2, 255, text1, 0);
    pos = (getwidth(2) * strlen(text2) + 1);
    centerprint(aa_imgwidth(context) + pos / 2 - (aa_imgwidth(context) + pos * 1.2) * STATE / (endtime - starttime), aa_imgheight(context) / 3, 2, 255, text2, 0);
}

static void dvojprujezd2(int starttime, char *text1, char *text2)
{
    int pos = (getwidth(2) * strlen(text1) + 1);
    centerprint(-pos / 2 + (aa_imgwidth(context) + pos * 1.2) * STATE / (endtime - starttime), aa_imgheight(context) / 3, 2, 255, text1, 0);
    pos = (getwidth(2) * strlen(text2) + 1);
    centerprint(aa_imgwidth(context) + pos / 2 - (aa_imgwidth(context) + pos * 1.2) * STATE / (endtime - starttime), 2 * aa_imgheight(context) / 3, 2, 255, text2, 0);
}

void message(char *text, int starttime)
{
    if (STATE > 0 && STATE < MAXPOS) {
	double pp = STATE * M_PI / MAXPOS * 2;
	double width = (1 + cos(pp)) * 5 + 2;
	int pos = aa_imgheight(context) - aa_imgheight(context) * STATE / MAXPOS;
	if (width <= 0)
	    return;
	centerprint(aa_imgwidth(context) / 2, pos, width, sin(pp / 2) * 255, text, 0);
    }
}

static float xpos, xpos1, f, t;

#define TTIME1 80
#define TTIME 100
#define G 0.02
#define AMP 40

void ctrllepic(int i)
{
    for (; i; i--) {
	t++;
	f += G,
	    f *= 0.96;
	if (t < TTIME) {
	    if (t < TTIME1)
		f -= xpos / (double) AMP;
	    else
		f -= xpos / (double) AMP / 2;
	    xpos += f;
	}
	else {
	    xpos1 += f;
	    xpos += G;
	}
	if (xpos < 0)
	    xpos = 0, f = -f;
    }
}

void drawlepic(char *mesg)
{
    print(0, xpos1 * aa_imgheight(context), aa_imgwidth(context) / (double) strlen(mesg), aa_imgheight(context) * xpos, font, 255, mesg);
}

void drawzoomer(char *mesg, int starttime, int pos)
{
    float width = 1000000.0 / (STATE);
    if (width > 1.0) {
	int color = (width - 1) * 255;
	if (color > 255)
	    color = 255;
	centerprint(aa_imgwidth(context) / 2, pos * aa_imgheight(context) / 6, width, color, mesg, 0);
    }
}

#define ETIME2 1000000

void drawlevotoc(char *mesg, char *mesg1, int starttime)
{
    if (STATE < ETIME2 && STATE > 0) {
	print(0, 0, aa_imgwidth(context) / (double) strlen(mesg) * ((float) STATE / ETIME2), aa_imgheight(context), font, 255, mesg);
	print(aa_imgwidth(context) * ((float) STATE / ETIME2), 0, aa_imgwidth(context) / (double) strlen(mesg1) * (1 - (float) STATE / ETIME2), aa_imgheight(context), font, 255, mesg1);
    }
    if (STATE > ETIME2)
	print(0, 0, aa_imgwidth(context) / (double) strlen(mesg), aa_imgheight(context), font, 255, mesg);
}

void drawpravotoc(char *mesg, char *mesg1, int starttime)
{
    if (STATE < ETIME2 && STATE > 0) {
	print(0, 0, aa_imgwidth(context) / (double) strlen(mesg1) * (1 - (float) STATE / ETIME2), aa_imgheight(context), font, 255, mesg1);
	print(aa_imgwidth(context) * (1 - (float) STATE / ETIME2), 0, aa_imgwidth(context) / (double) strlen(mesg) * ((float) STATE / ETIME2), aa_imgheight(context), font, 255, mesg);
    }
    if (STATE > ETIME2)
	print(0, 0, aa_imgwidth(context) / (double) strlen(mesg), aa_imgheight(context), font, 255, mesg);
}

void drawhorotoc(char *mesg, char *mesg1, int starttime)
{
    if (STATE < ETIME2 && STATE > 0) {
	print(0, 0, aa_imgwidth(context) / strlen(mesg), aa_imgheight(context) * ((float) STATE / ETIME2), font, 255, mesg);
	print(0, aa_imgheight(context) * ((float) STATE / ETIME2), aa_imgwidth(context) / strlen(mesg1), aa_imgheight(context) * (1 - (float) STATE / ETIME2), font, 255, mesg1);
    }
    if (STATE > ETIME2)
	print(0, 0, aa_imgwidth(context) / strlen(mesg), aa_imgheight(context), font, 255, mesg);
}

#define LTIME 200000

void drawprujezd(char *mesg, int starttime)
{
    double height = aa_imgheight(context) / 3 + aa_imgheight(context) / 4 * cos(STATE / (double) LTIME);
    double width = (double) aa_imgwidth(context) * 0.75 * 2.0 / 3.0 / 3;
    int pos = aa_imgwidth(context) - width * STATE / LTIME;
    print(pos + height, (aa_imgheight(context) - height) / 2, width, height, font, 255, mesg);
}

void initlepic()
{
    t = 0;
    f = 0;
    xpos = 0;
    xpos1 = 0;
}

static void messaguj()
{
    clrscr();
    drawzoomer(lastmesg, starttime - ETIME, lastpos);
    drawzoomer(mesg, starttime, pos);
}

static void mydraw()
{
    clrscr();
    dvojprujezd2(starttime, "Greetings", "To");
    /*drawpravotoc(mesg, lastmesg, starttime); */
}

void scene2(void)
{
    char *pokec[] =
    {
	"Future",
	"Crew",
	"Triton",
	"Cascada",
	"Complex",
	"Pascal",
	"Titans",
	"Xography",
	"Sonic PC",
	"Scrymag",
	"...",
	"Microsoft",
	"",
	"!?!",
    };
    int i;
    params->randomval = 0;
    drawptr = mydraw;
    timestuff(60, NULL, draw, 2.75 * 1000000);

    drawptr = messaguj;
    mesg = "";
    pos = 2;
    lastpos = 1;
    for (i = 0; i < sizeof(pokec) / sizeof(char *); i++) {
	lastpos = pos;
	pos++;
	if (pos > 4)
	    pos = 2;
	lastmesg = mesg;
	mesg = pokec[i];
	if (i == sizeof(pokec) / sizeof(char *) - 1)
	     timestuff(0, NULL, draw, 3 * ETIME);
	else
	    timestuff(0, NULL, draw, ETIME);
    }
}
