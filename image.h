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

#ifndef IMAGE_H
#define IMAGE_H
struct image {
  unsigned char *data;
  int size;
  int width,height;
  unsigned char *decompressed;
};
extern struct image fk1,fk2,fk3,fk4;
extern struct image hh1,hh2,hh3,hh4;
extern struct image kt1,kt2,kt3,kt4;
extern struct image ms1,ms2,ms3,ms4;
#endif
