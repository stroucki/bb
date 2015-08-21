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
#ifdef _plan9_
#include <u.h>
#include <libc.h>
#include <stdio.h>
#else

#ifdef __DJGPP__
#include "aconfig.dos"
#else
#ifndef _MAC
#include "aconfig.h"
#endif
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#include <math.h>
#include <string.h>
#endif
#include "config.h"
#include "complex.h"
#include "formulas.h"
#include "zoom.h"

int coloringmode;
int incoloringmode;

char *incolorname[] =
{
    "maxiter",
    "zmag",
};

char *outcolorname[] =
{
    "iter",
    "iter+real",
    "iter+imag",
    "iter+real/imag",
    "iter+real+imag+real/imag",
    "binary decomposition",
    "biomorphs",
    "biomorphs decompisition"
};

#define OUTPUT() if(iter>=MAXITER)\
		return(INT_MAX); else \
		return(iter)
#ifdef _RED_
#define OUTPUT() if(iter>=MAXITER)\
		return(incoloringmode?incolor_output(zre,zim,iter):INT_MAX); else \
		return(!coloringmode?iter:color_output(zre,zim,iter))

int INLINE
 color_output(number_t zre, number_t zim, int iter)
{
    switch (coloringmode) {
    case 1:			/* real */
	return (iter + zre);
	break;
    case 2:			/* imag */
	return (iter + zim);
	break;
    case 3:			/* real / imag */
	if (myfabs(zim) < 0.00001)
	    return 0;
	return (iter + zre / zim);
	break;
    case 4:			/* all of the above */
	if (myfabs(zim) < 0.00001)
	    return 0;
	return (iter + zre + zim + zre / zim);
	break;
    case 5:
	if (zim < 0)
	    return (iter);
	else
	    return (MAXITER - iter);
	break;
    case 6:
	if (myabs(zim) < 2.0 || myabs(zre) < 2.0)
	    return (MAXITER - iter);
	else
	    return (iter);
	break;
    default:
	break;
    }
    return iter;
}

int INLINE
 incolor_output(number_t zre, number_t zim, int iter)
{
    return ((zre * zre + zim * zim) * (number_t) (MAXITER >> 1) + 1);
}
#define lim 0.005
#endif

static int mand_calc(register number_t cre, register number_t cim, register number_t pre, register number_t pim)
{
    register number_t rp = 0, ip = 0;
    register unsigned int iter = MAXITER /*& (~(int) 3) */ ;
    register number_t zre, zim;
    register number_t r = 0, s = 0;
#ifdef _UNDEFINED_
    int whensavenew, whenincsave;
#endif
    zre = cre;
    zim = cim;
    if (!incoloringmode && pre == cre && pim == cim) {
	r = cre * cre + cim * cim;
	s = sqrt(r - cre / 2 + 1.0 / 16.0);
    }
    if (!incoloringmode && pre == cre && pim == cim && ((cre + 1) * (cre + 1) + cim * cim < 1.0 / 16.0 || ((16 * r * s) < (5 * s - 4 * cre + 1))))
	iter = 0;
    else {
	r = zre;
	s = zim;
#ifdef _UNDEFINED_
	whensavenew = 4;	/*You should adapt theese values */
	whenincsave = 10;
#endif

	while ((iter) && (rp + ip < 4)) {
	    ip = (zim * zim);
	    zim = (zim * zre) * 2 + pim;
	    rp = (zre * zre);
	    zre = rp - ip + pre;
#ifdef _UNDEFINED_
	    if ((iter % whensavenew) == 0) {
		r = zre;
		s = zim;
		whenincsave--;
		if (!whenincsave) {
		    whensavenew <<= 1;
		    whenincsave = 10;
		}
	    }
	    else {
		if ((myabs(r - zre) < lim) && (myabs(s - zim) < lim)) {
		    return (iter);
		}
	    }
#endif
	    iter--;

	}
    }
    iter = MAXITER - iter;
    OUTPUT();
}

