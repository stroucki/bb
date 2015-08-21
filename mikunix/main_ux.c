/*
 * BB: The portable demo
 *
 * (C) 1997 by AA-group (e-mail: aa@horac.ta.jcu.cz)
 *
 * 3rd August 1997
 * version: 1.0 [final]
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "mikmod.h"

static UNIMOD *mf;

void tickhandler(void)
{
    MP_HandleTick();		/* play 1 tick of the module */
    MD_SetBPM(mp_bpm);
}

static int load_sng(char *name)
{
    mf = ML_LoadFN(name);
    if (mf == NULL) {
	fprintf(stderr, "bb_sound_server errror: %s\n", myerr);
	putc('!', stdout);
	fflush(stdout);
	return 1;
    }
    MP_Init(mf);
    md_numchn = mf->numchn;
    md_numchn = mf->numchn;

    return 0;
}

int main(int argc, char *argv[])
{
    int t, i;
    md_dmabufsize = 10000;
    md_mode = 0;
    md_device = 0;

    if (argc != 7) {
	fprintf(stderr, "This program cannot be run in UNIX mode\n");
	exit(-1);
    }
    md_mixfreq = atol(argv[1]);
    if (argv[2][0] == '1')
	md_mode |= DMODE_16BITS;
    if (argv[3][0] == '1')
	md_mode |= DMODE_STEREO;
    ML_RegisterLoader(&load_s3m);

#ifdef SUN
    MD_RegisterDriver(&drv_sun);
#elif defined(SOLARIS)
    MD_RegisterDriver(&drv_sun);
#elif defined(__alpha)
    MD_RegisterDriver(&drv_AF);
#elif defined(OSS)
    MD_RegisterDriver(&drv_vox);
#ifdef ULTRA
    MD_RegisterDriver(&drv_ultra);
#endif				/* ULTRA */
#elif defined(__hpux)
    MD_RegisterDriver(&drv_hp);
#elif defined(AIX)
    MD_RegisterDriver(&drv_aix);
#elif defined(SGI)
    MD_RegisterDriver(&drv_sgi);
#endif
    MD_RegisterPlayer(tickhandler);

    if (!MD_Init()) {
	fprintf(stderr, "Driver error: %s.\n", myerr);
	putc('!', stdout);
	fflush(stdout);
	return 1;
    }
    putc('O', stdout);
    fflush(stdout);

    while (1) {
	char a;
	do {
	    a = getc(stdin);
	    if (a == '!') {
		MD_Exit();
		putc('!', stdout);
		return 0;
	    }
	} while (a < '0' || a > '3');
	if (load_sng(argv[4 + ((int) (a - '0'))])) {
	    ML_Free(mf);
	    MD_Exit();
	    exit(1);
	}
	MD_PlayStart();
	{
	    char ch;
	    ch = getc(stdin);
	    if (ch == '!') {
		MD_PlayStop();
		ML_Free(mf);
		MD_Exit();
		putc('!', stdout);
		return 0;
	    }
	    if (ch == 'T') {
		MD_PlayStop();
		ML_Free(mf);
		continue;
	    }
	}
	while (!MP_Ready()) {
	    fd_set readfds;
	    char ch;
	    struct timeval tv;

	    tv.tv_sec = 0;
	    tv.tv_usec = 0;
	    FD_ZERO(&readfds);
	    FD_SET(STDIN_FILENO, &readfds);
	    if (select(1, &readfds, NULL, NULL, &tv)) {
		ch = getc(stdin);
		if (ch == 'T') {
		    break;
		}
		if (ch == '!') {
		    MD_PlayStop();
		    ML_Free(mf);
		    MD_Exit();
		    putc('!', stdout);
		    return 0;
		}
	    }
	    MD_Update();
	}
	MD_PlayStop();		/* stop playing */
	ML_Free(mf);		/* and free the module */
    }
    MD_Exit();
    putc('!', stdout);
    return 0;
}
