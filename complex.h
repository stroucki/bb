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
#ifndef COMPLEX_H
#define COMPLEX_H

#define c_add_rp(ar,ai,br,bi) ((ar)+(br))
#define c_add_ip(ar,ai,br,bi) ((ai)+(bi))
#define c_add(ar,ai,br,bi,or,oi) ((or)=(ar)+(br),(oi)=(ai)+(bi))

#define c_sub_rp(ar,ai,br,bi) ((ar)-(br))
#define c_sub_ip(ar,ai,br,bi) ((ai)-(bi))
#define c_sub(ar,ai,br,bi,or,oi) ((or)=(ar)-(br),(oi)=(ai)-(bi))

#define c_mul(ar,ai,br,bi,or,oi) ((or)=(ar)*(br)-(ai)*(bi),(oi)=((ar)*(bi))+((ai)*(br)))
#define c_mul_rp(ar,ai,br,bi) ((ar)*(br)-(ai)*(bi))
#define c_mul_ip(ar,ai,br,bi) ((ar)*(bi)+(ai)*(br))

#define c_div_rp(ar,ai,br,bi) (((ar) * (br) + (ai) * (bi))/ ((bi) * (bi) + (br) * (br)))
#define c_div_ip(ar,ai,br,bi) ((-(ar) * (bi) + (ai) * (br)) / ((br) * (br) + (bi) * (bi)))
#define c_div(ar,ai,br,bi,or,oi) ((or)=c_div_rp(ar,ai,br,bi),(oi)=c_div_ip(ar,ai,br,bi))

#define c_pow2_rp(ar,ai) ((ar)*(ar)-(ai)*(ai))
#define c_pow2_ip(ar,ai) (2*(ar)*(ai))
#define c_pow2(ar,ai,or,oi) ((or)=c_pow2_rp(ar,ai),(oi)=c_pow2_ip(ar,ai))

#define c_pow3_rp(ar,ai) ((ar)*(ar)*(ar)-3*(ar)*(ai)*(ai))
#define c_pow3_ip(ar,ai) (3*(ar)*(ar)*(ai)-(ai)*(ai)*(ai))
#define c_pow3(ar,ai,or,oi) ((or)=c_pow3_rp(ar,ai),(oi)=c_pow3_ip(ar,ai))

#define c_pow4_rp(ar,ai) ((ar)*(ar)*(ar)*(ar)-6*(ar)*(ar)*(ai)*(ai)+(ai)*(ai)*(ai)*(ai))
#define c_pow4_ip(ar,ai) (4*(ar)*(ar)*(ar)*(ai)-4*(ar)*(ai)*(ai)*(ai))
#define c_pow4(ar,ai,or,oi) ((or)=c_pow4_rp(ar,ai),(oi)=c_pow4_ip(ar,ai))

#define square(x,y) ((x)*(x)+(y)*(y))
#define distance(x1,y1,x2,y2) square((x1)-(x2),(y1)-(y2))

#define myabs(x) ((x)>0?(x):-(x))

#define c_exp_rp(ar,ai) ((exp(ar))*(cos(ai)))
#define c_exp_ip(ar,ai) (sin(ai))
#define c_exp(ar,ai,or,oi) ((or)=(c_exp_rp(ar,ai)),(oi)=(c_exp_ip(ar,ai)))

/* Complex sin(const Complex &v)
   {    Complex u, i;
   i.c[0] = 0;
   i.c[1] = 1;
   u = (exp(i * v) - exp(i * (-v))) / (2 * i); 
   return u; } */

#define c_sin(ar, ai, or, oi) \
 { number_t _c_tmp_r1, _c_tmp_i1; \
   number_t _c_tmp_r2, _c_tmp_i2; \
   number_t _c_tmp_r3, _c_tmp_i3; \
	c_mul(0,1,ar,ai,_c_tmp_r1,_c_tmp_i1); \
	c_exp(_c_tmp_r1,_c_tmp_i1,_c_tmp_r2,_c_tmp_i2); \
	c_mul(0,1,-ar,-ai,or,oi); \
	c_exp(or,oi,_c_tmp_r1,_c_tmp_i1); \
	c_sub(_c_tmp_r2,_c_tmp_i2,_c_tmp_r1,_c_tmp_i1,_c_tmp_r3,_c_tmp_i3); \
	c_mul(2,0,0,1,_c_tmp_r1,_c_tmp_i1); \
	c_div(_c_tmp_r3,_c_tmp_i3,_c_tmp_r1,_c_tmp_i1,or,oi); }


#endif				/* COMPLEX_H */