static int mand3_calc(register number_t cre, register number_t cim, register number_t pre, register number_t pim)
{
    register number_t rp = 0, ip = 0;
    register int iter = MAXITER;
    register number_t zre, zim;

    zre = cre;
    zim = cim;
    rp = zre * zre;
    ip = zim * zim;
    while ((iter) && (rp + ip < 4)) {
	rp = zre * (rp - 3 * ip);
	zim = zim * (3 * zre * zre - ip) + pim;
	zre = rp + pre;
	rp = zre * zre;
	ip = zim * zim;
	iter--;
    }
    iter = MAXITER - iter;
    OUTPUT();
}
#ifdef _RED_

static int magnet_calc(register number_t zre, register number_t zim, register number_t pre, register number_t pim)
{
    register number_t rp = 0, ip = 0, tmp;
    register number_t br;
    register int iter = MAXITER;

    rp = zre * zre;
    ip = zim * zim;
    while ((iter) && (rp + ip < 100 * 100) /*(myabs(rp)+myabs(ip))>0.1 */ ) {
	br = zre + zre + pre - 2;
	tmp = zre * zim;
	zre = rp - ip + pre - 1;
	ip = zim + zim + pim;
	zim = tmp + tmp + pim;
	tmp = 1 / (br * br + ip * ip);
	rp = (zre * br + zim * ip) * tmp;
	ip = (zim * br - zre * ip) * tmp;
	zre = (rp + ip) * (rp - ip);
	zim = rp * ip;
	zim += zim;
	rp = zre - 1;
	ip = zim * zim;
	if (rp * rp + ip < 0.001)
	    goto skip;
	rp = (zre) * (zre);
	iter--;
    }
    if (iter) {
	zre *= 0.02;
	zim *= 0.02;
    }
  skip:;
    iter = MAXITER - iter;
    OUTPUT();
}


static int mand4_calc(register number_t cre, register number_t cim, register number_t pre, register number_t pim)
{
    register number_t rp = 0, ip = 0;
    register int iter = MAXITER;
    register number_t zre, zim;

    zre = cre;
    zim = cim;
    rp = zre * zre;
    ip = zim * zim;
    while ((iter) && (rp + ip < 4)) {
	rp = rp * rp - 6 * rp * ip + ip * ip + pre;
	zim = 4 * zre * zre * zre * zim - 4 * zre * ip * zim + pim;
	zre = rp;
	rp = zre * zre;
	ip = zim * zim;
	iter--;
    }
    iter = MAXITER - iter;
    OUTPUT();
}

static int mand5_calc(register number_t cre, register number_t cim, register number_t pre, register number_t pim)
{
    register number_t rp = 0, ip = 0, t;
    register int iter = MAXITER;
    register number_t zre, zim;

    zre = cre;
    zim = cim;
    while ((iter) && (rp + ip < 4)) {
	c_pow4(zre, zim, rp, ip);
	c_mul(zre, zim, rp, ip, t, zim);
	zre = t + pre;
	zim += pim;
	rp = zre * zre;
	ip = zim * zim;
	iter--;
    }
    iter = MAXITER - iter;
    OUTPUT();
}

static int mand6_calc(register number_t cre, register number_t cim, register number_t pre, register number_t pim)
{
    register number_t rp = 0, ip = 0, t;
    register int iter = MAXITER;
    register number_t zre, zim;

    zre = cre;
    zim = cim;
    while ((iter) && (rp + ip < 4)) {
	c_pow3(zre, zim, rp, ip);
	c_pow3(rp, ip, t, zim);
	zre = t + pre;
	zim += pim;
	rp = zre * zre;
	ip = zim * zim;
	iter--;
    }
    iter = MAXITER - iter;
    OUTPUT();
}

static int barnsley1_calc(register number_t cre, register number_t cim, register number_t pre, register number_t pim)
{
    register number_t rp, ip;
    register int iter = MAXITER;
    register number_t zre, zim;

    zre = cre;
    zim = cim;
    do {
	if (zre >= 0) {
	    rp = zre - 1;

	}
	else {
	    rp = zre + 1;
	}
	c_mul(rp, zim, pre, pim, zre, zim);
	rp = zre * zre;
	ip = zim * zim;
	iter--;
    }
    while ((iter) && (rp + ip < 4));
    iter = MAXITER - iter;
    OUTPUT();
}

