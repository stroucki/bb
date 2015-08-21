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

#include <sys/types.h>
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/nearptr.h>
#include "bb.h"
#include "mikdos/mikmod.h"


static int freq = 22050, stereo = 1, _16bit = 1;
static UNIMOD *mf;
static int interpolate;
int bbsound = 0;
int playing = 0;
int soundcounter;

void timeupdate(void)
{
    update_sound();
}
void tickhandler(void)
{
    MP_HandleTick();		/* play 1 tick of the module */
    MD_SetBPM(mp_bpm);
}


static int start_sound(void)
{
    md_dmabufsize = 50000;
    md_mode = 0;
    md_device = 0;
    md_mixfreq = freq;
    md_mode = (_16bit ? DMODE_16BITS : 0) | (stereo ? DMODE_STEREO : 0) | (interpolate ? DMODE_INTERP : 0);
    ML_RegisterLoader(&load_s3m);
    MD_RegisterDriver(&drv_pcsp);
    MD_RegisterDriver(&drv_ss);
    MD_RegisterDriver(&drv_sb);
    MD_RegisterDriver(&drv_gus);
    MD_RegisterPlayer(tickhandler);
    if (!MD_Init()) {
	fprintf(stderr, "Driver error: %s.\n", myerr);
	return 0;
    }
    bbsound = 1;

    return 1;
}
void stop()
{
    if (bbsound && playing) {
	playing = 0;
	MD_PlayStop();
	ML_Free(mf);
    }
}


int load_song(char *name)
{
    int i = TIME;
    finish_stuff = 0;
    if (bbsound) {
	bbsound = 1;
	if (playing)
	    stop();
	mf = ML_LoadFN(name);
	if (mf == NULL) {
	    fprintf(stderr, "bb_sound_server errror: %s\n", myerr);
	    return 1;
	}
	MP_Init(mf);
	md_numchn = mf->numchn;
	md_numchn = mf->numchn;
    }
    i -= TIME;
    starttime -= i;
    endtime -= i;

    return 0;
}
void update_sound()
{
    static int intimer;
    asm volatile ("cli");
    if (!intimer) {
	intimer = 1;
	asm volatile ("sti");
	if (bbsound == 1 && playing) {
	    if (MP_Ready()) {
		bbsound = 2;
		stop();
	    }
	    else
		MD_Update();
	}
	intimer = 0;
    }
    asm volatile ("sti");
}
#if 0
static void set_handler()
{
    _go32_dpmi_seginfo pmint;
    pmint.pm_selector = _my_cs();
    pmint.pm_offset = (int) update_sound;
    _go32_dpmi_chain_protected_mode_interrupt_vector(0x1c, &pmint);
}
#endif


void play()
{
    int t1;
    if (bbsound) {
	playing = 1;
	MD_PlayStart();
	MD_Update();
	if (md_driver != &drv_gus) {
	    t1 = md_mixfreq;
	    if (md_mode & DMODE_16BITS)
		t1 *= 2;
	    if (md_mode & DMODE_STEREO)
		t1 *= 2;
	    t1 = ((long long) 1000000) * (long long) md_dmabufsize / (long long) t1;
	}
	else
	    t1 = 0;
	if (md_driver != &drv_gus && md_driver != &drv_pcsp) {
	    static int h = 0;
	    if (!h) {
		h = 1;
		/*set_handler();*/
	    }
	}
	starttime += t1;
	endtime += t1;
    }
}

extern int __use_nearptr_hack;
int main(int argc, char *argv[])
{
    char s[255];
    int p = 0;
#if 0
    __djgpp_nearptr_enable();
    __use_nearptr_hack = 1;
#endif
    bbinit(argc, argv);
    aa_puts(context, 0, p++, AA_SPECIAL, "Music (GUS,SB,SS or PC speaker)?[Y/n]");
    aa_flush(context);
    if (tolower(aa_getkey(context, 1)) != 'n') {
	aa_puts(context, 0, p++, AA_SPECIAL, "Sample rate");
	do {
	    sprintf(s, "%i", freq);
	    aa_edit(context, 13, p - 1, 5, s, 6);
	} while (sscanf(s, "%i", &freq) == 0 || freq < 8000 || freq > 100000);
	aa_puts(context, 0, p++, AA_SPECIAL, "Stereo?[Y/n]");
	aa_flush(context);
	if (tolower(aa_getkey(context, 1)) == 'n')
	    stereo = 0;
	aa_puts(context, 0, p++, AA_SPECIAL, "16 bit?[Y/n]");
	aa_flush(context);
	if (tolower(aa_getkey(context, 1)) == 'n')
	    _16bit = 0;
	aa_puts(context, 0, p++, AA_SPECIAL, "Interpolation?[y/N]");
	aa_flush(context);
	if (tolower(aa_getkey(context, 1)) == 'y')
	    interpolate = 1;


	start_sound();
    }
    if (aa_mmwidth(context) > 300)
	dual = 1;
    bb();
    if (bbsound && playing) {
	MD_PlayStop();
	ML_Free(mf);
	MD_Exit();
    }
    return 0;
}
