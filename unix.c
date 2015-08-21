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
#include "bb.h"

static int soundin;
static int soundout;
static int freq = 22050, stereo = 1, _16bit = 1;
static int freqs[14] =
{
    5512, 6615, 8000, 9600, 11025, 16000, 18900,
    22050, 27428, 32000, 33075, 37800, 44100, 48000
};

int bbsound = 0;
int soundcounter;
static int isplaying=0;

void stop()
{
    if (bbsound) {
       write(soundout, "T", 1);
       isplaying=0;
    }
}

int load_song(char *name) 
{
    finish_stuff=0;
    if (bbsound) {
        stop();
	isplaying=1;
        bbsound=1;
	switch(name[2]) {
	case '.':
	    write(soundout, "0", 1);
	    break;
	case '2':
	    write(soundout, "1", 1);
	    break;
	case '3':
	    write(soundout, "2", 1);
	}
    }
    return 0;
}

static int start_sound(void)
{
    pid_t pid;
    int mypipe[2];
    int mypipe2[2];

    /* Create the pipe.  */
    if (pipe(mypipe) || pipe(mypipe2)) {
	fprintf(stderr, "Pipe failed.\n");
	return EXIT_FAILURE;
    }

    pid = fork();
    if (pid == (pid_t) 0) {	/* This is the child process.  */
	char str[256];
	sprintf(str, "bb_snd_server %i %i %i bb.s3m bb2.s3m bb3.s3m", freq, stereo, _16bit);
	close(mypipe[0]);
	close(mypipe2[1]);
	close(1);
	dup(mypipe[1]);
	close(0);
	dup(mypipe2[0]);
	fflush(stdout);
	if(system(str)) {
	  sprintf(str, "./bb_snd_server %i %i %i bb.s3m bb2.s3m bb3.s3m", freq, stereo, _16bit);
	  system(str);
	}
	write(mypipe[1], "!", 1);
	exit(0);
    }
    else if (pid > (pid_t) 0) {	/* This is the parent process.  */
	close(mypipe[1]);
	close(mypipe2[0]);
	soundin = mypipe[0];
	soundout = mypipe2[1];
	bbsound = 1;
	return EXIT_SUCCESS;
    }
    else {			/* The fork failed.  */
	fprintf(stderr, "Fork failed.\n");

	return EXIT_FAILURE;
    }
}
void play()
{
    if (bbsound) {
	write(soundout, "S", 1);
    }
}
void wait_sound()
{
    char ch;
    read(soundin, &ch, 1);
    if (ch == '!') {
	bbsound = 0;
    }
}
void update_sound()
{
    fd_set readfds;
    char ch;
    struct timeval tv;
    if (!bbsound)
	return;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(soundin, &readfds);
    while (select(soundin + 1, &readfds, NULL, NULL, &tv)) {
	soundcounter++;
	read(soundin, &ch, 1);
	if (ch == '!') {
	    bbsound = 0;
	    break;
	}
    }

}

int main(int argc, char *argv[])
{
    int retval;
    int p=0;
    char s[255];
    bbinit(argc,argv);
    aa_puts(context, 0, p++, AA_SPECIAL, "Music?[Y/n]");
    aa_flush(context);
    if (tolower(aa_getkey(context, 1)) != 'n') {
	aa_puts(context, 0, p, AA_SPECIAL, "Sample rate");
	do {
	    sprintf(s, "%i", freq);
	    aa_edit(context, 13, p++, 5, s, 6);
	} while (sscanf(s, "%i", &freq) == 0 || freq < 8000 || freq > 100000);
{int i,minv=freqs[0];
 for(i=0;i<14;i++)
   if(abs(freq-minv)>abs(freq-freqs[i])) minv=freqs[i];
freq=minv;
}
	aa_puts(context, 0, p++, AA_SPECIAL, "Stereo?[Y/n]");
	aa_flush(context);
	if (tolower(aa_getkey(context, 1)) == 'n')
	    stereo = 0;
	aa_puts(context, 0, p++, AA_SPECIAL, "16 bit?[Y/n]");
	aa_flush(context);
	if (tolower(aa_getkey(context, 1)) == 'n')
	    _16bit = 0;

	start_sound();
	wait_sound();
    }
    retval = bb();
    if (bbsound) {
	write(soundout, "!", 1);
	close(soundout);
	close(soundin);
    }
    return retval;
}
