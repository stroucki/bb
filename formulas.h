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
#ifndef FORMULAS_H
#define FORMULAS_H

#include "config.h"

#define INCOLORING 2
#define OUTCOLORING 7
#define MAX_LAMBDA 2

typedef struct {
    number_t mc, nc;
    number_t mi, ni;
} vinfo;

typedef struct {
    number_t y0, k;
} symetry;

struct symetryinfo {
    number_t xsym, ysym;
    int nsymetries;
    symetry *symetry;
};

#define FORMULAMAGIC 1121
struct formula {
    int magic;
    int (*calculate) (number_t, number_t, number_t, number_t) REGISTERS(3);
    char *name[2];
    vinfo v;
    int mandelbrot;
    number_t pre, pim;
    struct symetryinfo out[OUTCOLORING];
    struct symetryinfo in[INCOLORING];
    int juliamode;
    int startzero;
};

extern struct formula formulas[];
extern char *incolorname[];
extern char *outcolorname[];
extern CONST int nformulas;
extern int coloringmode;
extern int incoloringmode;

#endif				/* FORMULAS_H */
