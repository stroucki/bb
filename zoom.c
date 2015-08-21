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
/*#define STATISTICS */
#ifdef _plan9_
#include <u.h>
#include <libc.h>
#include <stdio.h>
#else
#include <stdlib.h>
#include <stdio.h>
#ifndef _MAC
#include <malloc.h>
#endif
#ifdef __DJGPP__
#include "aconfig.dos"
#else
#include "aconfig.h"
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#include <math.h>
#include <string.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include "config.h"
#ifndef _plan9_
/*#undef NDEBUG */
#include <assert.h>
#endif
#endif
#include "zoom.h"
#include "complex.h"
#include "formulas.h"
#ifdef _RED_
#include "plane.h"
#endif
#ifdef __GNUC__
#ifdef __i386__
#include "ctrl87.h"
#endif
#endif

#define callwait() if(d->wait_function!=NULL) d->wait_function();

int MAXITER = DEFAULT_MAX_ITER;

typedef struct {
    double y0, k, kk, y0k;
} symetry2;

struct symetryinfo2 {
    number_t xsym, ysym;
    int nsymetries;
    symetry2 *symetry;
};

static symetry2 sym_lines[100];
static struct symetryinfo2 cursymetry;
static number_t xmul, ymul, xdist, ydist;

static zoom_context *d;
static int INLINE calculate(number_t, number_t) CONSTF;
#ifdef _UNDEFINED_
static int INLINE symetries(number_t, number_t) CONSTF;
#endif
#ifdef STATISTICS
int mirrored = 0;
int nadded2 = 0, nsymetry2 = 0, nskipped2 = 0, mirrored2 = 0;
int tocalculate2 = 0, avoided2 = 0, frames2 = 0;
int ncalculated2 = 0, ninside2 = 0;
int niter2 = 0, niter1 = 0;
#endif

#ifdef _UNDEFINED_
static int INLINE symetries(number_t x, number_t y)
{
    int i;

    for (i = 0; i < cursymetry.nsymetries; i++) {
	double b = cursymetry.symetry[i].k, c = x + b * y, x1, y1, dist;
	int x2, y2;
	x1 = (c - cursymetry.symetry[i].y0k) / (cursymetry.symetry[i].kk);
	y1 = (c - x1) / b;
	x1 = x - 2 * (x - x1);
	y1 = y - 2 * (y - y1);
	if (x1 < d->s.nc || y1 < d->s.ni || x1 >= d->s.mc || y1 >= d->s.mi)
	    continue;

	x2 = (x1 - d->s.nc) * xmul;
	y2 = (y1 - d->s.ni) * ymul;

	dist = myfabs(d->reallocx[x2].possition - x1);
	if (x2 > 0 && myfabs(d->reallocx[x2 - 1].possition - x1) < dist) {
	    x2--;
	    dist = myfabs(d->reallocx[x2].possition - x1);
	} else if (x2 < d->width - 1 && myfabs(d->reallocx[x2 + 1].possition - x1) < dist) {
	    x2++;
	    dist = myfabs(d->reallocx[x2].possition - x1);
	}
	if (dist > xdist)
	    continue;
	dist = myfabs(d->reallocy[y2].possition - y1);
	if (y2 > 0 && myfabs(d->reallocy[y2 - 1].possition - y1) < dist) {
	    y2--;
	    dist = myfabs(d->reallocy[y2].possition - y1);
	} else if (y2 < d->height - 1 && myfabs(d->reallocy[y2 + 1].possition - y1) < dist) {
	    y2++;
	    dist = myfabs(d->reallocy[y2].possition - y1);
	}
	if (dist > ydist)
	    continue;
	if (d->reallocx[x2].symto != -1)
	    x2 = d->reallocx[x2].symto;
	if (d->reallocy[y2].symto != -1)
	    y2 = d->reallocy[y2].symto;
	if (d->reallocx[x2].dirty ||
	    d->reallocy[y2].dirty /* == 1 */ )
	    continue;
	STAT(mirrored2++);
	return ((unsigned char) *(d->vbuff + x2 + y2 * d->scanline));
    }
    return (-1);
}
#endif

static int INLINE calculate(number_t x, number_t y)
{
    int i;

#ifdef _UNDEFINED_
    i = symetries(x, y);
    if (i != -1)
	return (i);
#endif
#ifdef _RED_
    i = d->plane;
    if (i) {
	recalculate(i, &x, &y);
    }
#endif
    STAT(ncalculated2++);
    if (d->mandelbrot)
	if (d->currentformula->startzero)
	    i = d->currentformula->calculate(0.0, 0.0, x, y);
	else
	    i = d->currentformula->calculate(x, y, x, y);
    else
	i = d->currentformula->calculate(x, y, d->pre, d->pim);
    if (i == INT_MAX) {
	STAT(ninside2++);
	STAT(niter2 += MAXITER);
	return d->colors[0];
    }
    STAT(niter1 += i);
    if (i < 0)
	i += (-i / (d->num_colors - 1) + 1) * (d->num_colors - 1);

    return (d->colors[1 + i % (d->num_colors - 1)]);
}
struct dyn_data {
    int price;
    int previous;
};

#define FPMUL 512
#define RANGES 2		/*shift equal to x*RANGE */
#define RANGE 4
#define DSIZEHMASK (0x7)	/*mask equal to x%(DSIZE) */

#define DSIZE (2*RANGE)
#define DSIZES (RANGES+1)	/*shift equal to x*DSIZE */

#define getdata(n,i) ((((n)<<DSIZES)+(((i)&(DSIZEHMASK)))))
#define adddata(n,i) ((((n)<<DSIZES)+(((i)&(DSIZEHMASK)))))
#define getbest(i) (((size)<<DSIZES)+(i))

#define PRICE(i,i1) ((((ftmp=((i)-(i1)))*ftmp)*(rangedelta)))
#define NEWPRICE (FPMUL)

#define NOSETMASK ((unsigned int)0x80000000)
#define END 0x40000000
#define MAXPRICE INT_MAX
CONST int dynsize = sizeof(struct dyn_data);
#ifndef INT_MIN
#define INT_MIN (- INT_MAX - 1)
#endif

