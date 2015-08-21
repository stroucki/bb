/* This file was taken from XaoS-the fast portable realtime interactive 
   fractal zoomer. but it is simplified for BB. You may get complette
   sources at XaoS homepage (http://www.paru.cas.cz/~hubicka/XaoS
 */
/* 
 *     XaoS, a fast portable realtime fractal zoomer 
 *                  Copyright (C) 1996 by
 *
 *      Jan Hubicka          (hubicka@paru.cas.cz)
 *      Thomas Marsh         (tmarsh@austin.ibm.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifdef _plan9_
#include <u.h>
#include <stdio.h>
#include <libc.h>
#else
#ifdef __DJGPP__
#include "aconfig.dos"
#else
#include "aconfig.h"
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <signal.h>
#endif
#include "config.h"
#ifndef _plan9_
#include <assert.h>
#endif
#include "bb.h"
#include "zoom.h"
#include "autopilo.h"
#include "complex.h"
#include "ui.h"
#include "timers.h"
#define ETIME1 (25*1000000+150000)
#define ETIME2 (2*1000000)


#define VBUFF (zcontext->stereogram?zcontext->svbuff:zcontext->vbuff)
#define BACK (zcontext->stereogram?zcontext->sback:zcontext->back)
#define SIZE 1024
#define add_buffer(char) if(((end+1)%SIZE)!=begin) {keybuffer[end]=char;end=(end+1)%SIZE;}
#define INTERRUPTIBLE 0
#define ANIMATION 1
#define NEW_IMAGE 2
#define UNINTERRUPTIBLE 3
#define xstoc(x)        (((number_t) (zcontext->s.nc + (x) * ((zcontext->s.mc - zcontext->s.nc) \
                                / (number_t) zcontext->width))))
#define ystoc(y)        (((number_t) (zcontext->s.ni + (y) * ((zcontext->s.mi - zcontext->s.ni) \
                                / (number_t) zcontext->height))))

#define FRAMETIME (1000000/FRAMERATE)

#define FORMULA 0
#define HELP 1
#define DRIVER 2
#define MENU 3


static tl_timer *maintimer /*, *rotatetimer */ ;
static void ui_do_fractal(int);
static int displayed;
static int ui_autopilot(void);
static int incalculation;
static int fastmode = 2;
static int interruptiblemode;
static int autopilot = 0;
static double maxstep = MAXSTEP, speedup = STEP;
static zoom_context *zcontext;
static double mul;
static int drawingtime, tbreak = 0;

static void ui_display(void)
{
    displayed = 1;
    aa_render(context, params, 0, 0, aa_imgwidth(context), aa_imgheight(context));
    aa_flush(context);

#if 0
    if (numbertype != FORMULA)
	rotate_continue();
#endif
}

