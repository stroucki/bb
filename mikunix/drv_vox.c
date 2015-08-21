/*

   Name:
   DRV_VOX.C

   Description:
   Mikmod driver for output on linux and FreeBSD Open Sound System (OSS)
   (/dev/dsp) 

   Portability:  VoxWare/SS/OSS land. Linux, FreeBSD (NetBSD & SCO?)

   New fragment configuration code done by Rao:
   ============================================

   You can use the environment variables 'MM_FRAGSIZE' and 'MM_NUMFRAGS' to 
   override the default size & number of audio buffer fragments. If you 
   experience crackles & pops, try experimenting with these values.

   Read experimental.txt within the VoxWare package for information on these 
   options. They are _VERY_ important with relation to sound popping and smooth
   playback.                                                        

   In general, the slower your system, the higher these values need to be. 

   MM_NUMFRAGS is within the range 2 to 255 (decimal)

   MM_FRAGSIZE is is within the range 7 to 17 (dec). The requested fragment size 
   will be 2^MM_FRAGSIZE

 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef __FreeBSD__
#include <machine/soundcard.h>
#else
#include <sys/soundcard.h>
#endif				/* __FreeBSD__ */
#include <sys/ioctl.h>
#include <sys/wait.h>
#include "mikmod.h"

#define DEFAULT_FRAGSIZE 10
#define DEFAULT_NUMFRAGS 7

static int sndfd;
static int fragmentsize;
static char *audiobuffer;


static BOOL Vox_IsThere(void)
{
    return (access("/dev/dsp", W_OK) == 0);
}


static BOOL Vox_Init(void)
{
    char *env;
    int play_precision, play_stereo, play_rate;
    int fragsize, numfrags;
    int deffrag = DEFAULT_FRAGSIZE;
    if (md_mode & DMODE_16BITS)
	deffrag++;
    if (md_mode & DMODE_STEREO)
	deffrag++;
    if (md_mixfreq > 44000)
	deffrag++;
    if (md_mixfreq < 17000)
	deffrag--;

    if ((sndfd = open("/dev/dsp", O_WRONLY)) < 0) {
	myerr = "Cannot open sounddevice";
	return 0;
    }
    fragsize = (env = getenv("MM_FRAGSIZE")) ? atoi(env) : deffrag;
    numfrags = (env = getenv("MM_NUMFRAGS")) ? atoi(env) : DEFAULT_NUMFRAGS;

    if (fragsize < 7 || fragsize > 17)
	fragsize = deffrag;
    if (numfrags < 2 || numfrags > 255)
	numfrags = DEFAULT_NUMFRAGS;

    fragmentsize = (numfrags << 16) | fragsize;

#ifndef __FreeBSD__
    if (ioctl(sndfd, SNDCTL_DSP_SETFRAGMENT, &fragmentsize) < 0) {
	myerr = "Buffer fragment failed";
	close(sndfd);
	return 0;
    }
#endif				/* __FreeBSD__ */

    play_precision = (md_mode & DMODE_16BITS) ? 16 : 8;
    play_stereo = (md_mode & DMODE_STEREO) ? 1 : 0;
    play_rate = md_mixfreq;

    if (ioctl(sndfd, SNDCTL_DSP_SAMPLESIZE, &play_precision) == -1 ||
	ioctl(sndfd, SNDCTL_DSP_STEREO, &play_stereo) == -1 ||
	ioctl(sndfd, SNDCTL_DSP_SPEED, &play_rate) == -1) {
	myerr = "Device can't play sound in this format";
	close(sndfd);
	return 0;
    }
    ioctl(sndfd, SNDCTL_DSP_GETBLKSIZE, &fragmentsize);

    if (!VC_Init()) {
	close(sndfd);
	return 0;
    }
    audiobuffer = (char *) MyMalloc(fragmentsize * sizeof(char));

    if (audiobuffer == NULL) {
	VC_Exit();
	close(sndfd);
	return 0;
    }
    return 1;
}


static void Vox_Exit(void)
{
    free(audiobuffer);
    VC_Exit();
    close(sndfd);
}


static void Vox_Update(void)
{
    VC_WriteBytes(audiobuffer, fragmentsize);
    write(sndfd, audiobuffer, fragmentsize);
}


DRIVER drv_vox =
{
    NULL,
    "Open Sound System (OSS)",
    "Open Sound System (OSS) Driver v1.2 - by Rao & MikMak",
    Vox_IsThere,
    VC_SampleLoad,
    VC_SampleUnload,
    Vox_Init,
    Vox_Exit,
    VC_PlayStart,
    VC_PlayStop,
    Vox_Update,
    VC_VoiceSetVolume,
    VC_VoiceSetFrequency,
    VC_VoiceSetPanning,
    VC_VoicePlay,
    MD_BlankFunction,
    MD_BlankFunction,
    MD_BlankFunction
};