static /*INLINE */ void mkrealloc_table(register number_t * pos, realloc_t * realloc, CONST int size, CONST number_t begin, CONST number_t end, number_t sym)
{
    int i, ps, ps1 = 0, pe;
    register int p;
    register int bestprice;
    realloc_t *r = realloc, *reallocs;
    register struct dyn_data *dyndata, *dynptr;
    int bestdata = 0;
    int *best, *best1, *tmp;
    number_t step = (end - begin) / (number_t) size;
    number_t y, range = RANGE * step, rangedelta, yend, ftmp;
    int symi = -1;
    int lastplus = 0;
    int data;
    int previous = 0;
    int myprice;

#ifdef STATISTICS
    int nadded = 0, nsymetry = 0, nskipped = 0;
#endif
#ifdef HAVE_ALLOCA
    dyndata = alloca((size) * (DSIZE + 1) * sizeof(struct dyn_data));
    best = alloca(size * sizeof(int));
    best1 = alloca(size * sizeof(int));
#else
    dyndata = malloc((size) * (DSIZE + 1) * sizeof(struct dyn_data));
    best = malloc(size * sizeof(int));
    best1 = malloc(size * sizeof(int));
#endif
    if (dyndata == NULL) {
	fprintf(stderr, "XaoS fatal error:Could not allocate memory for"
		"temporary dynamical data of size:%i\n"
		"I am unable to handle this problem so please resize to lower window\n", (size) * (DSIZE + 1) * sizeof(struct dyn_data) + size * sizeof(int) + size * sizeof(int));
	return;
    }
    if (best == NULL) {
	fprintf(stderr, "XaoS fatal error:Could not allocate memory for"
		"temporary dynamical data of size:%i\n"
		"I am unable to handle this problem so please resize to lower window\n", (size) * (DSIZE + 1) * sizeof(struct dyn_data) + size * sizeof(int) + size * sizeof(int));
#ifndef HAVE_ALLOCA
	free(dyndata);
#endif
	return;
    }
    if (best1 == NULL) {
	fprintf(stderr, "XaoS fatal error:Could not allocate memory for"
		"temporary dynamical data of size:%i\n"
		"I am unable to handle this problem so please resize to lower window\n", (size) * (DSIZE + 1) * sizeof(struct dyn_data) + size * sizeof(int) + size * sizeof(int));
#ifndef HAVE_ALLOCA
	free(dyndata);
	free(best);
#endif
	return;
    }
/* get prices */

    if (begin > sym)
	symi = -2;
    ps = 0;
    pe = 0;
    pos[size] = end + 20 * step;
    rangedelta = FPMUL / (range * range);
    for (i = 0; i < size; i++) {
	tmp = best1;
	bestprice = MAXPRICE;
	y = begin + i * step;
	p = ps;			/*just inicialize parameters */
	best1 = best;
	if (y > sym && symi == -1)
	    symi = i - 1;	/*find symetry point */
	yend = y - range + step;
	best = tmp;
	if (yend < begin - step)	/*do no allow lines outside screen */
	    yend = begin - step;
	while (pos[p] < yend && p < size)	/*skip lines out of range */
	    p++;
	if (p != 0 && pos[p - 1] > y - range)
	    p--;
	ps1 = p;
	yend = y + range;
	if (ps != pe && p > ps) {	/*previous point had lines */
	    assert(p >= ps);
	    if (p < pe) {
		previous = best[p - 1];
	    } else
		previous = best[pe - 1];
	    myprice = dyndata[previous].price;	/*find best one */
	} else {
	    if (i > 0) {	/*previous line had no lines */
		previous = getbest(i - 1);
		myprice = dyndata[previous].price;
	    } else
		previous = END, myprice = 0;
	}
	data = getbest(i);	/*find store possition */
	myprice += NEWPRICE;
	bestdata = data;
	dyndata[data].previous = previous;
	bestprice = myprice;	/*calculate best available price */
	dyndata[data].price = myprice;	/*store data */
	assert(bestprice >= 0);
	if (yend > end + step)	/*check bounds */
	    yend = end + step;
	data = adddata(p, i);	/*calcualte all lines good for this y */
	dynptr = &dyndata[data];
	if (ps != pe) {		/*in case that previous had also possitions */
	    int price1 = INT_MAX;
	    if (p == ps) {	/*here we must skip previous point */
		if (pos[p] != pos[p + 1]) {
		    previous = getbest(i - 1);
		    myprice = dyndata[previous].price;
		    myprice += PRICE(pos[p], y);	/*store data */
		    if (myprice < bestprice) {	/*calcualte best */
			bestprice = myprice, bestdata = data;
			dynptr->price = myprice;
			dynptr->previous = previous;
		    }
		}
		assert(bestprice >= 0);
		assert(myprice >= 0);
		best1[p] = bestdata;
		data += DSIZE;
		p++;
		dynptr += DSIZE;
	    }
	    previous = -1;
	    while (p < pe) {	/*this is area where intervals of current point and previous one are crossed */
		if (pos[p] != pos[p + 1]) {
		    if (previous != best[p - 1]) {
			previous = best[p - 1];
			price1 = myprice = dyndata[previous].price;
			if (myprice + NEWPRICE < bestprice) {
			    myprice += NEWPRICE;
			    bestprice = myprice, bestdata = data - DSIZE;
			    (dynptr - DSIZE)->price = myprice;
			    (dynptr - DSIZE)->previous = previous | NOSETMASK;
			    best1[p - 1] = bestdata;
			    myprice -= NEWPRICE;
			}
		    } else
			myprice = price1;
		    myprice += PRICE(pos[p], y);	/*store data */
		} else
		    myprice += NEWPRICE;
		if (myprice < bestprice) {	/*calcualte best */
		    bestprice = myprice, bestdata = data;
		    dynptr->price = myprice;
		    dynptr->previous = previous;
		}
		assert(bestprice >= 0);
		assert(myprice >= 0);
		best1[p] = bestdata;
		data += DSIZE;
		p++;
		dynptr += DSIZE;
	    }
	    if (p > ps) {
		assert(p >= ps);	/*find best one in previous */
		previous = best[p - 1];
		price1 = dyndata[previous].price;
	    } else {
		previous = getbest(i - 1);
		price1 = dyndata[previous].price;
	    }
	    if (price1 + NEWPRICE < bestprice && p > ps1) {
		myprice = price1 + NEWPRICE;
		bestprice = myprice, bestdata = data - DSIZE;
		(dynptr - DSIZE)->price = myprice;
		(dynptr - DSIZE)->previous = previous | NOSETMASK;
		best1[p - 1] = bestdata;
		myprice -= NEWPRICE;
	    }
	    while (pos[p] < yend) {
		if (pos[p] != pos[p + 1]) {
		    myprice = price1;
		    myprice += PRICE(pos[p], y);	/*store data */
		    if (myprice < bestprice) {	/*calcualte best */
			bestprice = myprice, bestdata = data;
			dynptr->price = myprice;
			dynptr->previous = previous;
		    } else if (pos[p] > y)
			break;
		}
		assert(bestprice >= 0);
		assert(myprice >= 0);
		best1[p] = bestdata;
		data += DSIZE;
		p++;
		dynptr += DSIZE;
	    }
	    while (pos[p] < yend) {
		best1[p] = bestdata;
		p++;
	    }
	} else {
	    int myprice1;	/*simplified loop for case that previous
				   y had no lines */
	    if (pos[p] < yend) {
		if (i > 0) {
		    previous = getbest(i - 1);
		    myprice1 = dyndata[previous].price;
		} else
		    previous = END, myprice1 = 0;
		while (pos[p] < yend) {
		    if (pos[p] != pos[p + 1]) {
			myprice = myprice1 + PRICE(pos[p], y);
			if (myprice < bestprice) {
			    dynptr->price = myprice;
			    dynptr->previous = previous;
			    bestprice = myprice, bestdata = data;
			} else if (pos[p] > y)
			    break;
		    }
		    assert(bestprice >= 0);
		    assert(myprice >= 0);
		    best1[p] = bestdata;
		    p++;
		    data += DSIZE;
		    dynptr += DSIZE;
		}
		while (pos[p] < yend) {
		    best1[p] = bestdata;
		    p++;
		}
	    }
	}
	previous = ps;		/*store possitions for next loop */
	ps = ps1;
	ps1 = pe;
	pe = p;
    }
    assert(bestprice >= 0);
    realloc += size;
    previous = (begin > pos[0]) && (end < pos[size - 1]);
    if ((begin < pos[0]) && (end > pos[size - 1]))
	previous = 2;
    for (i = size; i > 0;) {	/*and finally traces the path */
	int bestdata1;
	realloc--;
	i--;
	realloc->symto = -1;
	realloc->symref = -1;
	bestdata1 = dyndata[bestdata].previous;
	if ((bestdata1 & NOSETMASK) || bestdata >= ((size) << DSIZES)) {
	    /*realloc->possition=y; */
	    realloc->possition = begin + i * step;

	    realloc->recalculate = 1;
	    STAT(nadded++);
	    realloc->dirty = 1;
	    lastplus++;
	    if (lastplus >= size)
		lastplus = 0;
	    realloc->plus = lastplus;
	    if (previous == 1)
		realloc->price = 1 / (1 + myabs(pos[i] - realloc->possition) / step);
	    else if (previous == 2) {
		realloc->price = (myabs(pos[i] - realloc->possition) / step);
		if (i == 0 || i == size - 1)
		    realloc->price *= 500;
	    } else
		realloc->price = 1;
	} else {
	    p = bestdata >> DSIZES;
	    assert(p >= 0 && p < size);
	    realloc->possition = pos[p];
	    realloc->plus = p;
	    realloc->dirty = 0;
	    realloc->recalculate = 0;
	    lastplus = p;
	}
	bestdata = bestdata1 & (~NOSETMASK);
    }
#ifndef HAVE_ALLOCA
    free(dyndata);
    free(best);
    free(best1);
#endif
    realloc = r;
    sym *= 2;
    if (symi < size && symi >= 0) {
	i = 2 * symi - size;
	if (i < 0)
	    i = 0;
	realloc += i;
	for (; i <= symi; i++, realloc++) {	/*makes symetries */
	    int j, min = 0;
	    number_t dist = INT_MAX, tmp1;
	    if (realloc->symto != -1)
		continue;
	    y = realloc->possition;
	    realloc->symto = 2 * symi - i;
	    if (realloc->symto >= size - RANGE)
		realloc->symto = size - RANGE - 1;
	    dist = range;
	    min = RANGE;
	    reallocs = &r[realloc->symto];
	    for (j = realloc->symto > RANGE ? -RANGE : -realloc->symto; j < RANGE && realloc->symto + j < size - 1; j++) {
		ftmp = sym - (reallocs + j)->possition;
		if ((tmp1 = myfabs((double)(ftmp - y))) < dist) {
		    if ((realloc == r || ftmp > (realloc - 1)->possition) &&
			(ftmp < (realloc + 1)->possition)) {
			dist = tmp1;
			min = j;
		    }
		}
	    }
	    realloc->symto += min;
	    if (min == RANGE || realloc->symto <= symi ||
		(reallocs = reallocs + min)->symto != -1 || reallocs->symref != -1) {
		realloc->symto = -1;
		continue;
	    }
	    if (myfabs(y - (sym - reallocs->possition)) < range) {
		if (!realloc->recalculate) {
		    realloc->symto = -1;
		    if (reallocs->symto != -1 || !reallocs->recalculate)
			continue;
		    reallocs->plus = realloc->plus;
		    reallocs->symto = i;
		    reallocs->dirty = 1;
		    realloc->symref = reallocs - r;
		    STAT(nadded -= reallocs->recalculate);
		    reallocs->recalculate = 0;
		    reallocs->possition = sym - realloc->possition;
		} else {
		    if (reallocs->symto != -1) {
			realloc->symto = -1;
			continue;
		    }
		    STAT(nadded -= realloc->recalculate);
		    realloc->dirty = 1;
		    realloc->plus = reallocs->plus;
		    realloc->recalculate = 0;
		    reallocs->symref = i;
		    realloc->possition = sym - reallocs->possition;
		}
		STAT(nsymetry++);
	    } else
		realloc->symto = -1;
	}
    }
    realloc = r + 1;
    for (i = 1; i < size - 1; i++, realloc++) {		/*optimizes new possitions to look better */
	if (realloc->recalculate) {
	    if ((!(realloc - 1)->recalculate || !(realloc + 1)->recalculate))
		realloc->possition = ((realloc - 1)->possition + (realloc + 1)->possition) / 2;
	    i++;
	    realloc++;
	}
    }
    STAT(printf("%i added %i skipped %i mirrored\n", nadded, nskipped, nsymetry));
    STAT(nadded2 += nadded;
	 nskipped2 += nskipped;
	 nsymetry2 += nsymetry);
}


