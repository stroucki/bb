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
#include <ctype.h>
#include <time.h>
#include <aalib.h>
#include "bb.h"

int finish_stuff, starttime, endtime;
int dual = 0;
static int quitnow = 0;
int loopmode;
aa_context *context;
aa_renderparams *params;
int TIME;
tl_timer *scenetimer;
struct font *font;

double getwidth(double size)
{
    double height = aa_imgheight(context) / size;
    double width = height * (double) aa_imgwidth(context) * 0.75 / aa_imgheight(context) * aa_mmheight(context) / aa_mmwidth(context);
    return (width);
}

void centerprint(int x, int y, double size, int color, char *text, int mode)
{
    if (!dual || !mode) {
	double height = aa_imgheight(context) / size;
	double width = height * (double) aa_imgwidth(context) * 0.75 / aa_imgheight(context) * aa_mmheight(context) / aa_mmwidth(context);
	print(x - (width * strlen(text)) / 2, y - height / 2, width, height, font, color, text);
    }
    else {
	if (mode & 1) {
	    double height = aa_imgheight(context) / size;
	    double width = height * (double) aa_imgwidth(context) * 0.75 / aa_imgheight(context) * aa_mmheight(context) / aa_mmwidth(context);
	    print(x / 2 - (width * strlen(text)) / 2, y - height / 2, width, height, font, color, text);
	}
	if (mode & 2) {
	    double height = aa_imgheight(context) / size;
	    double width = height * (double) aa_imgwidth(context) * 0.75 / aa_imgheight(context) * aa_mmheight(context) / aa_mmwidth(context);
	    print(aa_imgwidth(context) / 2 + x / 2 - (width * strlen(text)) / 2, y - height / 2, width, height, font, color, text);
	}
    }
}

void centerprinth(int x, int y, double size, int color, char *text, int mode)
{
    if (!mode || !dual) {
	double width = aa_imgwidth(context) / size;
	double height = width * (double) aa_imgheight(context) * 1.333 / aa_imgwidth(context) * aa_mmwidth(context) / aa_mmheight(context);
	print(x - (width * strlen(text)) / 2, y - height / 2, width, height, font, color, text);
    }
    else {
	if (mode & 1) {
	    double width = aa_imgwidth(context) / size / 2;
	    double height = width * (double) aa_imgheight(context) * 1.333 / aa_imgwidth(context) * aa_mmwidth(context) / aa_mmheight(context);
	    print(x / 2 - (width * strlen(text)) / 2, y - height / 2, width, height, font, color, text);
	}
	if (mode & 1) {
	    double width = aa_imgwidth(context) / size / 2;
	    double height = width * (double) aa_imgheight(context) * 1.333 / aa_imgwidth(context) * aa_mmwidth(context) / aa_mmheight(context);
	    print(aa_imgwidth(context) / 2 + x / 2 - (width * strlen(text)) / 2, y - height / 2, width, height, font, color, text);
	}
    }
}

static void (*control1) (int);
static int called = 0;

static void mycontrol(int i)
{
    called = 1;
    if (control1 != NULL)
	control1(i);
}

int bbupdate()
{
    int ch;
    tl_update_time();
    update_sound();
    TIME = tl_lookup_timer(scenetimer);
    ch = aa_getkey(context, 0);
    switch (ch) {
    case 's':
    case 'S':
    case AA_BACKSPACE:
	finish_stuff = 1;
	break;
    case AA_ESC:
    case 'q':
	finish_stuff = 1, quitnow = 1;
    }
    return (ch);
}

void timestuff(int rate, void (*control) (int), void (*draw) (void), int maxtime)
{
    int waitmode = 0, t;
    tl_timer *timer;
    bbupdate();
    /*starttime = TIME; */
    endtime = starttime + maxtime;
    timer = tl_create_timer();
    if (control == NULL) {
	rate = -40;
    }
    if (rate < 0) {
	waitmode = 1, rate = -rate;
	control1 = control;
	tl_set_multihandler(timer, mycontrol);
    }
    else
	tl_set_multihandler(timer, control);
    tl_set_interval(timer, 1000000 / rate);
    tl_add_timer(syncgroup, timer);
    tl_reset_timer(timer);
    if (control != NULL)
	control(1);
    while (!finish_stuff && TIME < endtime) {
	called = 0;
	t = tl_process_group(syncgroup);
	bbupdate();
	if (TIME > endtime)
	    break;
#ifdef __DJGPP__
	if (0)
#else
	if (!called && waitmode)
#endif
	    tl_sleep(t);
	else {
	    if (draw != NULL)
		draw();
	}
    }
    starttime = endtime;
    tl_free_timer(timer);
}

void bbwait(int maxtime)
{
    int wait;
    if (finish_stuff)
	return;
    bbupdate();
    endtime = starttime + maxtime;
#ifdef __DJGPP__
    while (TIME < endtime)
	bbupdate();
#else
    wait = maxtime + starttime - TIME;

    if (wait > 0) {
	tl_sleep(maxtime + starttime - TIME);
    }
#endif
    starttime = endtime;
}

void bbflushwait(int maxtime)
{
    int wait;
    if (finish_stuff)
	return;
    wait = maxtime + starttime - TIME;
    if (wait > 0) {
	aa_flush(context);
    }
    bbwait(maxtime);
}

static int stage = 1;

