/* This file was taken from XaoS-the fast portable realtime interactive 
   fractal zoomer. but it is simplified for BB. You may get complette
   sources at XaoS homepage (http://www.paru.cas.cz/~hubicka/XaoS
 */
#ifdef _plan9_
#include <u.h>
#include <stdio.h>
#include <libc.h>
#else
#include <math.h>
#include <string.h>
#endif
#include "zoom.h"
#include "complex.h"
/*This code is really ugly-unstrictalized, unextensible, works just
   for rectangular views. I didn't discovered better way how to get
   higher speed. For example in case I will implement zre zim iterating
   function as seperate static one should drastically slow down on my
   hp425e. Also gotos improves speed-does not force useless if..
   I can not use normal calculating routines, because lots of thinks
   must be done in internal loop so internal loop must be inside
   main loop, because shares lots of variables otherwise register
   alocation is imposible
 */

#undef SAG			/*solid anti-guessing */
#define NOT_CALCULATED 0
#define INSET 254
#define INPROCESS 255
#define RMIN -range
#define RMAX range
#define IMIN -range
#define IMAX range
#define QMAX 1000
#ifdef STATISTICS
int iters2, guessed2, unguessed2, total2, frames2;
#endif
void do_julia(zoom_context * context, register number_t pre, register number_t pim)
{
    int i, i1, i2, j, x, y, scanline = context->scanline;
    int iter;
    number_t rp = 0, ip = 0 /*, t */ ;
    register number_t zre, zim, im, xdelta, ydelta, br, tmp, range,
     rangep;
    number_t xstep, ystep;
    unsigned char *queue[QMAX];
    unsigned char **qptr;
    unsigned char *addr, *addr1 = context->vbuff;
    const int mode = context->currentformula->juliamode;
#ifdef STATISTICS
    int guessed = 0, unguessed = 0, iters = 0;
    frames2++;
#endif
    if (mode != 6)
	range = 2.0;
    else
	range = 4.0;
    rangep = range * range;

    for (i = 0; i < context->height; i++) {
	addr = addr1 + i * context->scanline;
	memset((char *) addr, NOT_CALCULATED, context->width);
    }
    xdelta = context->width / (RMAX - RMIN);
    ydelta = context->height / (IMAX - IMIN);
    xstep = (RMAX - RMIN) / context->width;
    ystep = (IMAX - IMIN) / context->height;
    for (i = 0; i < context->height; i++) {
	im = IMIN + (i + 0.5) * ystep;
	x = sqrt(rangep - im * im) * xdelta + 0.5;
	addr = addr1 + i * scanline;
	y = context->width / 2 - x;
	for (j = 0; j < y; j++) {
	    addr[j] = 1;
	}
	y = context->width;
	for (j = context->width / 2 + x; j < y; j++) {
	    addr[j] = 1;
	}
    }
    for (i2 = 0; i2 < 2; i2++)
	for (i1 = 0; i1 < context->height; i1++) {
	    if (i1 % 2)
		i = context->height / 2 - i1 / 2;
	    else
		i = context->height / 2 + i1 / 2 + 1;
	    if (i >= context->height)
		continue;
	    im = IMIN + (i + 0.5) * ystep;
	    for (j = (i + i2) & 1; j < context->width; j += 2) {
		STAT(total2++;
		    )
		    addr = addr1 + j + i * scanline;
		if (*addr != NOT_CALCULATED)
		    continue;
		x = j;
		y = i;
		if (x > 0 && x < context->width &&
		    *(addr + 1) != NOT_CALCULATED && *(addr + 1) == *(addr - 1) && y > 0 && y < context->height && *(addr + 1) ==
		*(addr + scanline) && *(addr + 1) == *(addr - scanline)) {
		    *addr = *(addr + 1);
		    continue;
		}
		zim = im;
		zre = RMIN + (j + 0.5) * xstep;
		iter = 0;
		qptr = queue;
		ip = (zim * zim);
		rp = (zre * zre);
		if (rp + ip > rangep)
		    goto outset;
		while (1) {
		    if (*addr != NOT_CALCULATED
#ifdef SAG
			&& (*addr == INPROCESS ||
			    (!(x > 0 && x < context->width
			       && y > 0 && y < context->height &&
			       *(addr + 1) != NOT_CALCULATED && (
								    (*(addr + 1) != *(addr - 1) && *(addr - 1) != NOT_CALCULATED) ||
								    (*(addr + 1) != *(addr + scanline) && *(addr + scanline) != NOT_CALCULATED) ||
								    (*(addr + 1) != *(addr - scanline) && *(addr - scanline) != NOT_CALCULATED)))))
#endif
			) {
			if (*addr == INPROCESS || *addr == INSET) {
			    *qptr = addr;
			    qptr++;
			    STAT(guessed++;
				)
				goto inset;
			}
			STAT(guessed++;
			    )
			    iter = *addr;
			goto outset;
		    }
#ifdef STATISTICS
		    if (*addr != NOT_CALCULATED)
			unguessed++;
#endif
		    if (*addr != INPROCESS) {
			*qptr = addr;
			qptr++;
			*addr = INPROCESS;
			if (qptr >= queue + QMAX)
			    goto inset;
		    }
		    STAT(iters++;
			)
			switch (mode) {		/*technique called cut and paste */
		    case 0:
			zim = (zim * zre) * 2 + pim;
			zre = rp - ip + pre;
			break;
		    case 1:
			rp = zre * (rp - 3 * ip);
			zim = zim * (3 * zre * zre - ip) + pim;
			zre = rp + pre;
			break;
#ifdef _RED_
		    case 2:
			rp = rp * rp - 6 * rp * ip + ip * ip + pre;
			zim = 4 * zre * zre * zre * zim - 4 * zre * ip * zim + pim;
			zre = rp;
			break;
		    case 3:
			c_pow4(zre, zim, rp, ip);
			c_mul(zre, zim, rp, ip, t, zim);
			zre = t + pre;
			zim += pim;
			break;
		    case 4:
			c_pow3(zre, zim, rp, ip);
			c_pow3(rp, ip, t, zim);
			zre = t + pre;
			zim += pim;
			break;
		    case 5:
			if (zre >= 0) {
			    c_mul(zre - 1, zim, pre, pim, rp, zim);
			}
			else {
			    c_mul(zre + 1, zim, pre, pim, rp, zim);
			}
			zre = rp;
			break;
		    case 6:
			br = zre + zre + pre - 2;
			tmp = zre * zim;
			zre = rp - ip + pre - 1;
			ip = zim + zim + pim;
			zim = tmp + tmp + pim;
			tmp = br * br + ip * ip;
			rp = (zre * br + zim * ip) / tmp;
			ip = (zim * br - zre * ip) / tmp;
			zre = (rp + ip) * (rp - ip);
			zim = rp * ip;
			zim += zim;
			rp = zre - 1;
			if (rp * rp + zim * zim < 0.001)
			    goto outset;
			break;
#endif
		    }
		    ip = (zim * zim);
		    rp = (zre * zre);
		    if (rp + ip > 4)
			goto outset;
		    x = (zre - RMIN) * xdelta;
		    y = (zim - IMIN) * ydelta;
		    addr = addr1 + x + y * scanline;
		    if (x > 0 && x < context->width &&
			*(addr + 1) != NOT_CALCULATED && *(addr + 1) == *(addr - 1) && y > 0 && y < context->height && *(addr + 1) ==
			*(addr + scanline) && *(addr + 1) == *(addr - scanline)) {
			*addr = *(addr + 1);
		    }
		}
		addr = addr1 + x + y * scanline;
		if (*addr == INPROCESS || *addr == INSET) {
		  inset:
		    while (qptr > queue) {
			qptr--;
			**qptr = INSET;
		    }
		}
		else {
		  outset:
		    x = context->maxiter;
		    y = context->num_colors;
		    while (qptr > queue) {
			qptr--;
			**qptr = (iter % y) + 1;
			iter++;
		    }
		}
	    }
	}
    for (i = 0; i < context->height; i++) {
	unsigned char *colors = context->colors;
	addr = context->vbuff + i * context->scanline;
	addr1 = addr + context->width;
	x = context->num_colors;
	while (addr < addr1) {
	    *addr = colors[*addr % x];
	    addr++;
	}
    }
#ifdef STATISTICS
    printf("iters %i guessed %i unguessed %i\n", iters, guessed, unguessed);
    iters2 += iters;
    guessed2 += guessed;
    unguessed2 += unguessed;
    printf("frames %i total %i iters %i guessed %i unguessed %i\n", frames2, total2, iters2, guessed2, unguessed2);
#endif
}