static /*INLINE */ void moveoldpoints(void)
{
    unsigned char *vline, *vbuff = d->vbuff;
    int *size, *siz, *sizend;
    int *start, *startptr;
    realloc_t *ry, *rend;
    realloc_t *rx, *rend1;
    int plus1 = 0, plus2 = 0;

#ifdef HAVE_ALLOCA
    size = (int *) alloca(d->width * sizeof(int));
    start = (int *) alloca(d->width * sizeof(int));
#else
    size = (int *) malloc(d->width * sizeof(int));
    start = (int *) malloc(d->width * sizeof(int));
#endif
    if (size == NULL) {
	fprintf(stderr, "XaoS fratal error:Could not allocate memory for"
		"temporary dynamical data of size:%i\n"
		"I am unable to handle this problem so please resize to lower window\n", 2 * d->width * sizeof(int));
	return;
    }
    if (start == NULL) {
	fprintf(stderr, "XaoS fratal error:Could not allocate memory for"
		"temporary dynamical data of size:%i\n"
		"I am unable to handle this problem so please resize to lower window\n", 2 * d->width * sizeof(int));
#ifndef HAVE_ALLOCA
	free(size);
#endif
	return;
    }
    assert(size != NULL);
    assert(start != NULL);

    startptr = start;
    siz = size;
    for (rx = d->reallocx, rend1 = d->reallocx + d->width;
	 rx < rend1; rx++)
	if ((rx->recalculate) && plus1 < d->width + 1)
	    plus1++;
	else
	    break;
    *startptr = d->reallocx->plus;
    *siz = 0;
    for (; rx < rend1; rx++) {
	if ((rx->recalculate || rx->plus == *startptr + *siz) && *startptr + *siz < d->width)
	    (*siz)++;
	else {
	    siz++, startptr++;
	    if (rx->recalculate) {
		*startptr = 0;
		ry = rx;
		for (; rx < rend1; rx++)
		    if (rx->recalculate)
			(*startptr)--;
		    else
			break;
		if (rx < rend1)
		    *startptr += rx->plus, rx = ry;
		else {
		    plus2 = -(*startptr);
		    startptr--;
		    siz--;
		    break;
		}
	    }
	    *startptr = rx->plus;
	    assert(rx->plus >= 0 && rx->plus <= d->width);
	    *siz = 1;
	}

    }
    if (*siz)
	sizend = siz + 1;
    else
	sizend = siz;
    plus2 += d->scanline - d->width;
    for (ry = d->reallocy, rend = ry + d->height; ry < rend; ry++) {
	vline = d->back + ry->plus * d->scanline;
	if (!ry->recalculate) {
	    vbuff += plus1;
	    for (startptr = start, siz = size; siz < sizend; siz++, startptr++) {
		assert(vbuff >= d->vbuff);
		assert(vbuff + *siz <= d->vbuff + d->scanline * d->height);
		assert(vline + *startptr >= d->back);
		assert(vline + *startptr + *siz <= d->back + d->scanline * d->height);
		memcpy(vbuff, vline + *startptr, *siz), vbuff += *siz;
	    }
	    vbuff += plus2;
	} else
	    vbuff += d->scanline;
    }
#ifndef HAVE_ALLOCA
    free((void *) size);
    free((void *) start);
#endif
}