static int waitcount;
static int waitcount1;
static int starttime1;
static int endtime1;
static int maxtime = 1000000 / 10;
#define WAITTIME  200000
#define WAITTIME1 2000000
static void ui_waitfunc(void)
{
    int l;
    bbupdate();
    tl_process_group(syncgroup);
    l = tl_lookup_timer(maintimer);
    if (interruptiblemode) {
	if (zcontext->incalculation && !starttime1)
	    starttime1 = l;
	else if (starttime1 && !zcontext->incalculation && !endtime1)
	    endtime1 = l;
	if (maxtime && l > maxtime && zcontext->readyforinterrupt)
	    zcontext->interrupt = 1, endtime1 = l;
    }
}
static float get_pixelwidth(int width)
{
    return (aa_mmwidth(context));
}
static float get_pixelheight(int height)
{
    return (aa_mmheight(context));
}
#ifdef _RED_
static void rotate_off(void)
{
    if (rotate) {
	tl_free_timer(rotatetimer);
	rotate = 0;
    }
}
static void rotate_stop(void)
{
    if (rotate && !stopped) {
	tl_remove_timer(rotatetimer);
	stopped = 1;
    }
}
static void rotate_continue(void)
{
    if (rotate && stopped) {
	stopped = 0;
	if (driver->flags & ASYNC_PALETTE) {
	    tl_add_timer(asyncgroup, rotatetimer);
	}
	else
	    tl_add_timer(syncgroup, rotatetimer);
    }
}
static void rotatehandler(int n)
{
    int direct;
    direct = direction * n;
    if (direction > 0)
	direction %= zcontext->num_colors - 1;
    else
	direction = -((-direction) % (zcontext->num_colors - 1));
    if (!direction)
	return;
    rotated = 1;
    rotate_palette(zcontext, direction * n);
    driver->rotate_palette(direction * n);
    if (ministatus && (driver->flags & PALETTE_REDISPLAYS)) {
	driver->print(0, 0, statustext);
    }
}
static void rotate_update(void)
{
    if (rotationspeed < 0)
	direction = -1;
    else
	direction = 1;
    tl_set_interval(rotatetimer, 1000000 / rotationspeed * direction);
}
static void rotate_on(void)
{
    if (driver->rotate_palette != NULL && (driver->flags & PALETTE_ROTATION)) {
	rotate = 1;
	rotatetimer = tl_create_timer();
	rotate_update();
	tl_set_multihandler(rotatetimer, rotatehandler);
	if (driver->flags & ASYNC_PALETTE) {
	    tl_add_timer(asyncgroup, rotatetimer);
	}
	else
	    tl_add_timer(syncgroup, rotatetimer);
    }
    else {
	driver->print(0, 0, "Rotating of palette not supported by this configuration");
    }
}
#endif
static char zoomed[256];
static void updatestatus(void)
{
    double times = (zcontext->currentformula->v.mc - zcontext->currentformula->v.nc) / (zcontext->s.mc - zcontext->s.nc);
    ui_display();
    sprintf(zoomed, "%.2f", times);

    tl_process_group(syncgroup);
    bbupdate();
    drawingtime = tl_lookup_timer(maintimer);
    tl_reset_timer(maintimer);
    if (drawingtime > 200000)
	drawingtime = 200000;
    mul = (double) drawingtime / FRAMETIME;
    incalculation = 0;
}
static void ui_do_julia(double xx, double yy)
{
    int oldw = zcontext->width, oldh = zcontext->height;
    float size;

    unsigned char *vold = zcontext->vbuff;
    incalculation = 1;
    zcontext->pre = xx, zcontext->pim = yy;
    if (zcontext->windowwidth > zcontext->windowheight)
	size = zcontext->windowheight;
    else
	size = zcontext->windowwidth;
    zcontext->width = zcontext->width * size / zcontext->windowwidth - 1;
    zcontext->height = zcontext->height * size / zcontext->windowheight - 1;
    if (!dual)
	zcontext->vbuff += (oldw - zcontext->width) / 2;
    else
	zcontext->vbuff += (oldw / 2 - zcontext->width) / 2;
    do_julia(zcontext,
	     zcontext->pre,
	     zcontext->pim);
    if (dual) {
	zcontext->currentformula = formulas + 1;
	zcontext->vbuff += aa_imgwidth(context) / 2;
	do_julia(zcontext,
		 zcontext->pre,
		 zcontext->pim);
	zcontext->currentformula = formulas;
    }
    zcontext->width = oldw;
    zcontext->height = oldh;
    zcontext->vbuff = vold;
    ui_display();
}
static void ui_do_fractal(int mode)
{
    int time, time1;
    if (mode < fastmode + 1)
	interruptiblemode = 1;
    else
	interruptiblemode = 0;
    if (tbreak)
	tl_reset_timer(maintimer);
    incalculation = 1;
    starttime1 = 0;
    endtime1 = 0;
    set_view(zcontext, &zcontext->s);
    waitcount = tl_lookup_timer(maintimer) / WAITTIME + 2;
    waitcount1 = tl_lookup_timer(maintimer) / WAITTIME1 + 1;
    do_fractal(zcontext, interruptiblemode);
    bbupdate();
    if (interruptiblemode) {
	bbupdate();
	time1 = time = tl_lookup_timer(maintimer);
	time -= endtime1 - starttime1;
	maxtime = time * 5;
	if (maxtime > 1000000 / 10)
	    maxtime = time * 3;
	if (maxtime < 1000000 / 30)
	    maxtime = 1000000 / 30;
	maxtime -= time1 - endtime1;
    }
    updatestatus();
}
static double step = 0;

