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

#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <aalib.h>
#include "bb.h"

#define DEG (M_PI/180)
#define X_s aa_imgwidth(context)
#define Y_s aa_imgheight(context)
#define data context->imagebuffer
#define X_S (X_s/2)
#define Y_S (Y_s/2)
#define Tx 127
#define Ty 127
#define XRATIO X_s*zoom/320
#define YRATIO Y_s*zoom*aa_mmwidth(context)/aa_mmheight(context)/320
#define Bufflen 768
#define VERT 3
#define ArC 12000
#define MaxFaces 500

aa_context *context;
aa_renderparams *params;
typedef unsigned char byte;
typedef unsigned short int word;
typedef unsigned long dword;
typedef struct {
    unsigned short int x, y;
} INFO;
typedef long TBL[361];
typedef struct POL {
    long x, y, z, n_x, n_y, n_z;
} POLYS[3];
typedef struct {
    long x, y, z, nx, ny, nz;
} TRANSFORMED;
typedef TRANSFORMED TRANS[MaxFaces][3];

static TBL sinn, coss;
static int nFaces;
dword alfa, beta, gama;
int centerx, centery, centerz;
float zoom = 2.0;
static byte *envmapa;
static POLYS *obj, rot[MaxFaces];
static TRANS polyg;
static int *zbuff;

#include "patnik.h"
#include "torus.h"

static void precalculate(void)
{
    unsigned i;
    for (i = 0; i <= 360; i++) {
	sinn[i] = (long) (sin(i * DEG) * 65536);
	coss[i] = (long) (cos(i * DEG) * 65536);
    }
}
void set_zbuff()
{
    zbuff = (int *) malloc(X_s * Y_s * sizeof(int));
}

void unset_zbuff()
{
    free(zbuff);
}

static inline void clear_zbuff()
{
    memset(zbuff, 0x55, (X_s * Y_s * sizeof(long)));
}


static inline void makej(long x1, long x2, long n1z, long n2z, long ii, long n1x, long n1y, long n2x, long n2y)
{
    register byte *off, *off1;
    register int *zbptr = zbuff + (ii * X_s) + x1;
    long n3x;
    long n3y;
    long n3z;
    long n3Dx;
    long n3Dy;
    long n3Dz;
    long e, rx, ry;

    if (ii >= Y_s || ii < 0 || x2 < 0 || x1 >= X_s)
	return;
    if (x1 < 0)
	x1 = 0;
    if (x2 >= X_s)
	x2 = X_s - 1;
    e = x2 - x1;
    if (!e)
	return;
    n3Dx = (n2x - n1x) / e;
    n3x = n1x;
    n3Dy = (n2y - n1y) / e;
    n3y = n1y;
    n3Dz = (n2z - n1z) / e;
    n3z = n1z;

    off = data + (ii * X_s) + x1;
    off1 = off + x2 - x1;
    for (; off <= off1;) {

	ry = (abs(n3y) >> 7) + 64;
	rx = (abs(n3x) >> 7) + 64;
	if (((n3z >> 8) - *zbptr) < 500) {
	    *off = *(envmapa + (((unsigned int) ((ry << 7) + rx)) & (128 * 128 - 1)));
	    *zbptr = n3z >> 8;
	}
	off++;
	zbptr++;
	n3x += n3Dx;
	n3y += n3Dy;
	n3z += n3Dz;
    }
}

