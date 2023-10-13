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

#include <stdlib.h>
#include <aalib.h>
#include "bb.h"

struct font *uncompressfont(const aa_font * font)
{
    struct font *ufont = malloc(sizeof(struct font));
    int i, y;
    ufont->width = 8;
    ufont->height = font->height;
    ufont->data = calloc(1, 256 * 8 * font->height);
    for (i = 0, y = 0; i < 256 * font->height; i++, y += 8) {
	char c = font->data[i];
	ufont->data[y] = c & (1 << 7);
	ufont->data[y + 1] = c & (1 << 6);
	ufont->data[y + 2] = c & (1 << 5);
	ufont->data[y + 3] = c & (1 << 4);
	ufont->data[y + 4] = c & (1 << 3);
	ufont->data[y + 5] = c & (1 << 2);
	ufont->data[y + 6] = c & (1 << 1);
	ufont->data[y + 7] = c & (1 << 0);
    }
    return (ufont);

}
