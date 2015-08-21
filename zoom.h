/* This file was taken from XaoS-the fast portable realtime interactive 
   fractal zoomer. but it is simplified for BB. You may get complette
   sources at XaoS homepage (http://www.paru.cas.cz/~hubicka/XaoS
 */
/* 
 *     XaoS, a fast portable realtime fractal zoomer 
 *                  Copyright (C) 1996,1997 by
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
#ifndef ZOOM_H
#define ZOOM_H

#include "config.h"
#include "formulas.h"
#define CONTEXTMAGIC 1122
#define checkcontext(c) assert((c)->magic==CONTEXTMAGIC)
struct realloc_s {
    number_t possition;
    number_t price;
    int plus;
    int recalculate;
    int symto;
    int symref;
    int dirty;
#ifdef __GNUC__
#ifdef __STDC__
/*__STRICT_ANSI__*/
    char dummy[((32 - sizeof(int) * 5 - ((int) (sizeof(number_t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int)) < 0 ? 0 :
		 (32 - sizeof(int) * 5 - ((int) (sizeof(number_t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int)))];	/*make it 32 byte aligned */
#endif
#endif
};
typedef struct realloc_s realloc_t;
typedef struct {
    number_t pre, pim;
    number_t *xpos, *ypos;
    void (*switch_function) (void);
    void (*wait_function) (void);
    int magic;
    unsigned char *vbuff, *back;
    unsigned char *svbuff, *sback;
    realloc_t *reallocx, *reallocy;
    struct formula *currentformula;
    int pos, max;
    char *pass;
    int stereogram;
    int maxiter, coloringmode, incoloringmode;
    int mandelbrot;
    int fullscreen;
    int plane;
    int dirty;
    int range;
    int incalculation;
    int readyforinterrupt;
    int uncomplette;
    int num_colors;
    int width, height;
    int scanline;
    int interrupt;
    unsigned char colors[MAXCOLORS];
    unsigned char cmap[3][MAXCOLORS];
    float pixelwidth, pixelheight;
    float windowwidth, windowheight;
    vinfo s;
} zoom_context;

void do_fractal(zoom_context *, int);
void do_julia(zoom_context *, number_t, number_t);
void set_formula(zoom_context *, CONST int);
int resize_to(zoom_context *, CONST int, CONST int, CONST int, char *, char *, double, double);
void set_view(zoom_context *, CONST vinfo *);
void free_context(zoom_context *);
void enable_stereogram(zoom_context *);
void disable_stereogram(zoom_context *);
void init_tables(zoom_context *);
void rotate_palette(zoom_context *, int direction);
zoom_context *make_context(CONST int, CONST int, CONST int, CONST int, CONST int, void (*)(void), void (*)(void), char *, char *, double, double);
#endif				/* ZOOM_H */
