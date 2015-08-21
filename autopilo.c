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
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "config.h"
#include <assert.h>
#endif
#include "zoom.h"
#include "autopilo.h"
#include "ui.h"

#define InSet(i) (i==context->colors[0])
static number_t minsize = 1000;
static number_t maxsize = 0;
static int autime = 0;
static int minlong = 0;
static int interlevel;
#define MINCOUNT 5
void clean_autopilot(void)
{
    minsize = 1000;
    maxsize = 0;
    autime = 0;
    minlong = 0;
}
void do_autopilot(zoom_context * context, int *x, int *y, int *controls, void (*changed) (void))
{
    static int x1, y1, c1;
    int i, j, i1, j1, c, max;
    checkcontext(context);
    assert(changed != NULL);
    if (context->dirty)
	clean_autopilot();
    if (context->s.mc - context->s.nc < minsize) {
	minsize = context->s.mc - context->s.nc;
	minlong = 0;
    }
    if (context->s.mc - context->s.nc > maxsize) {
	minsize = context->s.mc - context->s.nc;
	maxsize = context->s.mc - context->s.nc;
	minlong = 0;
    }
    j = 0;
    for (i = 0; i < context->width - 1; i++) {	/*detection of out of precisity... */
	if (context->xpos[i] == context->xpos[i + 1])
	    j++;
    }
    if (autime <= 0) {
	if (context->uncomplette && j > context->width / 2) {
	    *controls = 0;
	    return;
	}
	minlong++;
	max = NGUESSES1;
	ui_tbreak();
	do {
	    max--;
	    c = 0;
	    x1 = rand() % (context->width - 2 * LOOKSIZE) + LOOKSIZE;
	    y1 = rand() % (context->height - 2 * LOOKSIZE) + LOOKSIZE;
	    autime = rand() % MAXTIME;
	    for (i = x1 - LOOKSIZE; i <= x1 + LOOKSIZE; i++)
		for (j = y1 - LOOKSIZE; j <= y1 + LOOKSIZE; j++)
		    if (InSet(*(context->vbuff + i + j * context->scanline)))
			c++;
	} while ((c == 0 || c > LOOKSIZE * LOOKSIZE) && max > 0);
	if (max > 0)
	    c1 = BUTTON1, interlevel = 1;
	else {
	    max = NGUESSES2;
	    do {
		max--;
		c = 0;
		x1 = rand() % (context->width - 2 * LOOKSIZE) + LOOKSIZE;
		y1 = rand() % (context->height - 2 * LOOKSIZE) + LOOKSIZE;
		autime = rand() % MAXTIME;
		for (i = x1 - LOOKSIZE; i <= x1 + LOOKSIZE; i++)
		    for (j = y1 - LOOKSIZE; j < y1 + LOOKSIZE; j++)
			for (i1 = x1 - LOOKSIZE; i1 < x1 + LOOKSIZE; i1++)
			    for (j1 = y1 - LOOKSIZE; j1 < y1 + LOOKSIZE; j1++)
				if (i1 != i && j1 != j &&
				    (*(context->vbuff + i + j * context->scanline)) ==
				    (*(context->vbuff + i1 + j1 * context->scanline)))
				    c++;
	    } while ((c > LOOKSIZE * LOOKSIZE / 2) && max > 0);
	    if (max > 0)
		c1 = BUTTON1, interlevel = 2;
	    else {
		if (context->uncomplette) {
		    c1 = 0;
		    autime = 20;
		}
		else {
		    c1 = BUTTON3, autime >>= 1;
		}
	    }
	}
    }
    autime--; {
	*x = x1;
	*y = y1;
	*controls = c1;
    }
}