void ui_tbreak(void)
{
    tbreak = 2;
}


static int ui_autopilot(void)
{
    clean_autopilot();
    return (autopilot ^= 1);
}
static void ui_changed(void)
{
    ui_tbreak();
}

static int ui_mouse()
{
    int mousex = 0;
    int mousey = 0;
    int mousebuttons = 0;
    int inmovement = 0, slowdown = 1;
    static double autopilot_counter = 0;
    static number_t oldx = 0, oldy = 0;
    static int dirty = 0;
    static int pressed;


    if (tbreak)
	mul = 1.0;
    if (dirty)
	ui_do_fractal(NEW_IMAGE), ui_tbreak(), dirty = 0;
    if (autopilot) {
	static int mousex1, mousey1, mousebuttons1;
	autopilot_counter += mul;
	while (autopilot_counter > 1) {
	    do_autopilot(zcontext, &mousex1, &mousey1, &mousebuttons1, ui_changed);
	    autopilot_counter--;
	}
	mousex = mousex1, mousey = mousey1, mousebuttons = mousebuttons1;
    }
    if (tbreak)
	mul = 1;
    if (mul == 0)
	mul = 0.001;
    if (mousebuttons > 0) {
	switch (mousebuttons) {
	case BUTTON1:		/* button 1 */
	    step -= speedup * 2 * mul, slowdown = 0;
	    inmovement = 1;
	    break;
	case BUTTON3:		/* button 3 */
	    step += speedup * 2 * mul, slowdown = 0;
	    inmovement = 1;
	    break;
	case BUTTON2:		/* button 2 */
	    {
		number_t x = xstoc(mousex), y = ystoc(mousey);
		if (pressed && (oldx != x || oldy != y)) {
		    zcontext->s.nc -= x - oldx;
		    zcontext->s.mc -= x - oldx;
		    zcontext->s.ni -= y - oldy;
		    zcontext->s.mi -= y - oldy;
		    if (!step) {
			ui_do_fractal(ANIMATION), ui_tbreak();
		    }
		}
		pressed = 1;
		oldx = xstoc(mousex), oldy = ystoc(mousey);
	    }
	    break;
	default:
	    break;
	}
    }
    if (!(mousebuttons & BUTTON2))
	pressed = 0;
    if (slowdown) {
	if (step > 0) {
	    if (step < speedup * mul)
		step = 0;
	    else
		step -= speedup * mul;
	}
	else if (step < 0) {
	    if (step > -speedup * mul)
		step = 0;
	    else
		step += speedup * mul;
	}
    }
    if (step > maxstep)
	step = maxstep;
    else if (step < -maxstep)
	step = -maxstep;
    if (step) {
	number_t x = xstoc(mousex);
	number_t y = ystoc(mousey);
	number_t mmul = pow((double) (1 + step), (double) mul);
	zcontext->s.mc = x + (zcontext->s.mc - x) * (mmul);
	zcontext->s.nc = x + (zcontext->s.nc - x) * (mmul);
	zcontext->s.mi = y + (zcontext->s.mi - y) * (mmul);
	zcontext->s.ni = y + (zcontext->s.ni - y) * (mmul);
	inmovement = 1;
	ui_do_fractal(ANIMATION);
	if (tbreak)
	    tbreak--;
    }
    if (!inmovement)
	ui_tbreak();
    return (1);
}
#define BTIME 200000
#define BTIME1 1000000
static void main_loop(void)
{
    int inmovement = 1;
    while (TIME < endtime && !finish_stuff) {
	if (TIME < starttime + BTIME) {
	    params->bright = 255 - (TIME - starttime) * 255 / BTIME;
	}
	else
	    params->bright = 0;
	if (TIME > endtime - BTIME1) {
	    params->bright = -255 + (endtime - TIME) * 255 / BTIME1;
	}
	if (zcontext->uncomplette) {
	    inmovement = 1;
	    if (!displayed)
		ui_do_fractal(ANIMATION);
	}
	/*if ((time = tl_process_group(syncgroup)) != -1) {
	   if (!inmovement) {
	   if (time > 1000000 / 50)
	   time = 1000000 / 50;
	   if (!inmovement && !autopilot)
	   tl_sleep(time);
	   }
	   inmovement = 1;
	   } */
	displayed = 0;
	inmovement = ui_mouse();
    }
}
static void flip_buffers(void)
{
    context->imagebuffer = zcontext->back;
}
static int sef, eef;
static double is, ie, rs, re;
static void juliov(void)
{
    double time = (TIME - starttime) / (double) (endtime - starttime);
    if (sef) {
	if (TIME < starttime + BTIME) {
	    params->bright = 255 - (TIME - starttime) * 255 / BTIME;
	}
	else
	    params->bright = 0;
    }
    if (eef) {
	if (TIME > endtime - BTIME) {
	    params->bright = -255 + (endtime - TIME) * 255 / BTIME;
	}
    }
    ui_do_julia(rs + (re - rs) * time, is + (ie - is) * time);
}
static void mydraw()
{
    clrscr();
    dvojprujezd(starttime, zoomed, "Times");
}
static void mydraw1()
{
    clrscr();
    drawlepic("Zoomed");
}