static void optfsp(unsigned wh)
{
    byte pivot, maxy, a, b, c;
    long ii, aa, bb;
    long x1, x2, d1, d2, m1, m2, m3;

    long n1x, n1y, n1z;
    long n2x, n2y, n2z;
    long n1Dx, n1Dy, n1Dz;
    long n2Dx, n2Dy, n2Dz;

    pivot = maxy = 0;
    if (polyg[wh][pivot].y > polyg[wh][1].y)
	pivot = 1;
    if (polyg[wh][pivot].y > polyg[wh][2].y)
	pivot = 2;
    if (polyg[wh][maxy].y < polyg[wh][1].y)
	maxy = 1;
    if (polyg[wh][maxy].y < polyg[wh][2].y)
	maxy = 2;

    a = b = pivot;
    c = maxy;
  a01:
    if (b == 2)
	b = 0;
    else
	b++;
    if ((b == pivot) || (b == maxy))
	goto a01;

    m1 = polyg[wh][c].y - polyg[wh][a].y;
    m2 = polyg[wh][b].y - polyg[wh][a].y;
    m3 = polyg[wh][c].y - polyg[wh][b].y;

    if (m1) {
	d1 = ((polyg[wh][c].x - polyg[wh][a].x) << 16) / m1;
	x1 = polyg[wh][a].x << 16;
	n1Dx = (rot[wh][c].n_x - rot[wh][a].n_x) / m1;
	n1x = rot[wh][a].n_x;
	n1Dy = (rot[wh][c].n_y - rot[wh][a].n_y) / m1;
	n1y = rot[wh][a].n_y;
	n1Dz = (polyg[wh][c].z - polyg[wh][a].z) / m1;
	n1z = polyg[wh][a].z;


	ii = 0;
	if (m2) {
	    d2 = ((polyg[wh][b].x - polyg[wh][a].x) << 16) / m2;
	    x2 = polyg[wh][a].x << 16;
	    n2Dx = (rot[wh][b].n_x - rot[wh][a].n_x) / m2;
	    n2x = rot[wh][a].n_x;
	    n2Dy = (rot[wh][b].n_y - rot[wh][a].n_y) / m2;
	    n2y = rot[wh][a].n_y;
	    n2Dz = (polyg[wh][b].z - polyg[wh][a].z) / m2;
	    n2z = polyg[wh][a].z;
	    for (ii = polyg[wh][a].y; ii <= polyg[wh][b].y; ii++) {
		aa = x1 >> 16;
		bb = x2 >> 16;
		if (aa > bb)
		    makej(bb, aa, n2z, n1z, ii, n2x, n2y, n1x, n1y);
		if (aa < bb)
		    makej(aa, bb, n1z, n2z, ii, n1x, n1y, n2x, n2y);
		x1 += d1;
		x2 += d2;
		n1x += n1Dx;
		n1y += n1Dy;
		n1z += n1Dz;
		n2x += n2Dx;
		n2y += n2Dy;
		n2z += n2Dz;
	    }
	}
	if (m3) {
	    d2 = ((polyg[wh][c].x - polyg[wh][b].x) << 16) / m3;
	    x2 = polyg[wh][b].x << 16;
	    n2Dx = (rot[wh][c].n_x - rot[wh][b].n_x) / m3;
	    n2x = rot[wh][b].n_x;
	    n2Dy = (rot[wh][c].n_y - rot[wh][b].n_y) / m3;
	    n2y = rot[wh][b].n_y;
	    n2Dz = (polyg[wh][c].z - polyg[wh][b].z) / m3;
	    n2z = polyg[wh][b].z;
	    for (ii = polyg[wh][b].y + 1; ii <= polyg[wh][c].y; ii++) {
		aa = x1 >> 16;
		bb = x2 >> 16;
		if (aa > bb)
		    makej(bb, aa, n2z, n1z, ii, n2x, n2y, n1x, n1y);
		if (aa < bb)
		    makej(aa, bb, n1z, n2z, ii, n1x, n1y, n2x, n2y);
		x1 += d1;
		x2 += d2;
		n1x += n1Dx;
		n1y += n1Dy;
		n1z += n1Dz;
		n2x += n2Dx;
		n2y += n2Dy;
		n2z += n2Dz;
	    }
	}
    }
}