/*#define DRAW */
#ifdef STATISTICS
static int tocalculate = 0, avoided = 0;
#endif
static INLINE void calcline(unsigned char *vbuff1, realloc_t * ry)
{
    number_t y;
    int range = d->range;
    realloc_t *rx, *rend, *rend1, *ryl, *ryr;
    int distl, distr, distup, distdown;
    unsigned char *vbuff;
    int s2, c;
#ifdef DRAW
    int s3;
#endif
    y = ry->possition;
    rend = ry - range - 1;
    if (d->reallocy > rend)
	rend = d->reallocy;
    distl = 1;
    for (ryl = ry - 1; rend <= ryl && ryl->dirty; ryl--, distl++);
    rend = ry + range;
    distr = 1;
    if (d->reallocy + d->height < rend)
	rend = d->reallocy + d->height;
    for (ryr = ry + 1; rend > ryr && ryr->dirty; ryr++, distr++);
    distl *= d->scanline;
    distr *= d->scanline;
    rend = d->reallocy + d->height;
    if (ryr == d->reallocy + d->height || ryl < d->reallocy || ryr->dirty || ryl->dirty) {
	for (rx = d->reallocx, vbuff = vbuff1, rend1 = d->reallocx + d->width;
	     rx < rend1; rx += 1) {
	    if (!rx->dirty) {
		STAT(tocalculate++);
#ifdef DRAW
		s3 = mirrored;
#endif
		*vbuff = calculate(rx->possition, y);
#ifdef DRAW
		if (s3 == mirrored)
		    vga_drawpixel(rx - d->reallocx, ry - d->reallocy, 254);
#endif
	    }
	    vbuff += 1;
	}
    } else
	for (s2 = 0; s2 < 2; s2++) {
	    distup = range + 2;
	    distdown = 0;
	    for (rx = d->reallocx + s2, vbuff = vbuff1 + s2, rend1 = d->reallocx + d->width;
		 rx < rend1; rx += 2) {
		assert(vbuff >= d->vbuff);
		assert(vbuff < d->vbuff + d->scanline * d->height);
		assert(rx >= d->reallocx && rx <= d->reallocx + d->width);
		assert(ry >= d->reallocy && ry <= d->reallocy + d->height);
		if (!rx->dirty) {
		    STAT(tocalculate++);
		    if (distdown <= 0) {
			distdown = 1;
			for (ryr = rx + 1; ryr < rend1 && ryr->dirty; ryr++, distdown++);
			if (ryr == rend1)
			    distdown = INT_MAX;
		    }
		    if (distdown < range &&
			distup < range &&
			(vbuff[-distl] == vbuff[distr] &&
			 ((
			vbuff[-distl + distdown] == (c = vbuff[distr]) &&
			      vbuff[-distl - distup] == c &&
			      c == vbuff[distr - distup] &&
			      c == vbuff[distr + distdown]) &&
			  (!s2 ||
			   (
			       ((rx - 1)->dirty ||
				vbuff[-1] == c) &&
			       ((rx + 1)->dirty ||
				vbuff[+1] == c)
			   ))))) {
			*vbuff = vbuff[distr];
			STAT(avoided++);
		    } else {
#ifdef DRAW
			s3 = mirrored;
#endif
			*vbuff = calculate(rx->possition, y);
#ifdef DRAW
			if (s3 == mirrored)
			    vga_drawpixel(rx - d->reallocx, ry - d->reallocy, 255);
#endif
		    }
		    distup = 0;
		}
		vbuff += 2;
		if (!(rx + 1)->dirty)
		    distup = -1;
		distdown -= 2;
		distup += 2;
	    }
	}
    ry->recalculate = 2;
    ry->dirty = 0;
}
static INLINE void calccolumn(unsigned char *vbuff1, realloc_t * rx)
{
    number_t x;
    int range = d->range;
    realloc_t *ry, *rend, *rend1, *rxl, *rxr;
    int distl, distr, distup, distdown;
    int c;
    unsigned char *vbuff;
    int s2;
#ifdef DRAW
    int s3;
#endif
    rend = rx - range + 1;
    if (d->reallocx > rend)
	rend = d->reallocx;
    distl = 1;
    for (rxl = rx - 1; rend <= rxl && rxl->dirty; rxl--, distl++);
    rend = rx + range;
    distr = 1;
    if (d->reallocx + d->width < rend)
	rend = d->reallocx + d->width;
    for (rxr = rx + 1; rend > rxr && rxr->dirty; rxr++, distr++);
    x = rx->possition;
    rend = d->reallocx + d->width;
    if (rxr >= d->reallocx + d->width || rxl < d->reallocx || rxr->dirty || rxl->dirty) {
	for (ry = d->reallocy, vbuff = vbuff1, rend1 = d->reallocy + d->height; ry < rend1; ry++) {
	    if (!ry->dirty) {
		STAT(tocalculate++);
#ifdef DRAW
		s3 = mirrored;
#endif
		*vbuff = calculate(x, ry->possition);
#ifdef DRAW
		if (s3 == mirrored)
		    vga_drawpixel(rx - d->reallocx, ry - d->reallocy, 32);
#endif
	    }
	    vbuff += d->scanline;
	}
    } else
	for (s2 = 0; s2 < 2; s2++) {
	    distup = range + 2;
	    distdown = 0;
	    for (ry = d->reallocy + s2, vbuff = vbuff1 + s2 * d->scanline, rend1 = d->reallocy + d->height; ry < rend1; ry += 2) {
		assert(vbuff >= d->vbuff);
		assert(vbuff < d->vbuff + d->scanline * d->height);
		assert(rx >= d->reallocx && rx <= d->reallocx + d->width);
		assert(ry >= d->reallocy && ry <= d->reallocy + d->height);
		/*if (ry->symto == -1) { */
		if (!ry->dirty) {
		    STAT(tocalculate++);
		    if (distdown <= 0) {
			distdown = 1;
			for (rxr = ry + 1; rxr < rend1 && rxr->dirty; rxr++, distdown++);
			if (rxr == rend1)
			    distdown = d->width * 2;
		    }
		    if (
			   distup < range &&
			   distdown < range &&
			   (vbuff[-distl] == vbuff[distr] &&
			    ((
				 vbuff[-distl + distdown * d->scanline] == (c = (vbuff[distr])) &&
			     vbuff[-distl - distup * d->scanline] == c &&
			    c == vbuff[distr + distdown * d->scanline] &&
			     c == vbuff[distr - distup * d->scanline]) &&
			     (!s2 ||
			      (
				  ((ry + 1)->dirty ||
				   vbuff[d->scanline] == c) &&
				  ((ry - 1)->dirty ||
				   vbuff[-d->scanline] == c)
			      ))))) {
			STAT(avoided++);
			*vbuff = vbuff[distr];
		    } else {
#ifdef DRAW
			s3 = mirrored;
#endif
			*vbuff = calculate(x, ry->possition);
#ifdef DRAW
			if (s3 == mirrored)
			    vga_drawpixel(rx - d->reallocx, ry - d->reallocy, 255);
#endif
		    }
		    distup = 0;
		}
		vbuff += d->scanline << 1;
		if (!(ry + 1)->dirty)
		    distup = -1;
		distdown -= 2;
		distup += 2;
	    }
	}
    rx->recalculate = 0;
    rx->dirty = 0;
}