void scene6(void)
{
    int width, height, scanline;
    int i, formula = 0;
    char *buffer1, *buffer2;
    displayed = 0;
    incalculation = 0;
    fastmode = 2;
    interruptiblemode = 0;
    autopilot = 0;
    maxstep = MAXSTEP;
    speedup = STEP;
    step = 0;
    tbreak = 0;
    maintimer = tl_create_timer();
    signal(SIGFPE, SIG_IGN);
    width = aa_imgwidth(context);
    height = aa_imgheight(context);
    scanline = width;
    buffer1 = context->imagebuffer;
    buffer2 = malloc(width * height);
    endtime = starttime + ETIME1;
    params->bright = -255;

    zcontext = make_context(width, height, scanline, 0, 1, flip_buffers, ui_waitfunc, buffer1, buffer2, get_pixelwidth(width), get_pixelheight(height));
    if (!zcontext) {
	exit(-1);
    }
    set_formula(zcontext, formula);
    for (i = 0; i < 255; i++) {
	/*zcontext->colors[i] = (i * 15) % 255 + 1; */
	zcontext->colors[i] = (i * 8) % 255 + 1;
    }
    zcontext->colors[0] = 0;
    zcontext->num_colors = 255;
    tbreak = 2;
    zcontext->maxiter = 150;
    fastmode--;
    zcontext->range = 8;
    zcontext->incoloringmode = 0;
    zcontext->coloringmode = 0;
    zcontext->plane = 0;
    ui_do_fractal(NEW_IMAGE);
    ui_autopilot();
    speedup = 1 * STEP;
    maxstep *= 5;
    step = -maxstep;
    tl_process_group(syncgroup);
    bbupdate();
    tl_reset_timer(maintimer);
    main_loop();
    starttime = endtime;
    context->imagebuffer = buffer1;
    free(buffer2);
    tl_free_timer(maintimer);
    params->bright = 0;
    params->randomval = 0;
    initlepic();
    drawptr = mydraw1;
    timestuff(-60, ctrllepic, draw, 2000000);
    drawptr = mydraw;
    /*timestuff(60, NULL, draw, 4 * 1000000); */
    /*timestuff(20, blur, draw, 4 * 1000000); */
    timestuff(60, NULL, draw, 3 * 1000000);

}
void scene7(void)
{
    int width, height, scanline;
    int i, formula = 0;
    char *buffer1;
    signal(SIGFPE, SIG_IGN);
    width = aa_imgwidth(context);
    height = aa_imgheight(context);
    scanline = width;
    buffer1 = context->imagebuffer;
    params->bright = -255;
    params->dither = AA_NONE;

    zcontext = make_context(width, height, scanline, 0, 1, flip_buffers, ui_waitfunc, buffer1, buffer1, get_pixelwidth(width), get_pixelheight(height));
    if (!zcontext) {
	exit(-1);
    }
    set_formula(zcontext, formula);
    for (i = 0; i < 255; i++) {
	zcontext->colors[i] = (i * 15) % 255 + 1;
    }
    zcontext->colors[0] = 0;
    zcontext->num_colors = 255;
    tbreak = 2;
    zcontext->maxiter = 255;
    fastmode--;
    zcontext->range = 8;
    zcontext->incoloringmode = 0;
    zcontext->coloringmode = 0;
    zcontext->plane = 0;
    starttime = endtime;
    sef = 1;
    eef = 0;
    is = -2;
    rs = -2;
    ie = 2;
    re = 2;
    timestuff(0, NULL, juliov, ETIME2);
    sef = 0;
    eef = 0;
    is = 2;
    rs = 2;
    ie = 0;
    re = -2;
    timestuff(0, NULL, juliov, ETIME2);
    sef = 0;
    eef = 0;
    is = 0;
    rs = -2;
    ie = 0;
    re = 2;
    timestuff(0, NULL, juliov, ETIME2);
    sef = 0;
    eef = 1;
    is = 0;
    rs = 2;
    ie = -1;
    re = -5.0;
    timestuff(0, NULL, juliov, ETIME2);
    context->imagebuffer = buffer1;
    params->bright = 0;
    params->dither = AA_FLOYD_S;
}
#if 0
void scene9(void)
{
    int width, height, scanline;
    int i, formula = 1;
    char *buffer1;
    signal(SIGFPE, SIG_IGN);
    width = aa_imgwidth(context);
    height = aa_imgheight(context);
    scanline = width;
    buffer1 = context->imagebuffer;
    params->bright = -255;
    params->dither = AA_NONE;

    zcontext = make_context(width, height, scanline, 0, 1, flip_buffers, ui_waitfunc, buffer1, buffer1, get_pixelwidth(width), get_pixelheight(height));
    if (!zcontext) {
	exit(-1);
    }
    set_formula(zcontext, formula);
    for (i = 0; i < 255; i++) {
	zcontext->colors[i] = (i * 15) % 255 + 1;
    }
    zcontext->colors[0] = 0;
    zcontext->num_colors = 255;
    tbreak = 2;
    zcontext->maxiter = 255;
    fastmode--;
    zcontext->range = 8;
    zcontext->incoloringmode = 0;
    zcontext->coloringmode = 0;
    zcontext->plane = 0;
    starttime = endtime;
    sef = 1;
    eef = 0;
    is = -1;
    rs = -1;
    ie = 2;
    re = -2;
    timestuff(0, NULL, juliov, ETIME2);
    sef = 0;
    eef = 0;
    is = ie, rs = re;
    ie = 2;
    re = 2;
    timestuff(0, NULL, juliov, ETIME2);
    sef = 0;
    eef = 0;
    is = ie, rs = re;
    ie = -2;
    re = 2;
    timestuff(0, NULL, juliov, ETIME2);
    sef = 0;
    eef = 1;
    is = ie, rs = re;
    ie = 2;
    re = -2;
    timestuff(0, NULL, juliov, ETIME2);
    context->imagebuffer = buffer1;
    params->bright = 0;
    params->dither = AA_FLOYD_S;
}
#endif
