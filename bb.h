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

#ifndef BB_H
#define BB_H
#include <aalib.h>
#include "image.h"
#include "timers.h"
struct font {
    int width, height;
    unsigned char *data;
};

#define clrscr() memset(context->imagebuffer,0,aa_imgwidth(context)*aa_imgheight(context))
#define textclrscr()     memset(context->textbuffer,' ',aa_scrwidth(context)*aa_scrheight(context)), memset(context->attrbuffer,AA_NORMAL,aa_scrwidth(context)*aa_scrheight(context))

extern int finish_stuff, starttime, endtime;
extern aa_context *context;
extern int dual;
extern int TIME;
extern aa_renderparams *params;
extern tl_timer *scenetimer;
extern void (*drawptr) (void);
extern struct font *font;
extern void blur(int);
extern int bbsound, soundcounter;


void dvojprujezd(int, char *, char *);
void timestuff(int rate, void (*control) (int), void (*draw) (void), int maxtime);
void centerprint(int x, int y, double size, int color, char *text,int mode);
double getwidth(double);
void centerprinth(int x, int y, double size, int color, char *text,int mode);

struct font *uncompressfont(const aa_font * font);
void fastscale(unsigned char *b1, unsigned char *b2, int x1, int x2, int y1, int y2, int width1, int width2, int color);
void print(int x, int y, float width, int height, struct font *f, int color, char *text);
void draw(void);
void bbwait(int);
void bbflushwait(int);
void scene1(void);
void scene2(void);
void scene3(void);
void scene4(void);
void scene5(void);
void scene6(void);
void scene7(void);
void scene8(void);
void scene9(void);
void scene10(void);
void credits(void);
void credits2(void);
void devezen1(void);
void devezen2(void);
void devezen3(void);
void devezen4(void);
void strobikstart(void);
void strobikend(void);
void backconvert(int, int, int, int);
void introscreen(void);
int bbupdate();
void initscene3(void);
void message(char *text, int starttime);
void ctrllepic(int i);
int bb(void);
int bbinit(int, char **);
int load_song(char *);
void messager(char *c);
void drawlepic(char *mesg);
void initlepic(void);
void drawzoomer(char *mesg, int starttime, int pos);
void drawlevotoc(char *mesg, char *mesg1, int starttime);
void drawpravotoc(char *mesg, char *mesg1, int starttime);
void drawhorotoc(char *mesg, char *mesg1, int starttime);
void drawprujezd(char *mesg, int starttime);
void dispimg(struct image *image,int);
void vezen(struct image *i1, struct image *i2, struct image *i3, struct image *i4);
void play();
void update_sound();
void wait_sound();
unsigned char *decompressimg(struct image *image);
void scale(struct image *image, int x1, int y1, int x2, int y2);
extern int textsize;
extern char *line[];
extern void format(int);

#endif