static /*INLINE */ void calculatenew(void)
{
    int s;
    unsigned char *vbuff = d->vbuff;
    realloc_t *rx, *ry, *rend;
    /*int s3; */
    STAT(mirrored = 0);
    STAT(tocalculate = 0);
    STAT(avoided = 0);
    d->pass = "Solid guessing 1";
    d->max = 0;
    d->pos = 0;
    for (ry = d->reallocy, rend = ry + d->height; ry < rend; ry++) {
	if (ry->recalculate)
	    d->max++;
    }
    callwait();
    for (s = 0; s < 2; s++) {
	vbuff = d->vbuff + s * d->scanline;
	for (ry = d->reallocy + s, rend = ry + d->height - s; ry < rend; ry += 2) {
	    if (ry->recalculate) {
		calcline(vbuff, ry);
		d->pos++;
		callwait();
		if (d->interrupt)
		    return;
	    }			/*if recalculate */
	    vbuff += d->scanline << 1;
	}			/*for ry */
    }				/*for s */
    d->max = 0;
    d->pass = "Solid guessing 2";
    d->pos = 0;
    for (rx = d->reallocx, rend = rx + d->width; rx < rend; rx++) {
	if (rx->recalculate)
	    d->max++;
    }
    for (s = 0; s < 2; s++) {
	vbuff = d->vbuff + s;
	for (rx = d->reallocx + s, rend = rx + d->width - s; rx < rend; rx += 2) {
	    if (rx->recalculate) {
		calccolumn(vbuff, rx);
		d->pos++;
		callwait();
		if (d->interrupt)
		    return;
	    }
	    vbuff += 2;
	}
    }
    STAT(printf("Avoided caluclating of %i points from %i and mirrored %i %2.2f%% %2.2f%%\n", avoided, tocalculate, mirrored, 100.0 * (avoided + mirrored) / tocalculate, 100.0 * (tocalculate - avoided - mirrored) / d->width / d->height));
    STAT(mirrored2 += mirrored;
	 avoided2 += avoided;
	 tocalculate2 += tocalculate;
	 frames2 += 1);
}
static INLINE void addprices(realloc_t * r, realloc_t * r2)
{
    realloc_t *r3;
    while (r < r2) {
	r3 = r + (((int) (r2 - r)) >> 1);
	r3->price = (r2->possition - r3->possition) * (r3->price);
	if (r3->symref != -1)
	    r3->price = r3->price / 2;
	addprices(r, r3);
	r = r3 + 1;
    }
}
static /*INLINE */ void dosymetry(void)
{
    unsigned char *vbuff = d->vbuff;
    realloc_t *rx, *ry, *rend;

    vbuff = d->vbuff;
    for (ry = d->reallocy, rend = ry + d->height; ry < rend; ry++) {
	if (ry->symto != -1) {
	    if (!d->reallocy[ry->symto].dirty) {
		memcpy(vbuff, d->vbuff + d->scanline * ry->symto, d->width * sizeof(*vbuff));
		ry->dirty = 0;
	    }
	}
	vbuff += d->scanline;
    }
    vbuff = d->vbuff;
    for (rx = d->reallocx, rend = rx + d->width; rx < rend; rx++) {
	if (rx->symto != -1) {
	    if (!d->reallocx[rx->symto].dirty) {
		unsigned char *vsrc = d->vbuff + rx->symto, *vend = vbuff + d->scanline * d->height;
		for (; vbuff < vend; vbuff += d->scanline, vsrc += d->scanline)
		    *vbuff = *vsrc;
		vbuff -= d->scanline * d->height;
		rx->dirty = 0;
	    }
	}
	vbuff++;
    }
}
static /*INLINE */ void fill(void)
{
    register unsigned char *vbuff = d->vbuff, *vsrc = NULL, *vend;
    realloc_t *rx, *ry, *r1, *r2, *rend, *rs, *rs1;
    int n = 0;

    vbuff = d->vbuff;
    for (ry = d->reallocy, rend = ry + d->height; ry < rend; ry++) {
	if (ry->dirty) {
	    d->uncomplette = 1;
	    r1 = ry - 1;
	    for (r2 = ry + 1; r2->dirty && r2 < rend; r2++);
	    while (ry->dirty && ry < rend) {
		if (r1 < d->reallocy)
		    rs = r2;
		else if (r2 >= rend)
		    rs = r1;
		else if (ry->possition - r1->possition < r2->possition - ry->possition)
		    rs = r1;
		else
		    rs = r2;
		if (rs->dirty || rs >= rend || rs < d->reallocy) {
		    printf("FAIL!\n");
		    init_tables(d);
		    return;
		}
		memcpy(d->vbuff + d->scanline * (ry - d->reallocy), d->vbuff + d->scanline * (rs - d->reallocy), d->width * sizeof(*vbuff));
		ry->dirty = 0;
		ry->possition = rs->possition;
		ry++;
	    }
	}
    }
    vbuff = d->vbuff;
    vend = vbuff + d->scanline * d->height;
    for (rx = d->reallocx, rend = rx + d->width; rx < rend; rx++) {
	if (rx->dirty) {
	    d->uncomplette = 1;
	    r1 = rx - 1;
	    for (r2 = rx; r2->dirty && r2 < rend; r2++);
	    rs1 = NULL;
	    while (rx->dirty && rx < rend) {
		if (r1 < d->reallocx)
		    rs = r2;
		else if (r2 >= rend)
		    rs = r1;
		else if (rx->possition - r1->possition < r2->possition - rx->possition)
		    rs = r1;
		else
		    rs = r2;
		if (rs->dirty || rs >= rend || rs < d->reallocx) {
		    init_tables(d);
		    printf("FAIL!\n");
		    return;
		}
		if (rs != rs1) {
		    if (rs1 != NULL) {
			if (n == 1) {
			    register int /*plus=4*d->scanline, */ plus1 = d->scanline;
#ifdef _UNDEFINED_
			    vend -= plus;
			    for (; vbuff < vend; vbuff += plus, vsrc += plus)
				*vbuff = *vsrc,
				    *(vbuff + plus1) = *(vsrc + plus1),
				    *(vbuff + 2 * plus1) = *(vsrc + 2 * plus1),
				    *(vbuff + 3 * plus1) = *(vsrc + 3 * plus1);
			    vend += plus;
#endif
			    for (; vbuff < vend; vbuff += plus1, vsrc += plus1)
				*vbuff = *vsrc;
			} else {
			    for (; vbuff < vend; vbuff += d->scanline, vsrc += d->scanline)
				memset((char *) vbuff, (int) *vsrc, (size_t) n);
			}
		    }
		    rs1 = rs;
		    vbuff = d->vbuff + (rx - d->reallocx);
		    vsrc = d->vbuff + (rs - d->reallocx);
		    n = 1;
		} else
		    n++;
		rx->dirty = 0;
		rx->possition = rs->possition;
		rx++;
	    }
	    if (rs1 != NULL)
		if (n == 1) {
		    for (; vbuff < vend; vbuff += d->scanline, vsrc += d->scanline)
			*vbuff = *vsrc;
		} else {
		    for (; vbuff < vend; vbuff += d->scanline, vsrc += d->scanline)
			memset((char *) vbuff, (int) *vsrc, (size_t) n);
		}
	}
    }
}
static int fcmp(CONST void *t1, CONST void *t2)
{
    number_t a = (*(realloc_t **) t1)->price - (*(realloc_t **) t2)->price;
    if (a < 0.0)
	return 1;
    if (a > 0.0)
	return -1;
    return 0;
}
static void calculatenewinterruptible(void)
{
    int dx = 0, dy = 0;
    realloc_t *r, *r2, *end, *end1;
    realloc_t **table, **tptr, **tptr1;
    tptr = table = (realloc_t **) malloc(sizeof(*table) * (d->width + d->height));
    end = d->reallocx + d->width;
    STAT(mirrored = 0);
    STAT(tocalculate = 0);
    STAT(avoided = 0);
    d->pass = "Solid guessing";
    for (r = d->reallocx; r < end; r++)
	if (r->dirty)
	    dx++;
    for (r = d->reallocx; r < end; r++) {
	if (r->recalculate) {
	    for (r2 = r; r2->recalculate && r2 < end; r2++)
		*(tptr++) = r2;
	    if (r2 == end)
		/*(r2 - 1)->price = 0, */ r2--;
	    addprices(r, r2);
	    r = r2;
	}
    }
    end1 = d->reallocy + d->height;
    for (r = d->reallocy; r < end1; r++)
	if (r->dirty)
	    dy++;
    for (r = d->reallocy; r < end1; r++) {
	if (r->recalculate) {
	    for (r2 = r; r2->recalculate && r2 < end1; r2++)
		*(tptr++) = r2;
	    if (r2 == end1)
		/*(r2 - 1)->price = 0, */ r2--;
	    addprices(r, r2);
	    r = r2;
	}
    }
    qsort((void *) table, tptr - table, sizeof(*table), (void *)fcmp);
    tptr1 = table;
    d->max = tptr - table;
    d->incalculation = 1;
    while (tptr1 < tptr && !d->interrupt) {
	r = *tptr1;
	d->pos++;
	if (dx < d->width - 2 && dy < d->height - 2)
	    d->readyforinterrupt = 1;
	callwait();
	if (r >= d->reallocx && r < end) {
	    unsigned char *vbuf;
	    vbuf = d->vbuff + (r - d->reallocx);
	    dx--;
	    calccolumn(vbuf, r);
	} else {
	    unsigned char *vbuf;
	    vbuf = d->vbuff + (r - d->reallocy) * d->scanline;
	    dy--;
	    calcline(vbuf, r);
	}
	tptr1++;
    }
    callwait();
    free((void *) table);
    d->pos = 0;
    d->max = 0;
    d->pass = "Procesing symetries";
    d->incalculation = 0;
    callwait();
    dosymetry();
    if (d->interrupt) {
	d->pass = "reducing resolution";
	fill();
    }
    STAT(printf("Avoided caluclating of %i points from %i and mirrored %i %2.2f%% %2.2f%%\n", avoided, tocalculate, mirrored, 100.0 * (avoided + mirrored) / tocalculate, 100.0 * (tocalculate - avoided - mirrored) / d->width / d->height));
    STAT(mirrored2 += mirrored;
	 avoided2 += avoided;
	 tocalculate2 += tocalculate;
	 frames2 += 1);
}

