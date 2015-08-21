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
#include <aalib.h>
#include "bb.h"
#include "tex.h"

#define IN(v2,v1) ((((double)v1)*mul1+((double)v2)*mul2))

int salpha, sbeta, sgamma, scenterx, scentery, scenterz;
float szoom;
int ealpha, ebeta, egamma, ecenterx, ecentery, ecenterz;
float ezoom;
float poww = 1;

static void draw3d(void)
{
    int time = TIME;
    double mul1, mul2, div = (double) (endtime - starttime);
    if (time < starttime - 1)
	time = starttime + 1;
    if (time > endtime)
	time = endtime;
    mul1 = pow((double) (time - starttime) / div, poww), mul2 = 1 - mul1;
    if (mul2 < 0)
	mul1 = 1, mul2 = 0;
    if (mul1 < 0)
	mul2 = 1, mul1 = 0;
    alfa = ((int) IN(salpha, ealpha)) % 360;
    beta = ((int) IN(sbeta, ebeta)) % 360;
    gama = ((int) IN(sgamma, egamma)) % 360;
    centerx = IN(scenterx, ecenterx);
    centery = IN(scentery, ecentery);
    centerz = IN(scenterz, ecenterz);
    zoom = IN(szoom, ezoom);
    disp3d();
    aa_render(context, params, 0, 0, aa_scrwidth(context), aa_scrheight(context));
    aa_flush(context);
}

static void do3d(int time)
{
    timestuff(0, NULL, draw3d, time);
    ealpha %= 360;
    ebeta %= 360;
    egamma %= 360;
    salpha = ealpha;
    sbeta = ebeta;
    sgamma = egamma;
    szoom = ezoom;
    scenterx = ecenterx;
    scentery = ecentery;
    scenterz = ecenterz;
}

void scene10()
{
    salpha = sbeta = sgamma = scenterx = scentery = scenterz;
    szoom = 0;
    ealpha = ebeta = egamma = ecenterx = ecentery = ecenterz = 0;
    ezoom = 0;
    poww = 1;
    drawptr = NULL;
    patnikconstructor();
    params->gamma = 1;
    centery = -40;
    strobikstart();
    zoom = 3;
    alfa = 90;
    beta = 0;
    gama = 180;
    disp3d();
    aa_render(context, params, 0, 0, aa_scrwidth(context), aa_scrheight(context));
    strobikend();
    bbwait(500000);

    strobikstart();
    alfa = 0;
    disp3d();
    aa_render(context, params, 0, 0, aa_scrwidth(context), aa_scrheight(context));
    strobikend();
    bbwait(500000);

    strobikstart();
    alfa = 180;
    disp3d();
    aa_render(context, params, 0, 0, aa_scrwidth(context), aa_scrheight(context));
    strobikend();
    bbwait(500000);

    strobikstart();
    alfa = 270;
    disp3d();
    aa_render(context, params, 0, 0, aa_scrwidth(context), aa_scrheight(context));
    strobikend();

    salpha = 270;
    sbeta = 0;
    sgamma = 180;
    szoom = 3;
    scentery = -40;
    ealpha = 360 + 90;
    ebeta = 0;
    egamma = 180;
    ezoom = 3;
    ecentery = -40;
    do3d(4 * 1000000);
    poww = 3;
    ezoom = 2;
    ebeta = 90;
    ecenterz = 60;
    ecentery = 50;
    do3d(3 * 1000000);
    poww = 0.4;
    ebeta = 60;
    do3d(0.5 * 1000000);
    poww = 5;
    ebeta = 90;
    do3d(0.5 * 1000000);
    poww = 2;
    ecenterz = 0;
    ecenterx = 60;
    ebeta = 0;
    egamma = 180 * 5;
    ezoom = 0.1;
    do3d(11.5 * 1000000);
    params->gamma = 1;

}