static int newton_calc(register number_t pre, register number_t pim, register number_t zre, register number_t zim)
{
    register number_t n, sqrr, sqri, zre1, zim1;
    register int iter = MAXITER;

    sqri = zim * zim;
    do {
	zre1 = zre;
	zim1 = zim;
	n = zim * zim;
	sqri = zre * zre;
	sqrr = sqri - n;
	sqri = n + sqri;
	n = 0.3333333333 / ((sqri * sqri));
	zim = (0.66666666) * zim - (zre + zre) * zim * n + pim;
	zre = (0.66666666) * zre + (sqrr) * n + pre;
	zre1 -= zre;
	zim1 -= zim;
	n = zre1 * zre1 + zim1 * zim1;
	iter--;
    }
    while (iter && n > 1E-6);
    iter = MAXITER - iter;
    OUTPUT();
}


static int phoenix_calc(register number_t cre, register number_t cim, register number_t pre, register number_t pim)
{
    register int iter = MAXITER;
    register number_t zre, zim, zpr, zpm, rp = 0.0, ip = 0.0;

    zre = cre;
    zim = cim;
    zpr = zre * zre, zpm = zim * zim;
    while (iter && (zpr + zpm < 4)) {
	zpr = zpr - zpm + pre + pim * rp;
	zpm = 2 * zre * zim + pim * ip;
	rp = zre, ip = zim;
	zre = zpr;
	zim = zpm;
	zpr = zre * zre, zpm = zim * zim;
	iter--;
    }
    iter = MAXITER - iter;
    OUTPUT();
}

static int octo_calc(register number_t pre, register number_t pim, register number_t cre, register number_t cim)
{
    register number_t rp, ip, tr, ti;
    register int iter = MAXITER;
    register number_t zre = cre, zim = cim, zpr = 0, zpm = 0;

    while (iter && ((zpr * zpr + zpm * zpm) < 4)) {
	rp = zre;
	ip = zim;
	c_pow3(zre, zim, tr, ti);
	c_add(tr, ti, zpr, zpm, zre, zim);
	zpr = rp + pre;
	zpm = ip + pim;
	iter--;
    }
    iter = MAXITER - iter;
    OUTPUT();
}
#endif

symetry sym6[] =
{
    {0, 1.73205080758},
    {0, -1.73205080758}
};

symetry sym8[] =
{
    {0, 1},
    {0, -1}
};

symetry sym16[] =
{
    {0, 1},
    {0, -1},
    {0, 0.414214},
    {0, -0.414214},
    {0, 2.414214},
    {0, -2.414214}
};