static void /*INLINE */ combine_methods(void)
{
    int i, j;
    struct symetryinfo *s1 = d->currentformula->out + d->coloringmode,
    *s2 = d->currentformula->in + d->incoloringmode;
    if (d->mandelbrot != d->currentformula->mandelbrot) {
	cursymetry.xsym = INT_MAX;
	cursymetry.ysym = INT_MAX;
	cursymetry.nsymetries = 0;
	return;
    }
    xmul = d->width / (d->s.mc - d->s.nc);
    ymul = d->height / (d->s.mi - d->s.ni);
    xdist = (d->s.mc - d->s.nc) / d->width / 6;
    ydist = (d->s.mi - d->s.ni) / d->height / 6;
    if (s1->xsym == s2->xsym)
	cursymetry.xsym = s1->xsym;
    else
	cursymetry.xsym = INT_MAX;
    if (s1->ysym == s2->ysym)
	cursymetry.ysym = s1->ysym;
    else
	cursymetry.ysym = INT_MAX;
#ifdef _RED_
    if (d->plane == P_PARABOL)
	cursymetry.xsym = INT_MAX;
    if (d->plane == P_LAMBDA) {
	if (cursymetry.xsym == 0 && cursymetry.ysym == 0)
	    cursymetry.xsym = 1;
	else
	    cursymetry.xsym = INT_MAX;
    }
    if (d->plane == P_INVLAMBDA)
	cursymetry.xsym = INT_MAX;
    if (d->plane == P_TRANLAMBDA) {
	if (cursymetry.xsym != 0 || cursymetry.ysym != 0)
	    cursymetry.xsym = INT_MAX;
    }
    if (d->plane == P_MEREBERG)
	cursymetry.xsym = INT_MAX;
#endif
    cursymetry.symetry = sym_lines;
    cursymetry.nsymetries = 0;
#ifdef _RED_
    if (d->plane == P_PARABOL || d->plane == P_LAMBDA || d->plane == P_INVLAMBDA ||
	d->plane == P_TRANLAMBDA || d->plane == P_MEREBERG)
	return;
#endif
    for (i = 0; i < s1->nsymetries; i++) {
	number_t y1 = s1->symetry[i].y0 + d->s.nc * s1->symetry[i].k;
	number_t y2 = s1->symetry[i].y0 + d->s.mc * s1->symetry[i].k;
	if ((y1 > d->s.ni ? (y1 < d->s.mi || y2 < d->s.mi) :
	     y2 > d->s.ni)) {
	    for (j = 0; j < s2->nsymetries; j++) {
		if (s1->symetry[i].k == s2->symetry[j].k &&
		    s1->symetry[i].y0 == s2->symetry[j].y0) {
		    cursymetry.symetry[cursymetry.nsymetries].k = s1->symetry[i].k;
		    cursymetry.symetry[cursymetry.nsymetries].y0 = s1->symetry[i].y0;
		    cursymetry.symetry[cursymetry.nsymetries].kk = s1->symetry[i].k * s1->symetry[i].k + 1;
		    cursymetry.symetry[cursymetry.nsymetries++].y0k = s1->symetry[i].y0 * s1->symetry[i].k;
		}
	    }
	}
    }
}