int bbinit(int argc, char **argv)
{
    aa_defparams.supported|= AA_NORMAL_MASK | AA_BOLD_MASK | AA_DIM_MASK;
    aa_parseoptions(NULL, NULL, &argc, argv);
    if (argc != 1 && (argc != 2 || ((argv[1][0] <= '0' || argv[1][0] > '8') && strcmp(argv[1], "-loop")))) {
	printf("Usage: bb [aaoptions] [number]\n\n");
	printf("Options:\n"
	       "  -loop          play demo in infinite loop\n\n"
	       "AAlib options:\n%s\n", aa_help);
	exit(1);
    }
    context = aa_autoinit(&aa_defparams);
    if (!context) {
	printf("Failed to initialize aalib\n");
	exit(2);
    }
    if (!aa_autoinitkbd(context, 0)) {
	aa_close(context);
	printf("Failed to initialize keyboard\n");
	exit(3);
    }
    if (argc == 2 && !strcmp(argv[1], "-loop"))
	loopmode = 1;
    else if (argc == 2)
	stage = atol(argv[1]);
    aa_hidecursor(context);
    return 1;
}

int bb(void)
{
    aa_gotoxy(context, 0, 0);
    introscreen();
    params = aa_getrenderparams();
    aa_render(context, params, 0, 0, 1, 1);
    font = uncompressfont( /*context->params.font */ &aa_font16);
    scenetimer = tl_create_timer();
    srand(time(NULL));
    if (stage != 1)
	finish_stuff = 1;
    do
	switch (stage) {
	default:
	case 1:
	    load_song("bb.s3m");
	    bbupdate();
	    starttime = endtime = TIME;

	    scene1();
	    scene3();
	    if (quitnow)
		goto quit;
	    vezen(&fk1, &fk2, &fk3, &fk4);
	    messager("FILIP KUPSA known as FK, Tingle Notions, Dawn Music\n"
		"birth: June 22 1979, Tabor, Czech Republic, sex: male\n"
		     "\n"
	     "1992 - Changed his piano for 386/mp.com/pc-speaker music\n"
		     "1993 - Got his first Sound Blaster\n"
		     "1995 - Changed his SB for a new GUS technology\n"
		     "1996 - Composed his first great hits\n"
		     "1996 - FAT recomposition made by Windows 95\n"
		     "1997 - Released his musac in BB\n"
		     "\n"
		     "1998 - Got retired\n"
		     "\n"
		     "Contact address: via KT");
	    devezen2();
	    scene4();
	    scene2();
	    if (quitnow)
		goto quit;
	    vezen(&ms1, &ms2, &ms3, &ms4);
	    messager("MOJMIR SVOBODA known as MS, TiTania, MSS, Bill\n"
		     "birth: ??, Tabor, Czech Republic, sex: ? male ?\n"
		     "\n"
		     "1993 - Installed Linux on his 386sx/25 + 40MB HDD\n"
		     "1994 - Removed Linux to make space for Doom\n"
		   "1995 - Reinstalled Linux on his 486Dx4/120 + 850MB\n"
		     "1996 - Removed Linux to make space for Windows 95\n"
		     "\n"
		     "1997 - Removed Windows 95 to make space for aalib\n"
		     "\n"
		     "Contact address: titania@mbox.vol.cz");
	    devezen3();
	    scene8();
	    scene6();
	case 2:
	    if (quitnow)
		goto quit;
	    vezen(&kt1, &kt2, &kt3, &kt4);
	    messager("KAMIL TOMAN known as KT, Kato, Whale, Bart\n"
		 "birth: May 19 1979, Tabor, Czech Republic, sex: male\n"
		     "\n"
		     "1993 - Became a linux extremist\n"
		     "1993 - Successful attempt to establish a secret organization\n"
		     "       Commandline Brotherhood\n"
		     "1995 - Action 'koules' - a secret project to train brotherhood\n"
		     "       members - covered under a game design\n"
		     "\n"
		 "1998 - Heading a new wave of command line revolution\n"
		     "\n"
		     "Contact address: toman@horac.ta.jcu.cz");
	    bbupdate();
	    starttime = endtime = TIME;
	    devezen1();
	    if (quitnow)
		goto quit;
	    scene7();
	    if (quitnow)
		goto quit;
	    scene5();
	    if (quitnow)
		goto quit;
	    scene10();
	    vezen(&hh1, &hh2, &hh3, &hh4);
	    messager("JAN HUBICKA known as HH, Jahusoft, HuJaSoft, JHS, UNIX, Honza\n"
		  "birth: Apr 1 1978, Tabor, Czech Republic, sex: male\n"
		     "\n"
		     "1991 - Installed underground hackers OS Linux\n"
		     "1995 - Headed Action 'koules'\n"
		     "1996 - Famous troan XaoS to convert all windows instalations\n"
		     "       into Linux\n"
		     "\n"
		     "1998 - Secret plan to make `Text Windows` system to confuse users\n"
		 "2001 - Planning an assassination of dictator Bill G.\n"
		     "\n"
		     "Contact address: hubicka@paru.cas.cz");
	    devezen4();
	    if (quitnow)
		goto quit;
	    credits();
	    if (quitnow)
		goto quit;
	case 3:
	    if (loopmode)
		break;
	    credits2();
	}
    while (loopmode);
  quit:;
    aa_close(context);
    return (0);
}