struct formula formulas[] =
{
    {
	FORMULAMAGIC,
	mand_calc,
	{"Mandelbrot", "Julia"},
	{0.5, -2.0, 1.25, -1.25},
	1, 0.0, 0.0,
	{
	    {INT_MAX, 0, 0, NULL},
	    {INT_MAX, 0, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, 0, 0, NULL}
	},
	{
	    {INT_MAX, 0, 0, NULL},
	    {INT_MAX, 0, 0, NULL}
	},
	0, 0,
    },
    {
	FORMULAMAGIC,
	mand3_calc,
	{"Mandelbrot^3", "Julia^3"},
	{1.25, -1.25, 1.25, -1.25},
	1, 0.0, 0.0,
	{
	    {0, 0, 0, NULL},
	    {INT_MAX, 0, 0, NULL},
	    {0, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {0, INT_MAX, 0, NULL},
	    {0, 0, 0, NULL}
	},
	{
	    {0, 0, 0, NULL},
	    {0, 0, 0, NULL}
	},
	1, 0,
    },
#ifdef _RED_
    {
	FORMULAMAGIC,
	mand4_calc,
	{"Mandelbrot^4", "Julia^4"},
	{1.25, -1.25, 1.25, -1.25},
	1, 0.0, 0.0,
	{
	    {INT_MAX, 0, 2, sym6},
	    {INT_MAX, 0, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, 0, 0, NULL}
	},
	{
	    {INT_MAX, 0, 2, sym6},
	    {INT_MAX, 0, 2, sym6}
	},
	2, 0,
    },
    {
	FORMULAMAGIC,
	mand5_calc,
	{"Mandelbrot^5", "Julia^5"},
	{1.25, -1.25, 1.25, -1.25},
	1, 0.0, 0.0,
	{
	    {0, 0, 2, sym8},
	    {INT_MAX, 0, 0, NULL},
	    {0, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {0, INT_MAX, 0, NULL},
	    {0, 0, 2, sym8},
	},
	{
	    {0, 0, 2, sym8},
	    {0, 0, 2, sym8}
	},
	3, 0,
    },
    {
	FORMULAMAGIC,
	mand6_calc,
	{"Mandelbrot^6", "Julia^6"},
	{1.25, -1.25, 1.25, -1.25},
	1, 0.0, 0.0,
	{
	    {0, 0, 6, sym16},
	    {INT_MAX, 0, 0, NULL},
	    {0, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {0, INT_MAX, 0, NULL},
	    {0, 0, 0, NULL},
	},
	{
	    {0, 0, 6, sym16},
	    {0, 0, 6, sym16}
	},
	4, 0,
    },
    {
	FORMULAMAGIC,
	octo_calc,
	{"Octal", "Octal"},
	{1.25, -1.25, 1.25, -1.25},
	1, 0.0, 0.0,
	{
	    {0, 0, 6, sym16},
	    {INT_MAX, 0, 0, NULL},
	    {0, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {0, INT_MAX, 0, NULL},
	    {0, 0, 0, NULL},
	},
	{
	    {0, 0, 6, sym16},
	    {0, 0, 6, sym16}
	},
	-1, 1,
    },
    {
	FORMULAMAGIC,
	newton_calc,
	{"Newton", "Newton julia?"},
	{1.25, -1.25, 1.25, -1.25},
	1, 0.0, 0.0,
	{
	    {INT_MAX, 0, 2, sym6},
	    {INT_MAX, 0, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, 0, 2, sym6}
	},
	{
	    {INT_MAX, 0, 2, sym6},
	    {INT_MAX, 0, 2, sym6}
	},
	-1, 1,
    },
    {
	FORMULAMAGIC,
	barnsley1_calc,
	{"Barnsley1 Mandelbrot", "Barnsley1"},
	{1.25, -1.25, 1.25, -1.25},
	0, -0.6, 1.1,
	{
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL}
	},
	{
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL}
	},
	5, 0,
    },
    {
	FORMULAMAGIC,
	phoenix_calc,
	{"MandPhoenix", "Phoenix"},
	{1.25, -1.25, 1.25, -1.25},
	0, 0.56667000000000001, -0.5,
	{
	    {INT_MAX, 0, 0, NULL},
	    {INT_MAX, 0, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, 0, 0, NULL},
	},
	{
	    {INT_MAX, 0, 0, NULL},
	    {INT_MAX, 0, 0, NULL}
	},
	-1, 0,
    },
#endif
#if 0
    {
	FORMULAMAGIC,
	/*magnet_calc */ NULL,
	{"Magnet", "Magnet"},
	{3, 0, 2.2, -2.2},
	1, 0.0, 0.0,
	{
	    {INT_MAX, 0, 0, NULL},
	    {INT_MAX, 0, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, INT_MAX, 0, NULL},
	    {INT_MAX, 0, 0, NULL}
	},
	{
	    {INT_MAX, 0, 0, NULL},
	    {INT_MAX, 0, 0, NULL}
	},
	6, 1,
    },
#endif
};

struct formula *currentformula = formulas;
CONST int nformulas = sizeof(formulas) / sizeof(struct formula);