void do_fractal(zoom_context * context, int interruptible)
{
    unsigned char *tmp;
    number_t *posptr;
    realloc_t *r, *rend;

    d = context;

#ifdef __GNUC__
#ifdef __i386__
    _control87(PC_64, MCW_PC);
    _control87(MCW_EM, MCW_EM);
    _control87(RC_NEAR, MCW_RC);
    _control87(IC_PROJECTIVE, MCW_IC);
#endif
#endif
    /*many sanity checks */

    checkcontext(context);
    assert(d->xpos != NULL);	/*avarything correctly allocated? */
    assert(d->ypos != NULL);
    assert(d->switch_function != NULL);
    assert(d->vbuff != NULL);
    assert(d->back != NULL);
    assert(d->width > 0 && d->width < 65000);
    assert(d->height > 0 && d->height < 65000);
    assert(d->scanline >= d->width);
    assert(d->reallocx != NULL);
    assert(d->reallocy != NULL);
    /*options in allowed range? */
    assert(d->currentformula->magic == FORMULAMAGIC);
    assert(d->maxiter >= 0);
    assert(d->coloringmode >= 0);
    assert(d->incoloringmode >= 0);
    assert(d->coloringmode < OUTCOLORING);
    assert(d->incoloringmode < INCOLORING);
    assert(d->mandelbrot == 0 || d->mandelbrot == 1);
    assert(d->fullscreen == 0 || d->fullscreen == 1);
    assert(d->plane >= 0 && d->plane < PLANES);
    assert(d->num_colors > 0 && d->num_colors < 256);
    assert(d->s.mc >= d->s.nc);
    assert(d->s.mi >= d->s.ni);
    /*now we can safely start work... */
    d->uncomplette = 0;
    d->incalculation = 0;
    d->readyforinterrupt = 0;


    d->interrupt = 0;
    if (d->dirty)
	d->dirty--;
    combine_methods();
    if (cursymetry.xsym == INT_MAX)
	cursymetry.xsym = d->s.mc + INT_MAX;
    if (cursymetry.ysym == INT_MAX)
	cursymetry.ysym = d->s.mi + INT_MAX;
    d->switch_function();
    d->max = 0;
    d->pass = "Inicializing";
    d->pos = 0;
    tmp = d->vbuff;
    d->vbuff = d->back;
    d->back = tmp;
    tmp = d->svbuff;
    d->svbuff = d->sback;
    d->sback = tmp;
    MAXITER = d->maxiter;
    coloringmode = d->coloringmode;
    incoloringmode = d->incoloringmode;
    d->pass = "Making x realloc table";

    mkrealloc_table(d->xpos, d->reallocx, d->width, d->s.nc, d->s.mc, cursymetry.xsym);
    callwait();
    d->pass = "Making y realloc table";
    mkrealloc_table(d->ypos, d->reallocy, d->height, d->s.ni, d->s.mi, cursymetry.ysym);
    callwait();

    callwait();
    d->pass = "Moving old points";
    moveoldpoints();
    callwait();
    d->pass = "Starting calculation";
    if (interruptible)
	calculatenewinterruptible();
    else {
	calculatenew();
	if (d->interrupt) {
	    init_tables(d);
	    return;
	}
	callwait();
	d->pos = 0;
	d->max = 0;
	d->pass = "Procesing symetries";
	dosymetry();
    }
    callwait();
    for (r = d->reallocx, posptr = d->xpos, rend = d->reallocx + d->width;
	 r < rend; r++, posptr++) {
	*posptr = r->possition;
	/*if(*posptr==*(posptr-1)) printf("aa\n"); */
    }
    for (r = d->reallocy, posptr = d->ypos, rend = d->reallocy + d->height;
	 r < rend; r++, posptr++) {
	*posptr = r->possition;
	/*if(*posptr==*(posptr-1)) printf("aa\n"); */
    }
    STAT(printf("Statistics: frames %i\n"
		"mkrealloctable: added %i, symetry %i\n"
		"calculate loop: tocalculate %i avoided %i mirrored %i\n"
		"calculate:calculated %i inside %i\n"
		"iters inside:%i iters outside:%i\n"
		,frames2, nadded2, nsymetry2, tocalculate2, avoided2, mirrored2, ncalculated2, ninside2, niter2, niter1));
}


static void recalculate_view(zoom_context * c)
{
    number_t
	xs = c->s.mc - c->s.nc, ys = (c->s.mi - c->s.ni) * c->windowwidth / c->windowheight,
	xc = (c->s.mc + c->s.nc) / 2, yc = (c->s.mi + c->s.ni) / 2, size;
    checkcontext(c);
    assert(c->s.mc >= c->s.nc);
    assert(c->s.mi >= c->s.ni);

    xs = fabs(xs);		/*do not crash in owerflowing cases */
    ys = fabs(ys);
    if (xs > ys)
	size = xs;
    else
	size = ys;
    c->s.nc = xc - size / 2;
    c->s.mc = xc + size / 2;
    c->s.ni = yc - size * c->windowheight / c->windowwidth / 2;
    c->s.mi = yc + size * c->windowheight / c->windowwidth / 2;
    if (c->s.nc > c->s.mc)
	xc = c->s.nc, c->s.nc = c->s.mc, c->s.mc = xc;
    if (c->s.ni > c->s.mi)
	xc = c->s.ni, c->s.ni = c->s.mi, c->s.mi = xc;
}