static void show(void)
{
    double ZZ;
    unsigned i, j;
    for (i = 0; i < nFaces; i++)
	for (j = 0; j < 3; j++) {
	    polyg[i][j].x = ((rot[i][j].x << 8) / (256 + rot[i][j].z)) * XRATIO + X_S;
	    polyg[i][j].y = ((rot[i][j].y << 8) / (256 + rot[i][j].z)) * YRATIO + Y_S;
	    polyg[i][j].z = rot[i][j].z << 16;
	}
    for (i = 0; i < nFaces; i++) {
	ZZ = (polyg[i][2].x - polyg[i][0].x) * (polyg[i][1].y - polyg[i][0].y) -
	    (polyg[i][1].x - polyg[i][0].x) * (polyg[i][2].y - polyg[i][0].y);
	if (ZZ <= 0)
	    optfsp(i);
    }
}

void disp3d(void)
{
    int i, j;
    for (i = 0; i < nFaces; i++)
	for (j = 0; j < 3; j++) {
	    int x = obj[i][j].x + centerx, y = obj[i][j].y + centery,
	     z = obj[i][j].z + centerz;
	    rot[i][j].x = (z * coss[alfa] - x * sinn[alfa]) >> 16;
	    rot[i][j].z = (z * sinn[alfa] + x * coss[alfa]) >> 16;
	    x = (y * coss[beta] - rot[i][j].z * sinn[beta]) >> 16;
	    rot[i][j].z = (y * sinn[beta] + rot[i][j].z * coss[beta]) >> 16;
	    rot[i][j].y = x;
	    x = (rot[i][j].y * coss[gama] - rot[i][j].x * sinn[gama]) >> 16;
	    rot[i][j].x = (rot[i][j].y * sinn[gama] + rot[i][j].x * coss[gama]) >> 16;
	    rot[i][j].y = x;


	    x = obj[i][j].n_x + centerx, y = obj[i][j].n_y + centery, z = obj[i][j].n_z + centerz;
	    rot[i][j].n_x = (z * coss[alfa] - x * sinn[alfa]) >> 16;
	    rot[i][j].n_z = (z * sinn[alfa] + x * coss[alfa]) >> 16;
	    x = (y * coss[beta] - rot[i][j].z * sinn[beta]) >> 16;
	    rot[i][j].n_z = (y * sinn[beta] + rot[i][j].z * coss[beta]) >> 16;
	    rot[i][j].n_y = x;
	    x = (rot[i][j].n_y * coss[gama] - rot[i][j].n_x * sinn[gama]) >> 16;
	    rot[i][j].n_x = (rot[i][j].n_y * sinn[gama] + rot[i][j].n_x * coss[gama]) >> 16;
	    rot[i][j].n_y = x;
	}
    clear_zbuff();
    memset(data, 0, X_s * Y_s);
    show();
}

void patnikconstructor(void)
{
    long i, j;
    unsigned char table[256];
    for (i = 0; i < 256; i++) {
	table[i] = pow(i / 256.0, 2.0) * 256.0;
    }

    precalculate();
    nFaces = patniknFaces;
    obj = patnikobj;
    envmapa = (byte *) malloc(128 * 128);

    for (i = 0; i <= 127; i++)
	for (j = 0; j <= 127; j++)
	    *(envmapa + i * 128 + j) = table[(unsigned char) (0x0007A120 / ((i - 64) * (i - 64) + (j - 64) * (j - 64) + 2100))];
    memcpy(&rot, &obj, sizeof(obj));

}

void torusconstructor(void)
{
    long i, j;
    unsigned char table[256];
    for (i = 0; i < 256; i++) {
	table[i] = pow(i / 256.0, 3.0) * 200.0;
    }

    precalculate();
    nFaces = torusnFaces;
    obj = torusobj;
    envmapa = (byte *) malloc(128 * 128);

    for (i = 0; i <= 127; i++)
	for (j = 0; j <= 127; j++)
	    *(envmapa + i * 128 + j) = table[(unsigned char) (0x0007A120 / ((i - 64) * (i - 64) + (j - 64) * (j - 64) + 2100))];
    memcpy(&rot, &obj, sizeof(obj));

}

void destructor()
{
    free(envmapa);
}