void init_tables(zoom_context * c)
{
    int i;
    checkcontext(c);

    c->dirty = 2;
    for (i = 0; i < c->width + 1; i++)
	c->xpos[i] = INT_MAX + c->s.mc;
    for (i = 0; i < c->height + 1; i++)
	c->ypos[i] = INT_MAX + c->s.mi;
}

void set_formula(zoom_context * c, CONST int num)
{
    checkcontext(c);
    assert(num < nformulas);
    assert(num >= 0);
    c->currentformula = formulas + num;
    c->mandelbrot = c->currentformula->mandelbrot;
    c->pre = c->currentformula->pre;
    c->pim = c->currentformula->pim;
    c->s.ni = c->currentformula->v.ni;
    c->s.nc = c->currentformula->v.nc;
    c->s.mi = c->currentformula->v.mi;
    c->s.mc = c->currentformula->v.mc;
    init_tables(c);
}

static int alloc_tables(zoom_context * c)
{
    checkcontext(c);
    c->xpos = (number_t *) malloc((c->width + 1) * sizeof(*c->xpos));
    if (c->xpos == NULL)
	return 0;
    c->ypos = (number_t *) malloc((c->height + 1) * sizeof(*c->ypos));
    if (c->ypos == NULL) {
	free((void *) c->xpos);
	return 0;
    }
    c->reallocx = (realloc_t *) malloc(sizeof(realloc_t) * (c->width + 1));
    if (c->reallocx == NULL) {
	free((void *) c->xpos);
	free((void *) c->ypos);
	return 0;
    }
    c->reallocy = (realloc_t *) malloc(sizeof(realloc_t) * (c->height + 1));
    if (c->reallocy == NULL) {
	free((void *) c->xpos);
	free((void *) c->ypos);
	free((void *) c->reallocx);
	return 0;
    }
    return 1;
}

static void free_tables(zoom_context * c)
{
    checkcontext(c);
    free((void *) c->xpos);
    free((void *) c->ypos);
    free((void *) c->reallocx);
    free((void *) c->reallocy);
}

#ifdef _RED_
int resize_to(zoom_context * c, CONST int width, CONST int height, CONST int scanline, char *vbuff, char *back, double wi, double he)
{
    int s = 0;
    checkcontext(c);
    assert(width > 0 && width < 65000);
    assert(height > 0 && height < 65000);
    assert(scanline >= width);
    assert(vbuff != NULL);
    assert(back != NULL);
    /*try cause segfaults on unallocated memory */
    memset(vbuff, 0, scanline * height);
    memset(back, 0, scanline * height);
    free_tables(c);
    c->scanline = scanline;
    c->width = width;
    c->height = height;
    if (c->stereogram)
	disable_stereogram(c), s = 1;
    c->vbuff = (unsigned char *) vbuff;
    c->back = (unsigned char *) back;
    if (s)
	enable_stereogram(c);
    c->pixelwidth = wi / width;
    c->pixelheight = he / height;
    c->windowwidth = wi;
    c->windowheight = he;
    if (!alloc_tables(c))
	return 0;
    init_tables(c);
    return 1;
}
#endif

void set_view(zoom_context * c, CONST vinfo * s)
{
    checkcontext(c);
    c->s = *s;
    recalculate_view(c);
}

void free_context(zoom_context * c)
{
    checkcontext(c);
    free_tables(c);
    if (c->stereogram) {
	free(c->vbuff);
	free(c->back);
    }
    free((void *) c);
}
#ifdef _RED_
void enable_stereogram(zoom_context * context)
{
    context->svbuff = context->vbuff;
    context->sback = context->back;
    context->vbuff = malloc(context->width * context->height);
    context->back = malloc(context->width * context->height);
    context->stereogram = 1;
    init_tables(context);
}

void disable_stereogram(zoom_context * context)
{
    free(context->vbuff);
    free(context->back);
    context->vbuff = context->svbuff;
    context->back = context->sback;
    context->stereogram = 0;
    init_tables(context);
}
void rotate_palette(zoom_context * c, int direction)
{
    int i;
    int i1, i2, i3;
    unsigned char co[3][MAXCOLORS];
    checkcontext(c);
    if (direction > 0)
	direction %= c->num_colors - 1;
    else
	direction = -((-direction) % (c->num_colors - 1));
    if (!direction)
	return;
    memcpy(co, c->cmap, sizeof(co));
    i3 = (c->num_colors - 1 + direction) % (c->num_colors - 1) + 1;
    for (i = 1; i < c->num_colors; i++) {	/*this is ugly..I know */
	i1 = c->colors[i];
	i2 = c->colors[i3];
	c->cmap[0][i1] = co[0][i2];
	c->cmap[1][i1] = co[1][i2];
	c->cmap[2][i1] = co[2][i2];
	i3++;
	if (i3 >= c->num_colors)
	    i3 = 1;
    }
}
#endif


zoom_context *
 make_context(CONST int width, CONST int height, CONST int scanline, CONST int formula, CONST int full, void (*switchptr) (void), void (*waitfunc) (void), char *vbuf, char *bckup, double wi, double he)
{
    zoom_context *new;
    assert(width > 0 && width < 65000);
    assert(height > 0 && height < 65000);
    assert(scanline >= width);
    /*try cause segfaults on unallocated memory */
    memset(vbuf, 0, scanline * height);
    memset(bckup, 0, scanline * height);
    assert(vbuf != NULL);
    assert(bckup != NULL);
    assert(formula >= 0 && formula < nformulas);
    assert(switchptr != NULL);

    new = (zoom_context *) calloc(sizeof(zoom_context), 1);
    if (new == NULL)
	return 0;
    new->magic = CONTEXTMAGIC;
    new->fullscreen = full;
    new->pixelwidth = wi / width;
    new->pixelheight = he / height;
    new->windowwidth = wi;
    new->windowheight = he;
    new->scanline = scanline;
    new->width = width;
    new->maxiter = DEFAULT_MAX_ITER;
    new->coloringmode = 0;
    new->height = height;
    new->range = 3;
    new->stereogram = 0;
    new->vbuff = (unsigned char *) vbuf;
    new->back = (unsigned char *) bckup;
    new->switch_function = switchptr;
    new->wait_function = waitfunc;
    if (!alloc_tables(new))
	return NULL;
    set_formula(new, formula);
    return (new);
}
